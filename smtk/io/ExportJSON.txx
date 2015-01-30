#ifndef __smtk_io_ExportJSON_txx
#define __smtk_io_ExportJSON_txx

#include "smtk/io/ExportJSON.h"

#include "smtk/model/CellEntity.h"
#include "smtk/model/GroupEntity.h"
#include "smtk/model/InstanceEntity.h"
#include "smtk/model/ModelEntity.h"
#include "smtk/model/ShellEntity.h"
#include "smtk/model/UseEntity.h"

#include "cJSON.h"

namespace smtk {
  namespace io {

/**\brief Populate the \a json node with the record(s) related to given \a entities.
  *
  */
template<typename T>
int ExportJSON::forEntities(
  cJSON* json,
  const T& entities,
  JSONRecords relatedEntities,
  JSONFlags sections)
{
  using namespace smtk::model;

  if (!json || sections == JSON_NOTHING)
    return 1;

  typename T::const_iterator eit = entities.begin();
  std::set<Cursor> queue;
  // If we are asked to return all the entities of the related model(s),
  // find the owning model
  smtk::model::ModelEntity parent;
  if (relatedEntities == JSON_MODELS)
    for (typename T::const_iterator rit = entities.begin(); rit != entities.end(); ++rit)
      if ((parent = rit->owningModel()).isValid())
        queue.insert(parent);
      else
        queue.insert(*rit); // Well, if it doesn't have a parent, at least make sure it's included.
  else
    queue.insert(entities.begin(), entities.end());

  Cursors visited;
  int status = 1;
  while (!queue.empty())
    {
    // Pull the first entry off the queue.
    Cursor ent = *queue.begin();
    queue.erase(queue.begin());

    // Generate JSON for the queued entity
    ManagerPtr modelMgr = ent.manager();
    UUIDWithEntity it = modelMgr->topology().find(ent.entity());
    if (
      (it == ent.manager()->topology().end()) ||
      ((it->second.entityFlags() & BRIDGE_SESSION) && !(sections & JSON_BRIDGES)))
      continue;

    cJSON* curChild = cJSON_CreateObject();
      {
      std::string suid = it->first.toString();
      cJSON_AddItemToObject(json, suid.c_str(), curChild);
      }
    if (sections & JSON_ENTITIES)
      {
      status &= ExportJSON::forManagerEntity(it, curChild, modelMgr);
      status &= ExportJSON::forManagerArrangement(
        modelMgr->arrangements().find(it->first), curChild, modelMgr);
      }
    if (sections & JSON_TESSELLATIONS)
      status &= ExportJSON::forManagerTessellation(it->first, curChild, modelMgr);
    if (sections & JSON_PROPERTIES)
      {
      status &= ExportJSON::forManagerFloatProperties(it->first, curChild, modelMgr);
      status &= ExportJSON::forManagerStringProperties(it->first, curChild, modelMgr);
      status &= ExportJSON::forManagerIntegerProperties(it->first, curChild, modelMgr);
      }

    // Now push any requested relations to queue as needed and mark the entry as visited
    switch (relatedEntities)
      {
      // Both children and models fetch the same related entities...
      // but JSON_MODEL starts with a different initial queue:
    case JSON_MODELS:
    case JSON_CHILDREN:
        {
        Cursors children;
        if (ent.isCellEntity())
          {
          children = ent.boundaryEntities();
          }
        else if (ent.isUseEntity())
          {
          children = ent.as<UseEntity>().shellEntities<Cursors>();
          }
        else if (ent.isShellEntity())
          {
          children = ent.as<ShellEntity>().uses<Cursors>();
          }
        else if (ent.isGroupEntity())
          {
          children = ent.as<GroupEntity>().members<Cursors>();
          }
        else if (ent.isModelEntity())
          { // Grrr....
          CellEntities mcells = ent.as<smtk::model::ModelEntity>().cells();
          children.insert(mcells.begin(), mcells.end());

          GroupEntities mgroups = ent.as<smtk::model::ModelEntity>().groups();
          children.insert(mgroups.begin(), mgroups.end());

          ModelEntities msubmodels = ent.as<smtk::model::ModelEntity>().submodels();
          children.insert(msubmodels.begin(), msubmodels.end());
          }
        for (Cursors::const_iterator cit = children.begin(); cit != children.end(); ++cit)
          if (visited.find(*cit) == visited.end())
            queue.insert(*cit);
        }
      break;
    case JSON_BARE: // Add nothing to the list of requested entities
    default:
      break; // do nothing.
      }
    visited.insert(ent);
    }
  return 0;
}

/**\brief Populate the \a json node with the record(s) related to given \a entities.
  *
  */
template<typename T>
std::string ExportJSON::forEntities(
  const T& entities,
  JSONRecords relatedEntities,
  JSONFlags sections)
{
  using namespace smtk::model;

  cJSON* top = cJSON_CreateObject();
  ExportJSON::forEntities(top, entities, relatedEntities, sections);
  char* json = cJSON_Print(top);
  std::string result(json);
  free(json);
  cJSON_Delete(top);
  return result;
}

  } // namespace io
} // namespace smtk

#endif // __smtk_io_ExportJSON_txx
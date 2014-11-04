//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#include "smtk/AutoInit.h"

#include "smtk/io/AttributeReader.h"
#include "smtk/io/Logger.h"

#include "smtk/attribute/Attribute.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/IntItem.h"

#include "smtk/model/Bridge.h"
#include "smtk/model/DefaultBridge.h"
#include "smtk/model/ModelEntity.h"
#include "smtk/model/Operator.h"
#include "smtk/model/Manager.h"

#include "smtk/common/testing/cxx/helpers.h"
#include "smtk/model/testing/cxx/helpers.h"

#include "smtk/Options.h"

// Encoded XML describing the operator classes below.
// These are generated by CMake from unitXXXOperator.sbt.
#include "unitOutcomeOperator_xml.h"

using namespace smtk::model;
using smtk::attribute::IntItem;

template<typename T>
void printVec(const std::vector<T>& v, const char* typeName, char sep = '\0')
{
  if (v.empty()) return;
  std::cout << " " << typeName << "(" << v.size() << "): [";
  std::cout << " " << v[0];
  if (sep)
    for (typename std::vector<T>::size_type i = 1; i < v.size(); ++i)
      std::cout << sep << " " << v[i];
  else
    for (typename std::vector<T>::size_type i = 1; i < v.size(); ++i)
      std::cout << " " << v[i];
  std::cout << " ]";
}


int WillOperateWatcher(OperatorEventType event, const Operator& op, void* user)
{
  test(event == WILL_OPERATE, "WillOperate callback invoked with bad event type");
  int shouldCancel = *reinterpret_cast<int*>(user);
  std::cout
    << "Operator " << op.name() << " about to run: "
    << (shouldCancel ? "will" : "will not") << " request cancellation.\n";
  return shouldCancel;
}

int DidOperateWatcher(OperatorEventType event, const Operator& op, OperatorResult result, void* user)
{
  test(event == DID_OPERATE, "DidOperate callback invoked with bad event type");
  IntItem::Ptr outcome = result->findInt("outcome");
  std::cout
    << "Operator " << op.name() << " did operate"
    << " (outcome = \"" << outcomeAsString(outcome->value()) << "\")\n";
  // increment the number of times the parameter was modified.
  (*reinterpret_cast<int*>(user))++;
  return 0;
}

void testBridgeList(Manager::Ptr manager)
{
  std::cout
    << "Default bridge is \""
    << manager->bridgeForModel(smtk::common::UUID::null())->name()
    << "\"\n";
  std::cout << "Available bridges\n";
  StringList bridges = manager->bridgeNames();
  for (StringList::iterator it = bridges.begin(); it != bridges.end(); ++it)
    std::cout << "  " << *it << "\n";
  std::cout << "\n";
}

class TestOutcomeOperator : public Operator
{
public:
  smtkTypeMacro(TestOutcomeOperator);
  smtkCreateMacro(TestOutcomeOperator);
  smtkSharedFromThisMacro(Operator);
  smtkDeclareModelOperator();

  TestOutcomeOperator()
    : m_able(false) // fail operation until told otherwise
    {
    }
  virtual bool ableToOperate()
    {
    this->ensureSpecification();
    return
      // Force failure?
      this->m_able &&
      this->m_specification &&
      this->m_specification->isValid();
    }
  bool m_able; // Used to force UNABLE_TO_OPERATE result.

protected:
  virtual OperatorResult operateInternal()
    {
    return this->createResult(
      this->specification()->findInt("shouldSucceed")->value() ?
      OPERATION_SUCCEEDED : OPERATION_FAILED);
    }
};

// Implementation corresponding to smtkDeclareModelOperator() above.
smtkImplementsModelOperator(
  TestOutcomeOperator,
  OutcomeOp,
  "outcome test",
  unitOutcomeOperator_xml,
  smtk::model::DefaultBridge);

void testImportOperators(Manager::Ptr manager)
{
  // Add operator descriptions to the default bridge of our manager.
  smtk::model::BridgePtr bridge =
    manager->bridgeForModel(smtk::common::UUID::null());
  smtk::io::Logger log;
  smtk::io::AttributeReader rdr;
  rdr.setReportDuplicateDefinitionsAsErrors(false);
  if (rdr.readContents(
    *bridge->operatorSystem(), unitOutcomeOperator_xml, sizeof(unitOutcomeOperator_xml), log))
    {
    std::cerr << "Error. Log follows:\n---\n" << log.convertToString() << "\n---\n";
    throw std::string("Could not parse operator XML.");
    }
  if (log.numberOfRecords())
    {
    std::cout << "  " << log.convertToString() << "\n";
    }

  // Now enumerate attribute definitions that inherit "operator".
  smtk::attribute::DefinitionPtr opBase =
    bridge->operatorSystem()->findDefinition("operator");
  typedef std::vector<smtk::attribute::DefinitionPtr> OpListType;
  OpListType opList;
  bridge->operatorSystem()->derivedDefinitions(opBase, opList);
  std::cout << "Imported XML for operators:\n";
  for (OpListType::iterator it = opList.begin(); it != opList.end(); ++it)
    {
    std::cout << "  \"" << (*it)->type() << "\"\n";
    }
  std::cout << "\n";

  if (opList.size() != 1)
    {
    std::ostringstream err;
    err << "Error: Expected 1 operator, found " << opList.size() << "\n";
    std::cerr << err.str();
    throw err.str();
    }
}

void testOperatorOutcomes(Manager::Ptr manager)
{
  TestOutcomeOperator::Ptr op = smtk::dynamic_pointer_cast<TestOutcomeOperator>(
    manager->bridgeForModel(smtk::common::UUID::null())->op("outcome test", manager));

  int shouldCancel = 1;
  int numberOfFailedOperations = 0;
  op->observe(WILL_OPERATE, WillOperateWatcher, &shouldCancel);
  op->observe(DID_OPERATE, DidOperateWatcher, &numberOfFailedOperations);

  // Verify that ableToOperate() is called and complains:
  test(op->operate()->findInt("outcome")->value() == UNABLE_TO_OPERATE, "Should have been unable to operate.");
  std::cout
    << "Operator " << op->name() << " should be unable to operate"
    << " (outcome = \"" << outcomeAsString(op->operate()->findInt("outcome")->value()) << "\""
    << ").\n--\n";

  // Verify that the WILL_OPERATE observer is called and cancels the operation:
  op->m_able = true;
  test(op->operate()->findInt("outcome")->value() == OPERATION_CANCELED, "Operation should have been canceled.");
  std::cout << "--\n";

  // Verify that the operation fails when "shouldSucceed" parameter is zero.
  shouldCancel = 0;
  test(op->operate()->findInt("outcome")->value() == OPERATION_FAILED, "Operation should have failed.");
  std::cout << "--\n";

  // Verify that removing observer bypasses cancellation.
  op->unobserve(WILL_OPERATE, WillOperateWatcher, &shouldCancel);
  shouldCancel = 1; // Force cancellation if observer wasn't removed.

  // Verify that the parameter-change callback is invoked.
  op->specification()->findInt("shouldSucceed")->setValue(1);

  // Verify that the operation succeeds when "shouldSucceed" parameter is non-zero.
  test(op->operate()->findInt("outcome")->value() == OPERATION_SUCCEEDED, "Operation should have succeeded.");
  std::cout << "--\n";

  // Now test values passed to callbacks:
  test(numberOfFailedOperations > 0, "Expected more operator failures.");

  op->unobserve(DID_OPERATE, DidOperateWatcher, &numberOfFailedOperations);
}

void testBridgeAssociation(Manager::Ptr manager)
{
  // Test that operators added by previous tests still exist
  ModelEntity model = manager->addModel(3, 3, "Model Airplane");
  test(model.operatorNames().size() == 1, "Expected 1 operators defined for the test model.");

  // Test op(name) method
  Operator::Ptr op = model.op("outcome test");
  test(op ? 1 : 0, "ModelEntity::op(\"outcome test\") returned a \"null\" shared pointer.");

  // Test Operator->Bridge association
  test(op->bridge() == manager->bridgeForModel(smtk::common::UUID::null()).get(),
    "Bad bridge reported by operator.");

  // Test Operator->Manager association
  test(op->manager() == manager, "Bad manager reported by operator.");

  // Test operatorNames()
  StringList opNames = model.bridge()->operatorNames();
  std::cout << "Printing";
  printVec(opNames, "operator names", ',');
  std::cout << "\n";
}

int main()
{
  int status = 0;

  Manager::Ptr manager = Manager::create();

  try {

    testBridgeList(manager);
    testImportOperators(manager);
    testOperatorOutcomes(manager);
    testBridgeAssociation(manager);

  } catch (const std::string& msg) {
    (void) msg; // Ignore the message; it's already been printed.
    std::cerr << "Exiting...\n";
    status = -1;
  }

  return status;
}
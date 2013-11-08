#ifndef __smtk_vtk_ModelRepresentation_h
#define __smtk_vtk_ModelRepresentation_h

#include "smtk/vtkSMTKExports.h"
#include "smtk/PublicPointerDefs.h"

#include "vtkRenderedRepresentation.h"

class vtkActor;
class vtkApplyColors;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTransformFilter;

/**\brief Generate a VTK dataset used to represent an SMTK model.
  *
  * This requires the model to have per-body or per-face tessellation information.
  */
class VTKSMTK_EXPORT vtkSMTKModelRepresentation : public vtkRenderedRepresentation
{
public:
  static vtkSMTKModelRepresentation* New();
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  vtkTypeMacro(vtkSMTKModelRepresentation,vtkRenderedRepresentation);

  /**\brief Get/set the selection mask.
    *
    * The selection mask is bit-wise ANDed with each selected entity's flags
    * before that entity is added to the pedigree ID selection output by
    * ConvertSelection().
    *
    * This can be used to force only edges, faces, or vertices to be selected.
    */
  //@{
  vtkGetMacro(SelectionMask,int);
  vtkSetMacro(SelectionMask,int);
  //@}

  /// Set/get the model (used for selection masking). This is really a hack.
  //@{
  virtual void SetModel(smtk::model::ModelBodyPtr model)
    {
    if (this->Model == model)
      return;
    this->Model = model;
    this->Modified();
    }
  smtk::model::ModelBodyPtr GetModel()
    { return this->Model; }
  //@}

  virtual void ApplyViewTheme(vtkViewTheme* theme);

  vtkGetObjectMacro(Transform,vtkTransformFilter);
  vtkGetObjectMacro(ApplyColors,vtkApplyColors);
  vtkGetObjectMacro(Mapper,vtkPolyDataMapper);
  vtkGetObjectMacro(Actor,vtkActor);

protected:
  vtkSMTKModelRepresentation();
  virtual ~vtkSMTKModelRepresentation();

  //virtual int FillInputPortInformation(int port, vtkInformation* request);
  //virtual int FillOutputPortInformation(int port, vtkInformation* request);

  virtual int RequestData(
    vtkInformation* request,
    vtkInformationVector** inInfo,
    vtkInformationVector* outInfo);

  virtual void PrepareForRendering(vtkRenderView* view);
  virtual bool AddToView(vtkView* view);
  virtual bool RemoveFromView(vtkView* view);
  virtual vtkSelection* ConvertSelection(vtkView* view, vtkSelection* selection);

  void SetTransform(vtkTransformFilter*);
  void SetApplyColors(vtkApplyColors*);
  void SetMapper(vtkPolyDataMapper*);
  void SetActor(vtkActor*);

  // Instance storage:
  vtkTransformFilter* Transform;
  vtkApplyColors* ApplyColors;
  vtkPolyDataMapper* Mapper;
  vtkActor* Actor;
  int SelectionMask;
  smtk::model::ModelBodyPtr Model;

private:
  vtkSMTKModelRepresentation(const vtkSMTKModelRepresentation&); // Not implemented.
  void operator = (const vtkSMTKModelRepresentation&); // Not implemented.
};

#endif // __smtk_vtk_ModelRepresentation_h

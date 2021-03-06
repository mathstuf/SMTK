#=============================================================================
#
#  Copyright (c) Kitware, Inc.
#  All rights reserved.
#  See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=============================================================================

set(unit_tests_which_require_data
  UnitTestVTKDataConverter.cxx
  )

smtk_unit_tests(LABEL "Mesh"
                SOURCES_REQUIRE_DATA ${unit_tests_which_require_data}
                LIBRARIES vtkToSMTKMesh vtkFiltersSources)



add_executable(MultiScaleConverter MultiScaleConverter.cxx)
target_link_libraries(MultiScaleConverter
                      LINK_PUBLIC vtkToSMTKMesh
                                  vtkIOExodus vtkIOXML vtksys)
target_include_directories(MultiScaleConverter PRIVATE ${MOAB_INCLUDE_DIRS})

add_executable(SurfacePerMaterialExtraction SurfacePerMaterialExtraction.cxx)

target_link_libraries(SurfacePerMaterialExtraction
                      LINK_PUBLIC vtkToSMTKMesh
                                  vtkIOExodus vtkIOXML vtksys)
target_include_directories(SurfacePerMaterialExtraction PRIVATE ${MOAB_INCLUDE_DIRS})

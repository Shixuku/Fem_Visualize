#pragma once
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkScalarBarActor.h>
#include <vtkProperty.h>
#include <vtkAutoInit.h>
#include <vtkTransform.h>
#include <vtkAppendPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkLine.h>
#include <vtkVertexGlyphFilter.h>
#include <map>

#include "NodeFem.h"
#include "StructureFem.h"
using namespace std;

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)

class VisualizeWindow
{
public:
	VisualizeWindow(StructureFem& structure);
	~VisualizeWindow();

	void SetStructure(StructureFem& structure);

private:
	void InitWindow();
	void InitNode(std::map<int, NodeFem*> nodes);
	void InitElement();

	/* 构建旋转矩阵 */
	void ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix);

	/* 创建矩形截面 */
	void CreateRecSection(double length, double width, double stratPoint[3], double endPoint[3]);

	vtkNew<vtkNamedColors> colors;
	vtkNew<vtkActor> actor;

	// A renderer and render window
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkRenderWindow> renderWindow;
	// An interactor
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkNew<vtkPoints> points;
	vtkNew<vtkAppendPolyData> appendFilter;

	
	StructureFem m_structure;
};


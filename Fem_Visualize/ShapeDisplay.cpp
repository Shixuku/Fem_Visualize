#include "ShapeDisplay.h"

LineDisplay::LineDisplay()
{
}

void LineDisplay::CreateCell(vtkSmartPointer<vtkPoints> points)
{
	double startPoint[3];
	double endPoints[3];


	// 创建vtkPolyLine对象并设置点集
	vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	polyLine->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());

	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		polyLine->GetPointIds()->SetId(i, i);   // 第一个点的索引为0
	}
	// 创建vtkCellArray对象并添加vtkPolyLine对象
	vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
	cellArray->InsertNextCell(polyLine);

	// 创建vtkPolyData对象并设置线集
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(cellArray);

	vtkNew<vtkVertexGlyphFilter> vertexfilter;
	vertexfilter->AddInputData(polyData);
	vertexfilter->Update();

	appendPolyData->AddInputConnection(vertexfilter->GetOutputPort());
	appendPolyData->AddInputData(polyData);
	appendPolyData->Update();
}

void LineDisplay::LinkActor()
{
	mapper->SetInputData(appendPolyData->GetOutput());
	actor->SetMapper(mapper);

	//actor->GetProperty()->SetLineWidth(2);
	actor->GetProperty()->SetColor(0, 0, 1.0);
	actor->GetProperty()->SetPointSize(5);
}

vtkSmartPointer<vtkActor> ShapeDisplay::GetActor()
{
	return actor;
}

void PointsDisplay::CreateCell(vtkSmartPointer<vtkPoints> points)
{

	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		// Create sphere for glyphing
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetRadius(0.5); // radius of spheres
		sphereSource->SetThetaResolution(20);
		sphereSource->SetPhiResolution(20);
		sphereSource->SetCenter(points->GetPoint(i));

		appendPolyData->AddInputConnection(sphereSource->GetOutputPort());
		appendPolyData->Update();
	}
}

void PointsDisplay::LinkActor()
{
	mapper->SetInputData(appendPolyData->GetOutput());
	actor->SetMapper(mapper);

	actor->GetProperty()->SetColor(0, 0, 1);
}

PointsDisplay::PointsDisplay()
{

}

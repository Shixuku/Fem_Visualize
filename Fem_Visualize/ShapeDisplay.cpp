#include "ShapeDisplay.h"

LineDisplay::LineDisplay()
{
}

void LineDisplay::CreateCell(vtkSmartPointer<vtkPoints> points)
{
	double startPoint[3];
	double endPoints[3];


	// ����vtkPolyLine�������õ㼯
	vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
	polyLine->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());

	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		polyLine->GetPointIds()->SetId(i, i);   // ��һ���������Ϊ0
	}
	// ����vtkCellArray�������vtkPolyLine����
	vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
	cellArray->InsertNextCell(polyLine);

	// ����vtkPolyData���������߼�
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

#include "VisualizeWindow.h"

VisualizeWindow::VisualizeWindow(StructureFem& structure)
{
	InitWindow();
	SetStructure(structure);
	InitNode(structure.m_Nodes);
	InitElement();
}

void VisualizeWindow::SetStructure(StructureFem& structure)
{
	m_structure = structure;
}

void VisualizeWindow::InitWindow()
{

	renderWindow->AddRenderer(renderer);
	renderWindow->SetWindowName("RubberBandZoom");

	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Add the actors to the scene
	renderer->AddActor(actor);
	renderWindow->SetWindowName("RubberBandZoom");
	renderWindow->SetSize(900, 600);

	renderer->SetBackground(colors->GetColor3d("White").GetData());
}

void VisualizeWindow::InitNode(std::map<int, NodeFem*> nodes)
{
	double point[3];

	std::cout << "window" << std::endl;
	for (auto node : nodes)
	{
		NodeFem *fem = node.second;
		point[0] = fem->m_x;
		point[1] = fem->m_y;
		point[2] = fem->m_z;
		points->InsertPoint(fem->m_id - 1, point);
		std::cout << "x:" << point[0] << "y:" << point[1] << "z:" << point[2] << " id" << fem->m_id << std::endl;
	}
	vtkNew<vtkPolyData> polydata;
	polydata->SetPoints(points);

	vtkNew<vtkVertexGlyphFilter> vertex;
	vertex->SetInputData(polydata);

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(vertex->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);

	actor->GetProperty()->SetPointSize(4);
	actor->GetProperty()->SetColor(0, 0, 0);

	renderer->AddActor(actor);
	renderWindow->Render();
}

void VisualizeWindow::InitElement()
{
	vtkNew <vtkCellArray> cellArray;
	for (auto element : m_structure.m_Element)
	{
		vtkNew<vtkLine> line;
		line->GetPointIds()->SetId(0, element.second->m_idNode.at(0) - 1);
		line->GetPointIds()->SetId(1, element.second->m_idNode.at(1) - 1);
		cellArray->InsertNextCell(line);
	}
	double startPoint[3];
	double endPoint[3];
	int startNodeIndex;
	int endNodeIndex;

	for (auto element : m_structure.m_Element)
	{
		startNodeIndex = element.second->m_idNode.at(0);
		endNodeIndex = element.second->m_idNode.at(1);

		NodeFem* startNode = m_structure.Find_Node(startNodeIndex);
		NodeFem* endNode = m_structure.Find_Node(endNodeIndex);

		startPoint[0] = startNode->m_x;
		startPoint[1] = startNode->m_y;
		startPoint[2] = startNode->m_z;

		endPoint[0] = endNode->m_x;
		endPoint[1] = endNode->m_y;
		endPoint[2] = endNode->m_z;
		
		CreateRecSection(0.2, 0.1, startPoint, endPoint);
	}

	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points);
	polyData->SetLines(cellArray);

	vtkNew<vtkDoubleArray> stress;
	stress->SetNumberOfComponents(1);
	stress->InsertNextValue(10);
	stress->InsertNextValue(20);
	stress->InsertNextValue(5);
	stress->InsertNextValue(60);
	polyData->GetPointData()->SetScalars(stress);

	vtkNew<vtkPolyDataMapper> mapper;
	//mapper->SetInputData(polyData);
	mapper->SetInputConnection(appendFilter->GetOutputPort());
	mapper->SetScalarRange(0, 90);
	mapper->SetScalarModeToUsePointData();

	vtkNew<vtkScalarBarActor> scalarBar;
	scalarBar->SetLookupTable(mapper->GetLookupTable());
	scalarBar->SetTitle("Stress");
	//scalarBar->SetNumberOfLabels(20);

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	//actor->GetProperty()->EdgeVisibilityOn();

	actor->GetProperty()->SetLineWidth(2);

	//renderer->RemoveAllViewProps();

	renderer->AddActor(actor);
	renderer->AddActor(scalarBar);
	renderWindow->Render();
	renderWindowInteractor->Start();

}

void VisualizeWindow::ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix)
{
	// 计算连接这两个点的向量
	double normal[3] = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2] };

	// 定义两个向量
	double sourceVector[3] = { 0.0, 0.0, 1.0 };
	double targetVector[3] = { normal[0], normal[1], normal[2] };

	// 标准化两个向量
	vtkMath::Normalize(sourceVector);
	vtkMath::Normalize(targetVector);

	// 计算旋转轴和旋转角度
	// 两个向量的点积等于一个向量在另一个向量方向上的投影长度与另一个向量本身长度的乘积，反映了一个向量在另一个向量方向上的分量大小。
	// 除以两个向量的长度即可得到两个向量的夹角
	double rotationAxis[3];
	vtkMath::Cross(sourceVector, targetVector, rotationAxis);
	double rotationAngle = acos(vtkMath::Dot(sourceVector, targetVector));

	// 构建旋转矩阵
	vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
	rotation->RotateWXYZ(rotationAngle * vtkMath::DegreesFromRadians(1.0), rotationAxis);

	// 构建平移矩阵
	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	// translation->Translate(startPoint);

	// 构建旋转和平移的组合矩阵
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Concatenate(translation);
	transform->Concatenate(rotation);

	// 获取变换矩阵
	transform->GetMatrix(transformMatrix);
}

void VisualizeWindow::CreateRecSection(double length, double width, double startPoint[3], double endPoint[3])
{
	vtkNew<vtkPolyData> polyData;

	vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ConstuctRotationMatrix(startPoint, endPoint, transformMatrix);

	// create four points for the cross section, centered at the origin
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->InsertNextPoint(-length / 2, -width / 2, 0);
	points->InsertNextPoint(length / 2, -width / 2, 0);
	points->InsertNextPoint(length / 2, width / 2, 0);
	points->InsertNextPoint(-length / 2, width / 2, 0);

	vtkNew<vtkDoubleArray> stress;
	stress->SetNumberOfComponents(1);

	double value = vtkMath::Random(0, 90);
	// transform the points using the rotation matrix and the center point
	for (vtkIdType i = 0; i < 4; i++)
	{
		double point[3];
		points->GetPoint(i, point);
		transformMatrix->MultiplyPoint(point, point);
		point[0] += startPoint[0];
		point[1] += startPoint[1];
		point[2] += startPoint[2];
		points->SetPoint(i, point);
	}


	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType vertices[4] = { 0, 1, 2, 3 };
	cells->InsertNextCell(4, vertices);

	// Create a polydata object for the cross-sectional shape
	polyData->SetPoints(points);
	polyData->SetPolys(cells);

	vtkNew<vtkLinearExtrusionFilter> extrusion;
	extrusion->SetInputData(polyData);
	// 设置第一个截面的拉伸方向
	extrusion->SetExtrusionTypeToNormalExtrusion();
	extrusion->SetVector(endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2]);
	extrusion->Update();

	double nextValue = vtkMath::Random(0, 90);
	double thirdValue = vtkMath::Random(0, 90);
	double fouthValue = vtkMath::Random(0, 90);

	int pointNum = extrusion->GetOutput()->GetNumberOfPoints();
	std::cout << pointNum << std::endl;
	for (int i = 0; i < pointNum / 4; i++)
	{
		stress->InsertNextValue(value);
	}
	for (int i = 0; i < pointNum / 4; i++)
	{
		stress->InsertNextValue(nextValue);
	}
	for (int i = 0; i < pointNum / 4; i++)
	{
		stress->InsertNextValue(thirdValue);
	}
	for (int i = 0; i < pointNum / 4; i++)
	{
		stress->InsertNextValue(fouthValue);
	}
	extrusion->GetOutput()->GetPointData()->SetScalars(stress);

	appendFilter->AddInputConnection(extrusion->GetOutputPort());
}

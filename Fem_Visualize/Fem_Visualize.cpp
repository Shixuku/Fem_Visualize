#include "Fem_Visualize.h"
#include <iostream>
#include <Eigen/Dense>
#include "StructureFem.h"
#include <QVTKOpenGLNativeWidget.h>
#include <QPushButton>
#include <QAction>
#include <QDebug>
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include "VisualizeWindow.h"
#include "Element_Beam3D.h"
#include "Section_Beam3D.h"

Fem_Visualize::Fem_Visualize(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	this->setFixedSize(1200, 800);

	renderer->SetGradientBackground(true);
	renderer->SetBackground2(40.0 / 255.0, 110.0 / 255.0, 170.0 / 255.0);
	renderer->SetBackground(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);

	EntityBase::Set_Structure(m_structure);
	m_structure->Input_data("../data6.txt");
	m_structure->Analyse();
	m_structure = m_structure;
	SetRenderWindow();

	connect(ui.pushButton_2, &QPushButton::clicked, this, &Fem_Visualize::onIsShowSection);
	connect(ui.pushButton, &QPushButton::clicked, this, &Fem_Visualize::onShowDisplacement);
	connect(ui.pushButton_3, &QPushButton::clicked, this, &Fem_Visualize::onShowAxialForces);
	connect(reactionForce, &ReactionForceWindow::signalSendForceType, 
		this, &Fem_Visualize::onSendForceType);
	
	//VisualizeWindow* window = new VisualizeWindow(ss);
	//ShowDisplacement();
	//ShowAxialForces();
}

Fem_Visualize::~Fem_Visualize()
{}

void Fem_Visualize::SetRenderWindow()
{
	InitNode(m_structure->m_Nodes);
	InitElement();
}

void Fem_Visualize::InitNode(std::map<int, NodeFem*> nodes)
{
	double point[3];

	std::cout << "window" << std::endl;
	for (auto node : nodes)
	{
		NodeFem* fem = node.second;
		point[0] = fem->m_x;
		point[1] = fem->m_y;
		point[2] = fem->m_z;
		points->InsertPoint(fem->m_id - 1, point);
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
	ui.widget->GetRenderWindow()->AddRenderer(renderer);
	ui.widget->GetRenderWindow()->Render();
}

void Fem_Visualize::InitElement()
{
	vtkNew <vtkCellArray> cellArray;
	for (auto element : m_structure->m_Element)
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

	for (auto element : m_structure->m_Element)
	{
		startNodeIndex = element.second->m_idNode.at(0);
		endNodeIndex = element.second->m_idNode.at(1);

		NodeFem* startNode = m_structure->Find_Node(startNodeIndex);
		NodeFem* endNode = m_structure->Find_Node(endNodeIndex);

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
	//polyData->GetPointData()->SetScalars(stress);

	vtkNew<vtkPolyDataMapper> lineMapper;
	lineMapper->SetInputData(polyData);

	vtkNew<vtkPolyDataMapper> mapper;
	//mapper->SetInputData(polyData);
	mapper->SetInputConnection(appendFilter->GetOutputPort());
	mapper->SetScalarRange(0, 90);
	mapper->SetScalarModeToUsePointData();

	scalarBar->SetLookupTable(mapper->GetLookupTable());
	scalarBar->SetOrientationToHorizontal();
	scalarBar->SetPosition(0.05, 0.1);
	scalarBar->SetPosition2(0.9, 0.11);
	//scalarBar->SetWidth(0.5);
	scalarBar->SetNumberOfLabels(10);
	scalarBar->SetTextPositionToPrecedeScalarBar();

	// Get the property for the scalar bar labels
	vtkTextProperty* labelTextProperty = scalarBar->GetLabelTextProperty();
	labelTextProperty->SetFontSize(5); // Set the font size for the scalar bar labels

	lineActor->SetMapper(lineMapper);
	lineActor->GetProperty()->SetLineWidth(3);
	lineActor->GetProperty()->SetColor(0, 0, 1);

	sectionActor->SetMapper(mapper);
	//actor->GetProperty()->EdgeVisibilityOn();

	sectionActor->GetProperty()->SetLineWidth(2);

	//renderer->RemoveAllViewProps();

	renderer->AddActor(sectionActor);
	//renderer->AddActor(lineActor);
	renderer->AddActor(scalarBar);
	ui.widget->GetRenderWindow()->Render();
}

void Fem_Visualize::ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix)
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
	// 两个向量的点积等于一个向量在另一个向量方向上的投影长度与另一个向量本身长度的乘积，
	// 反映了一个向量在另一个向量方向上的分量大小。
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

void Fem_Visualize::CreateRecSection(double length, double width, double startPoint[3], double endPoint[3])
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

void Fem_Visualize::onIsShowSection()
{
	if (showFlag == 0)
	{
		renderer->RemoveActor(sectionActor);
		renderer->AddActor(lineActor);
		ui.widget->GetRenderWindow()->Render();
		showFlag = 1;
		return;
	}
	if (showFlag == 1);
	{
		renderer->RemoveActor(lineActor);
		renderer->AddActor(sectionActor);
		renderer->ResetCamera();
		ui.widget->GetRenderWindow()->Render();
		showFlag = 0;
		return;
	}
}

void Fem_Visualize::onShowDisplacement()
{
	reactionForce->show();

	vtkNew<vtkPoints> newPoints;
	newPoints->DeepCopy(points);
	vtkNew<vtkDoubleArray> dis;
	double displacement = 0;
	double pointNum = 0;
	double point[3];
	double maxDis = 0;
	double minDis = 0;
	for (auto a : m_structure->m_Nodes)
	{
		NodeFem* pNode = a.second;
		int dofNum = pNode->m_DOF[1];
		if (dofNum < m_structure->m_nFixed)
		{
			displacement = m_structure->m_x1[dofNum];
		}
		else
		{
			displacement = m_structure->m_x2[dofNum - m_structure->m_nFixed];
		}
		newPoints->GetPoint(pointNum, point);
		point[1] += displacement * 800;
		newPoints->SetPoint(pointNum, point);
		dis->InsertNextValue(displacement);
		pointNum++;
		if (displacement > maxDis) maxDis = displacement;
		if (displacement < minDis) minDis = displacement;
	}

	vtkNew<vtkLookupTable> lut;
	//lut->SetValueRange(minDis, maxDis);
	lut->SetHueRange(0.667, 0.0); // 设置颜色范围从蓝色到红色（对应色相范围从240到0）

	renderer->RemoveAllViewProps();

		// 输出原始的点坐标
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
	{
		double* point = points->GetPoint(i);
	}

	// 输出修改后的点坐标
	for (vtkIdType i = 0; i < newPoints->GetNumberOfPoints(); i++)
	{
		double* point = newPoints->GetPoint(i);
	}

	vtkNew <vtkCellArray> cellArray;
	for (auto element : m_structure->m_Element)
	{
		vtkNew<vtkLine> line;
		line->GetPointIds()->SetId(0, element.second->m_idNode.at(0) - 1);
		line->GetPointIds()->SetId(1, element.second->m_idNode.at(1) - 1);
		cellArray->InsertNextCell(line);
	}

	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(newPoints);
	polyData->SetLines(cellArray);
	polyData->GetPointData()->SetScalars(dis);

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputData(polyData);
	mapper->SetScalarRange(minDis, maxDis);
	mapper->SetLookupTable(lut);

	scalarBar->SetLookupTable(lut);

	vtkNew<vtkActor> newActor;
	newActor->SetMapper(mapper);
	newActor->GetProperty()->SetLineWidth(2);

	renderer->AddActor(newActor);
	renderer->AddActor(scalarBar);
	ui.widget->GetRenderWindow()->Render();
}

void Fem_Visualize::onShowAxialForces()
{
	renderer->RemoveAllViewProps();
	vtkNew<vtkDoubleArray> forcesArray;

	int startDofIndex;
	int endDoftIndex;
	double maxForce = 0;
	double minForce = 0;
	double displacement;

	vtkNew <vtkCellArray> cellArray;
	for (auto element : m_structure->m_Element)
	{
		vtkNew<vtkLine> line;
		NodeFem* startNode = m_structure->Find_Node(element.second->m_idNode.at(0));
		NodeFem* endNode = m_structure->Find_Node(element.second->m_idNode.at(1));

		startDofIndex = startNode->m_DOF[0];
		endDoftIndex = endNode->m_DOF[0];

		double deltaL = GetDofValue(startDofIndex) - GetDofValue(endDoftIndex);

		double L = GetDistance(startNode->m_x, startNode->m_y, startNode->m_z, endNode->m_x, endNode->m_y, endNode->m_z);

		Section_Base* pSection = m_structure->Find_Section(element.second->m_idSection);
		Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);

		// F = △L / L * E * A;
		double axialForce = deltaL / L * pSectionBeam->Get_E() * pSectionBeam->m_Area;


		forcesArray->InsertNextValue(axialForce);

		if (axialForce > maxForce) maxForce = axialForce;
		if (axialForce < minForce) minForce = axialForce;

		line->GetPointIds()->SetId(0, element.second->m_idNode.at(0) - 1);
		line->GetPointIds()->SetId(1, element.second->m_idNode.at(1) - 1);
		cellArray->InsertNextCell(line);
	}

	vtkNew<vtkLookupTable> lut;
	//lut->SetValueRange(minDis, maxDis);
	lut->SetHueRange(0.667, 0.0); // 设置颜色范围从蓝色到红色（对应色相范围从240到0）

	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points);
	polyData->SetLines(cellArray);
	polyData->GetPointData()->SetScalars(forcesArray);

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputData(polyData);
	mapper->SetScalarRange(minForce, maxForce);
	mapper->SetLookupTable(lut);

	scalarBar->SetLookupTable(lut);

	vtkNew<vtkActor> newActor;
	newActor->SetMapper(mapper);
	newActor->GetProperty()->SetLineWidth(2);

	renderer->AddActor(newActor);
	renderer->AddActor(scalarBar);
	ui.widget->GetRenderWindow()->Render();
}

double Fem_Visualize::GetDofValue(int dofIndex)
{
	double displacement;

	if (dofIndex < m_structure->m_nFixed)
	{
		displacement = m_structure->m_x1[dofIndex];
		//std::cout << "fix displacement:" << displacement << "\n";
	}
	else
	{
		displacement = m_structure->m_x2[dofIndex - m_structure->m_nFixed];
		//std::cout << "displacement:" << displacement << "\n";
	}

	return displacement;
}

double Fem_Visualize::GetDistance(double x1, double y1, double z1, double x2, double y2, double z2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	double dz = z2 - z1;

	return std::sqrt(dx * dx + dy * dy + dz * dz);
}


void Fem_Visualize::onSendForceType()
{
	switch (reactionForce->forceType)
	{
	case FX:
		qDebug() << "X";
		break;
	case FY:
		// Call function for FY
		break;
	case FZ:
		// Call function for FZ
		break;
	case FXYZ:
		// Call function for FXYZ
		break;
	case MX:
		// Call function for MX
		break;
	case MY:
		// Call function for MY
		break;
	case MZ:
		// Call function for MZ
		break;
	case MXYZ:
		// Call function for MXYZ
		break;
	default:
		break;
	}
}

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
#include <vtkPointData.h>
#include <vtkLabelHierarchy.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkFreeTypeLabelRenderStrategy.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include "Boundary.h"
#include "VisualizeWindow.h"
#include "Element_Beam3D.h"
#include "Section_Beam3D.h"

using namespace std;

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
	m_structure->Input_data("../data5.txt");
	m_structure->Analyse();
	m_structure = m_structure;
	SetRenderWindow();

	connect(ui.pushButtonShow, &QPushButton::clicked, this, &Fem_Visualize::onIsShowSection);
	connect(ui.pushButtonDis, &QPushButton::clicked, this, &Fem_Visualize::onShowDisplacement);
	connect(ui.pushButtonForce, &QPushButton::clicked, this, &Fem_Visualize::onShowAxialForces);
	connect(ui.pushButtonRea, &QPushButton::clicked, this, &Fem_Visualize::onShowReactionForces);

	connect(reactionForce, &ReactionForceWindow::signalSendForceType, 
		this, &Fem_Visualize::onSendForceType);

	connect(displacement, &DisplacementWindow::signalSendForceType,
		this, &Fem_Visualize::onSendDispType);
	
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

	ui.widget->GetRenderWindow()->AddRenderer(renderer);
	ui.widget->GetRenderWindow()->Render();
}

void Fem_Visualize::InitNode(std::map<int, NodeFem*> nodes)
{
	vtkNew<vtkPoints> pts;
	vtkNew<vtkStringArray> Label;
	double point[3];

	for (auto node : nodes)
	{
		NodeFem* fem = node.second;
		point[0] = fem->m_x;
		point[1] = fem->m_y;
		point[2] = fem->m_z;
		points->InsertPoint(fem->m_id - 1, point);
		point[1] += 0.1; 
		pts->InsertNextPoint(point);
		Label->InsertNextValue(std::to_string(fem->m_id));
	}
	Label->SetName("Labels");

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

	//GeneraterPointLable(pts, Label, pointLabelActor);

	renderer->AddActor(actor);
	//renderer->AddActor(pointLabelActor);
}

void Fem_Visualize::InitElement()
{
	vtkNew <vtkCellArray> cellArray;
	vtkNew<vtkPoints> pts;
	vtkNew<vtkStringArray> label;
	for (auto element : m_structure->m_Element)
	{
		vtkNew<vtkLine> line;
		line->GetPointIds()->SetId(0, element.second->m_idNode.at(0) - 1);
		line->GetPointIds()->SetId(1, element.second->m_idNode.at(1) - 1);
		cellArray->InsertNextCell(line);
	}
	double startPoint[3];
	double endPoint[3];
	double middlePoint[3];
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

		middlePoint[0] = (startPoint[0] + endPoint[0]) / 2;
		middlePoint[1] = (startPoint[1] + endPoint[1]) / 2 + 0.1;
		middlePoint[2] = (startPoint[2] + endPoint[2]) / 2;

		pts->InsertNextPoint(middlePoint);
		label->InsertNextValue(std::to_string(element.second->m_id));

		CreateRecSection(0.2, 0.1, startPoint, endPoint);
	}

	label->SetName("Labels");
	GeneraterLable(pts, label, elementLableActor);
	cout << "array sizes:" << label->GetSize() << std::endl;

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

	lineActor->SetMapper(lineMapper);
	lineActor->GetProperty()->SetLineWidth(3);
	lineActor->GetProperty()->SetColor(0, 0, 1);

	sectionActor->SetMapper(mapper);

	sectionActor->GetProperty()->SetLineWidth(2);

	renderer->AddActor(sectionActor);
	renderer->AddActor(scalarBar);
	renderer->AddActor(elementLableActor);
}

void Fem_Visualize::ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix)
{
	// ���������������������
	double normal[3] = { endPoint[0] - startPoint[0], endPoint[1] - startPoint[1], endPoint[2] - startPoint[2] };

	// ������������
	double sourceVector[3] = { 0.0, 0.0, 1.0 };
	double targetVector[3] = { normal[0], normal[1], normal[2] };

	// ��׼����������
	vtkMath::Normalize(sourceVector);
	vtkMath::Normalize(targetVector);

	// ������ת�����ת�Ƕ�
	// ���������ĵ������һ����������һ�����������ϵ�ͶӰ��������һ�������������ȵĳ˻���
	// ��ӳ��һ����������һ�����������ϵķ�����С��
	// �������������ĳ��ȼ��ɵõ����������ļн�
	double rotationAxis[3];
	vtkMath::Cross(sourceVector, targetVector, rotationAxis);
	double rotationAngle = acos(vtkMath::Dot(sourceVector, targetVector));

	// ������ת����
	vtkSmartPointer<vtkTransform> rotation = vtkSmartPointer<vtkTransform>::New();
	rotation->RotateWXYZ(rotationAngle * vtkMath::DegreesFromRadians(1.0), rotationAxis);

	// ����ƽ�ƾ���
	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	// translation->Translate(startPoint);

	// ������ת��ƽ�Ƶ���Ͼ���
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Concatenate(translation);
	transform->Concatenate(rotation);

	// ��ȡ�任����
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
	// ���õ�һ����������췽��
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
	displacement->show();

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
	lut->SetHueRange(0.667, 0.0); // ������ɫ��Χ����ɫ����ɫ����Ӧɫ�෶Χ��240��0��

	renderer->RemoveAllViewProps();

		// ���ԭʼ�ĵ�����
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
	{
		double* point = points->GetPoint(i);
	}

	// ����޸ĺ�ĵ�����
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

		double deltaX = GetDofValue(startNode->m_DOF[0]) - GetDofValue(endNode->m_DOF[0]);
		double deltaY = GetDofValue(startNode->m_DOF[1]) - GetDofValue(endNode->m_DOF[1]);
		double deltaZ = GetDofValue(startNode->m_DOF[2]) - GetDofValue(endNode->m_DOF[2]);

		double deltaL = std::sqrt(deltaX * deltaX + deltaY * deltaY * deltaZ * deltaZ);

		std::cout << "element:" << element.second->m_id << std::endl;


		double L = GetDistance(startNode->m_x, startNode->m_y, startNode->m_z, 
						       endNode->m_x, endNode->m_y, endNode->m_z);

		Section_Base* pSection = m_structure->Find_Section(element.second->m_idSection);
		Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);

		// F = ��L / L * E * A;
		double axialForce = deltaL / L * pSectionBeam->Get_E() * pSectionBeam->m_Area;

		std::cout << "axialForce:" << axialForce << std::endl;
		forcesArray->InsertNextValue(axialForce);

		if (axialForce > maxForce) maxForce = axialForce;
		if (axialForce < minForce) minForce = axialForce;

		line->GetPointIds()->SetId(0, element.second->m_idNode.at(0) - 1);
		line->GetPointIds()->SetId(1, element.second->m_idNode.at(1) - 1);
		cellArray->InsertNextCell(line);
	}

	vtkNew<vtkLookupTable> lut;
	//lut->SetValueRange(minDis, maxDis);
	lut->SetHueRange(0.667, 0.0); // ������ɫ��Χ����ɫ����ɫ����Ӧɫ�෶Χ��240��0��

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

void Fem_Visualize::onShowReactionForces()
{
	reactionForce->show();
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


void Fem_Visualize::GeneraterLable(vtkPoints* pts, vtkStringArray* label, vtkSmartPointer<vtkActor2D> actor)
{
	vtkNew<vtkLabelHierarchy> polyData;
	polyData->SetPoints(pts);
	polyData->GetPointData()->AddArray(label);

	vtkNew<vtkTextProperty> TextProperty;
	TextProperty->SetFontSize(15);
	TextProperty->SetColor(0, 0, 0);
	TextProperty->SetBold(true);
	TextProperty->SetFontFamilyToArial();

	vtkNew<vtkPointSetToLabelHierarchy> Hie;
	Hie->SetInputData(polyData);
	Hie->SetMaximumDepth(20);
	Hie->SetLabelArrayName("Labels");
	Hie->SetTextProperty(TextProperty);

	vtkNew<vtkLabelPlacementMapper> MapperLabel;
	MapperLabel->SetInputConnection(Hie->GetOutputPort());
	vtkNew<vtkFreeTypeLabelRenderStrategy> strategy_tmp;
	MapperLabel->SetRenderStrategy(strategy_tmp);
	MapperLabel->SetShapeToNone();
	MapperLabel->UseDepthBufferOn();
	MapperLabel->SetStyleToOutline();

	actor->SetMapper(MapperLabel);
}

void Fem_Visualize::onSendForceType()
{
	switch (reactionForce->forceType)
	{
	case FX:
		for (auto &a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "FX: " << a->m_ReactionForce[0] << std::endl;
		}
		break;
	case FY:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "FY: " << a->m_ReactionForce[1] << std::endl;
		}
		break;
	case FZ:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "FZ: " << a->m_ReactionForce[2] << std::endl;
		}
		break;
	case FXYZ:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "FX: " << a->m_ReactionForce[0] << std::endl;
			std::cout << "FY: " << a->m_ReactionForce[1] << std::endl;
			std::cout << "FZ: " << a->m_ReactionForce[2] << std::endl;
		}
		break;
	case MX:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "MX: " << a->m_ReactionForce[3] << std::endl;
		}
		break;
	case MY:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "MY: " << a->m_ReactionForce[4] << std::endl;
		}
		break;
	case MZ:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "MZ: " << a->m_ReactionForce[5] << std::endl;
		}
		break;
	case MXYZ:
		for (auto& a : m_structure->m_ReationForce)
		{
			std::cout << "Node: " << a->m_id << std::endl;
			std::cout << "MX: " << a->m_ReactionForce[3] << std::endl;
			std::cout << "MY: " << a->m_ReactionForce[4] << std::endl;
			std::cout << "MZ: " << a->m_ReactionForce[5] << std::endl;
		}
		break;
	default:
		break;
	}
	std::cout << "\n\n";
}

void Fem_Visualize::onSendDispType()
{
	switch (displacement->dispType)
	{
	case DX:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DX: " << GetDofValue(a.second->m_DOF[0]) << std::endl;
		}
		break;
	case DY:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DY: " << GetDofValue(a.second->m_DOF[1]) << std::endl;
		}
		break;
	case DZ:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DZ: " << GetDofValue(a.second->m_DOF[2]) << std::endl;
		}
		break;
	case DXY:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DX: " << GetDofValue(a.second->m_DOF[0]) << std::endl;
			std::cout << "DY: " << GetDofValue(a.second->m_DOF[1]) << std::endl;
		}
		break;
	case DYZ:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DY: " << GetDofValue(a.second->m_DOF[1]) << std::endl;
			std::cout << "DZ: " << GetDofValue(a.second->m_DOF[2]) << std::endl;
		}
		break;
	case DXZ:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DX: " << GetDofValue(a.second->m_DOF[0]) << std::endl;
			std::cout << "DZ: " << GetDofValue(a.second->m_DOF[2]) << std::endl;
		}
		break;
	case DXYZ:
		for (auto& a : m_structure->m_Nodes)
		{
			std::cout << "Node: " << a.second->m_id << std::endl;
			std::cout << "DX: " << GetDofValue(a.second->m_DOF[0]) << std::endl;
			std::cout << "DY: " << GetDofValue(a.second->m_DOF[1]) << std::endl;
			std::cout << "DZ: " << GetDofValue(a.second->m_DOF[2]) << std::endl;
		}
		break;
	default:
		break;
	}
	std::cout << "\n\n";
}
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
#include <vtkHexahedron.h>
#include <vtkTetra.h>
#include <vtkAppendFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkMergePoints.h>
#include <vtkCellArray.h>
#include "Boundary.h"
#include "Element_Beam3D.h"
#include "Section_Beam3D.h"
#include "Element_Base.h"
#include "NodeFem.h"
#include <vtkVertexGlyphFilter.h>
#include <QStandardItemModel>

#include <map>
using namespace std;

Fem_Visualize::Fem_Visualize(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	showMaximized();

	renderer->SetGradientBackground(true);
	renderer->SetBackground2(40.0 / 255.0, 110.0 / 255.0, 170.0 / 255.0);
	renderer->SetBackground(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);

	EntityBase::Set_Structure(m_structure);
	m_structure->Input_datas("../data9.txt");
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
	

	//SetRenderWidget();
	SetTreeWidget();

	connect(deckWindow, &DeckWindow::SignalShowDeckModel, this, &Fem_Visualize::onShowDeckModel);
	connect(towerWindow, &TowerWindow::SignalShowTowerModel, this, &Fem_Visualize::onShowTowerModel);
	connect(ropeWindow, &RopeWindow::SignalShowRopeModel, this, &Fem_Visualize::onShowRopeModel);
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

void Fem_Visualize::InitNodes()
{
	// Get a point point in the set
	double inSet[3];
	vtkNew<vtkPolyData> points;
	points->SetPoints(bridge->appendPoints);

	std::cout << "There are " << points->GetNumberOfPoints() << " input points."
		<< std::endl;

	vtkIdType id;

	// Insert all of the points
	vtkNew<vtkMergePoints> mergePoints;
	mergePoints->SetDataSet(points);
	mergePoints->SetDivisions(10, 10, 10);
	mergePoints->InitPointInsertion(points->GetPoints(), points->GetBounds());

	//清空Node map 容器
	m_structure->m_Nodes.clear();
	int inserted;
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
	{
		points->GetPoint(i, inSet);
		inserted = mergePoints->InsertUniquePoint(inSet, id);
		std::cout << "\tPoint: " << inSet[0] << ", " << inSet[1] << ", " << inSet[2]
			<< " ";
		if (inserted != 0)
		{
			int idNode = (int)id;
			m_structure->m_Nodes.insert(
				make_pair(idNode, new NodeFem(idNode, inSet[0], inSet[1], inSet[2])));
		}
		std::cout << "Inserted? " << ((inserted == 0) ? "No, " : "Yes, ");
		std::cout << "Id:: " << id << std::endl;
	}

	for (int i = 0; i < m_structure->m_Nodes.size(); i++)
	{
		NodeFem* node = m_structure->m_Nodes[i];
		std::cout << "id:" << node->m_id << " x:" << node->m_x << " y:" << node->m_y << " z:" << node->m_z << std::endl;
	} 
}


void Fem_Visualize::InitElement()
{
	vtkNew <vtkCellArray> cellArray;
	vtkNew<vtkPoints> pts;
	vtkNew<vtkStringArray> label;
	for (auto element : m_structure->m_Elements)
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

	for (auto element : m_structure->m_Elements)
	{
		if (element.second->m_type == "Beam")
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

		else if (element.second->m_type == "Brick")
		{
			vtkNew<vtkPoints> points;
			vtkNew<vtkHexahedron> hexahedron;
			for (int i = 0; i < element.second->m_idNode.size(); i++)
			{
				NodeFem* node = m_structure->Find_Node(element.second->m_idNode[i]);
				points->InsertNextPoint(node->m_x, node->m_y, node->m_z); // 顶点
				std::cout << "m_x:" << node->m_x << " m_y:" << node->m_y << " m_z:" << node->m_z << std::endl;
				hexahedron->GetPointIds()->SetId(i, i);
			}

			// Add the hexahedron to a cell array.
			vtkNew<vtkCellArray> hexs;
			hexs->InsertNextCell(hexahedron);

			// Add the points and hexahedron to an unstructured grid.
			vtkNew<vtkUnstructuredGrid> uGrid;
			uGrid->SetPoints(points);
			uGrid->InsertNextCell(hexahedron->GetCellType(), hexahedron->GetPointIds());
			soildAppendFilter->AddInputData(uGrid);
			soildAppendFilter->Update();
		}

		else if (element.second->m_type == "Tetra")
		{
			vtkNew<vtkPoints> points;
			vtkNew<vtkTetra> tetra;
			for (int i = 0; i < element.second->m_idNode.size(); i++)
			{
				NodeFem* node = m_structure->Find_Node(element.second->m_idNode[i]);
				points->InsertNextPoint(node->m_x, node->m_y, node->m_z); // 顶点0
				std::cout << "m_x:" << node->m_x << " m_y:" << node->m_y << " m_z:" << node->m_z << std::endl;
				tetra->GetPointIds()->SetId(i, i);
			}

			// Add the hexahedron to a cell array.
			vtkNew<vtkCellArray> cells;
			cells->InsertNextCell(tetra);

			// Add the points and hexahedron to an unstructured grid.
			vtkNew<vtkUnstructuredGrid> uGrid;
			uGrid->SetPoints(points);
			uGrid->InsertNextCell(tetra->GetCellType(), tetra->GetPointIds());
			soildAppendFilter->AddInputData(uGrid);
			soildAppendFilter->Update();
		}

		int pointsNum = element.second->m_idNode.size();
		double centerPoints[3] = { 0,0,0 };
		for (int i = 0; i < pointsNum; i++)
		{
			NodeFem *node = m_structure->Find_Node(element.second->m_idNode[i]);
			centerPoints[0] += node->m_x / pointsNum;
			centerPoints[1] += (node->m_y + 0.1) / pointsNum;
			centerPoints[2] += node->m_z / pointsNum;
		}
		pts->InsertNextPoint(centerPoints);
		label->InsertNextValue(std::to_string(element.second->m_id));
	}
	label->SetName("Labels");
	GeneraterLable(pts, label, elementLableActor);

	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points);
	polyData->SetLines(cellArray);

	vtkNew<vtkDataSetMapper> soildMapper;
	if (soildAppendFilter->GetOutput()->GetNumberOfPoints() != 0)
	{
		soildMapper->SetInputConnection(soildAppendFilter->GetOutputPort());

		soildActor->SetMapper(soildMapper);
		soildActor->GetProperty()->SetEdgeColor(0, 0, 0);
		soildActor->GetProperty()->SetColor(0.8, 1, 1); // 淡蓝色
		soildActor->GetProperty()->EdgeVisibilityOn();
		soildActor->GetProperty()->SetLineWidth(1);
		renderer->AddActor(soildActor);
	}

	vtkNew<vtkPolyDataMapper> linkMapper;
	if (linkAppendFilter->GetOutput()->GetNumberOfPoints() != 0)
	{
		//mapper->SetInputData(polyData);
		linkMapper->SetInputConnection(linkAppendFilter->GetOutputPort());
		linkMapper->SetScalarRange(0, 90);
		linkMapper->SetScalarModeToUsePointData();
		renderer->AddActor(linkActor);
	}

	vtkNew<vtkPolyDataMapper> lineMapper;
	lineMapper->SetInputData(polyData);

	scalarBar->SetLookupTable(linkMapper->GetLookupTable());
	scalarBar->SetOrientationToHorizontal();
	scalarBar->SetPosition(0.05, 0.1);
	scalarBar->SetPosition2(0.9, 0.11);
	//scalarBar->SetWidth(0.5);
	scalarBar->SetNumberOfLabels(10);
	scalarBar->SetTextPositionToPrecedeScalarBar();

	lineActor->SetMapper(lineMapper);
	lineActor->GetProperty()->SetLineWidth(3);
	lineActor->GetProperty()->SetColor(0, 0, 1);
	linkActor->SetMapper(linkMapper);

	renderer->AddActor(scalarBar);
	renderer->AddActor(elementLableActor);
}

void Fem_Visualize::InitElements()
{
	double inSet[3];
	vtkNew<vtkPolyData> points;
	points->SetPoints(bridge->appendPoints);

	std::cout << "There are " << points->GetNumberOfPoints() << " input points."
		<< std::endl;

	vtkIdType id;

	// Insert all of the points
	vtkNew<vtkMergePoints> mergePoints;
	mergePoints->SetDataSet(points);
	mergePoints->SetDivisions(10, 10, 10);
	mergePoints->InitPointInsertion(points->GetPoints(), points->GetBounds());

	m_structure->m_Elements.clear();
	int inserted;
	int elementId = 1;
	for (int i = 0 ; i < bridge->leftDeckPoints->GetNumberOfPoints() - 1; i++)
	{
		Element_Beam3D* element = new Element_Beam3D();
		points->GetPoint(i, inSet);
		inserted = mergePoints->InsertUniquePoint(inSet, id);
		element->m_idNode[0] = id;
		points->GetPoint(i + 1, inSet);
		inserted = mergePoints->InsertUniquePoint(inSet, id);
		element->m_idNode[1] = id;
		element->m_id = elementId;
		elementId++;
		m_structure->m_Elements.insert(make_pair(elementId, element));
	}

	for (auto a : m_structure->m_Elements)
	{
		Element_Base* element = a.second;
		std::cout << element->m_id << std::endl;
	}

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

	linkAppendFilter->AddInputConnection(extrusion->GetOutputPort());
	linkAppendFilter->Update();
}

void Fem_Visualize::onIsShowSection()
{
	if (showFlag == 0)
	{
		renderer->RemoveActor(linkActor);
		renderer->AddActor(lineActor);
		ui.widget->GetRenderWindow()->Render();
		showFlag = 1;
		return;
	}
	if (showFlag == 1);
	{
		renderer->RemoveActor(lineActor);
		renderer->AddActor(linkActor);
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
	for (auto element : m_structure->m_Elements)
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
	for (auto element : m_structure->m_Elements)
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

		// F = △L / L * E * A;
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

void Fem_Visualize::onShowDeckModel()
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(bridge->deckPointDisplay->GetActor());
	renderer->AddActor(bridge->deckLineDisplay->GetActor());

	//cout << "numbers of cells" << bridge->deckLineDisplay->GetActor()->GetMapper()->GetOutputPort()->.
	ui.widget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
	ui.widget->GetRenderWindow()->Render();
}

void Fem_Visualize::onShowTowerModel()
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(bridge->deckLineDisplay->GetActor());
	renderer->AddActor(bridge->towerLineDisplay->GetActor());

	vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);
	renderWindow->Render();

	ui.widget->update();
}


void Fem_Visualize::onShowRopeModel()
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(bridge->deckLineDisplay->GetActor());
	renderer->AddActor(ropeWindow->ropeActor);
	renderer->AddActor(bridge->towerLineDisplay->GetActor());

	vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);
	renderWindow->Render();
	InitNodes();
	InitElements();
	ui.widget->update();
}

void Fem_Visualize::onHiddenOrShowModel()
{
	static bool isPressed = false;

	if (isPressed)
	{
		onShowRopeModel();
	}
	else
	{
		renderer->RemoveAllViewProps();
		renderer->AddActor(deckWindow->deckActor);
		renderer->AddActor(towerWindow->towerActor);
		renderer->AddActor(ropeWindow->ropeActor);

		renderer->SetGradientBackground(true);
		renderer->SetBackground2(40.0 / 255.0, 110.0 / 255.0, 170.0 / 255.0);
		renderer->SetBackground(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);

		vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
		renderWindow->AddRenderer(renderer);
		renderWindow->Render();

		ui.widget->update();
	}

	isPressed = !isPressed;
}

void Fem_Visualize::ShowDeckWindow()
{
	deckWindow->exec();
}

void Fem_Visualize::ShowTowerWindow()
{
	towerWindow->exec();
}

void Fem_Visualize::ShowRopeWindow()
{
	ropeWindow->exec();
}

void Fem_Visualize::SetTreeWidget()
{
	ui.treeView->header()->hide();
	ui.treeView->setUniformRowHeights(true);
	ui.treeView->setStyleSheet("QTreeView::item { height: 50px; }");

	// 添加树状菜单的数据模型
	QStandardItemModel* model = new QStandardItemModel(this);
	QStandardItem* rootItem = model->invisibleRootItem();
	QStandardItem* item1 = new QStandardItem(tr("Model Build"));
	QStandardItem* sectionItem = new QStandardItem(tr("Model Generate"));
	QStandardItem* deckItem = new QStandardItem(tr("Deck Build"));
	deckItem->setData("Deck", Qt::UserRole);
	QStandardItem* towerItem = new QStandardItem(tr("Tower Build"));
	towerItem->setData("Tower", Qt::UserRole);
	QStandardItem* ropeItem = new QStandardItem(tr("Rope Build"));
	ropeItem->setData("Rope", Qt::UserRole);
	item1->appendRow(deckItem);
	item1->appendRow(towerItem);
	item1->appendRow(ropeItem);
	rootItem->appendRow(item1);
	rootItem->appendRow(sectionItem);

	ui.treeView->setModel(model);

	// 禁止编辑树状菜单
	ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 连接树状菜单的信号和槽，实现点击展开
	connect(ui.treeView, &QTreeView::clicked, ui.treeView, &QTreeView::expand);
	connect(ui.treeView, &QTreeView::clicked, this, &Fem_Visualize::onHandleItemClick);
}

void Fem_Visualize::SetRenderWidget()
{
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetGradientBackground(true);
	renderer->SetBackground2(40.0 / 255.0, 110.0 / 255.0, 170.0 / 255.0);
	renderer->SetBackground(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow = ui.widget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);
}

void Fem_Visualize::SetMenuWidget()
{

}

void Fem_Visualize::onHandleItemClick(const QModelIndex& index)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
	QStandardItem* item = model->itemFromIndex(index);
	if (item)
	{
		QVariant data = item->data(Qt::UserRole); // 获取item的标识符
		QString function = data.toString();
		if (function == "Deck") {
			// 执行函数1
			ShowDeckWindow();
		}
		else if (function == "Tower") {
			// 执行函数2
			ShowTowerWindow();
		}
		else if (function == "Rope") {
			// 执行函数3
			ShowRopeWindow();
		}
	}
}
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
#include <vtkAreaPicker.h>
#include "AreaPickerCellInteractor.h"
#include "AreaPickerNodeInteractor.h"
#include "PointPickerCellInteractor.h"
#include "PointPickerNodeInteractor.h"
#include "Boundary.h"
#include "LinkElement_Beam3D.h"
#include "Section_Beam3D.h"
#include "LinkElement_Truss3D.h"
#include "Element_Base.h"
#include "NodeFem.h"
#include "Material.h"
#include <vtkVertexGlyphFilter.h>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <map>
using namespace std;

Fem_Visualize::Fem_Visualize(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	showMaximized();
	Init_Action();

	renderer->SetGradientBackground(true);
	renderer->SetBackground2(40.0 / 255.0, 110.0 / 255.0, 170.0 / 255.0);
	renderer->SetBackground(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);

	vtkSmartPointer<vtkRenderWindow> GetRenderWindow = ui.widget->renderWindow();;
	GetRenderWindow->AddRenderer(renderer);

	//EntityBase::Set_Structure(m_structure);
	//m_structure->Input_datas("../data9.txt");
	//m_structure->Analyse();

	ui.widget->renderWindow()->AddRenderer(renderer);

	connect(ui.pushButtonShow, &QPushButton::clicked, this, &Fem_Visualize::onIsShowSection);
	connect(ui.pushButtonDis, &QPushButton::clicked, this, &Fem_Visualize::onShowDisplacement);
	connect(ui.pushButtonForce, &QPushButton::clicked, this, &Fem_Visualize::onShowAxialForces);
	connect(ui.pushButtonRea, &QPushButton::clicked, this, &Fem_Visualize::onShowReactionForces);
	connect(ui.pushButtonSelFile, &QPushButton::clicked, this, &Fem_Visualize::onSelectFile);
	connect(ui.pushButtonLoad, &QPushButton::clicked, this, &Fem_Visualize::onLoadWindow);
	connect(ui.pushButton, &QPushButton::clicked, this, &Fem_Visualize::onAnalyse);
	connect(ui.pushButtonInVar, &QPushButton::clicked, this, &Fem_Visualize::onShowInVarWindow);

	connect(reactionForce, &ReactionForceWindow::signalSendForceType, 
		this, &Fem_Visualize::onSendForceType);

	connect(displacement, &DisplacementWindow::signalSendForceType,
		this, &Fem_Visualize::onSendDispType);
	

	//SetRenderWidget();
	SetTreeWidget();

	connect(deckWindow, &DeckWindow::SignalShowDeckModel, this, &Fem_Visualize::onShowDeckModel);
	connect(towerWindow, &TowerWindow::SignalShowTowerModel, this, &Fem_Visualize::onShowTowerModel);
	connect(ropeWindow, &RopeWindow::SignalShowRopeModel, this, &Fem_Visualize::onShowRopeModel);

	connect(ui.pushButtonCell, &QPushButton::clicked, this, &Fem_Visualize::onIsShowCellNum);
	connect(ui.pushButtonPoint, &QPushButton::clicked, this, &Fem_Visualize::onIsShowNodeNum);
	//VisualizeWindow* window = new VisualizeWindow(ss);
	//ShowDisplacement();
	//ShowAxialForces();
}

Fem_Visualize::~Fem_Visualize()
{}

void Fem_Visualize::SetRenderWindow()
{
	std::cout << "Number of actors after removal: " << renderer->GetActors()->GetNumberOfItems() << std::endl;

	InitActors();
	InitNode(m_structure->m_Nodes);
	InitElement();
	std::cout << "Number of actors after create: " << renderer->GetActors()->GetNumberOfItems() << std::endl;

	ui.widget->renderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
	ui.widget->renderWindow()->Render();
}

void Fem_Visualize::InitActors()
{
	// 清空所有Actor
	renderer->RemoveAllViewProps();
	vtkNew<vtkAppendPolyData> newLinkFilter;
	vtkNew<vtkAppendFilter> newSoildFilter;

	linkAppendFilter = newLinkFilter;
	soildAppendFilter = newSoildFilter;

	// 清空数据
	points->Reset();
	linkAppendFilter->RemoveAllInputs();
	soildAppendFilter->RemoveAllInputs();
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
	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(bridge->appendPoints);

	std::cout << "There are " << polyData->GetNumberOfPoints() << " input points."
		<< std::endl;

	vtkIdType id;

	// Insert all of the points
	vtkNew<vtkMergePoints> mergePoints;
	mergePoints->SetDataSet(polyData);
	mergePoints->SetDivisions(10, 10, 10);
	mergePoints->InitPointInsertion(polyData->GetPoints(), polyData->GetBounds());

	//清空Node map 容器
	m_structure->m_Nodes.clear();
	//清空points
	points->Reset();
	int inserted;
	for (vtkIdType i = 0; i < polyData->GetNumberOfPoints(); i++)
	{
		polyData->GetPoint(i, inSet);
		inserted = mergePoints->InsertUniquePoint(inSet, id);
		std::cout << "\tPoint: " << inSet[0] << ", " << inSet[1] << ", " << inSet[2]
			<< " ";
		if (inserted != 0)
		{
			int idNode = (int)id;
			m_structure->m_Nodes.insert(
				make_pair(idNode, new NodeFem(idNode, inSet[0], inSet[1], inSet[2])));
			points->InsertNextPoint(inSet);
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
	vtkNew<vtkPoints> elementPoints;
	vtkNew<vtkStringArray> elementLabel;

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
		if (element.second->m_type == "Beam" || element.second->m_type == "Truss")
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
		elementPoints->InsertNextPoint(centerPoints);
		elementLabel->InsertNextValue(std::to_string(element.second->m_id));
	}

	vtkNew<vtkPoints> nodePoints;
	vtkNew<vtkStringArray> nodeLabel;
	nodeLabel->SetName("Labels");

	for (int i = 0; i < m_structure->m_Nodes.size(); i++)
	{
		double nodePoint[3];
		NodeFem* node = m_structure->Find_Node(i + 1);
		nodePoint[0] = node->m_x;
		nodePoint[1] = node->m_y + 0.01;
		nodePoint[2] = node->m_z;
		nodePoints->InsertNextPoint(nodePoint);
		nodeLabel->InsertNextValue(std::to_string(node->m_id));
	}
	elementLabel->SetName("Labels");
	GeneraterLable(elementPoints, elementLabel, elementLableActor);
	GeneraterLable(nodePoints, nodeLabel, nodeLabelActor);

	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points);
	polyData->SetLines(cellArray);

	vtkNew<vtkIdFilter> idFilter;
	idFilter->SetInputData(polyData);

	idFilter->SetCellIdsArrayName("OriginalIds");
	idFilter->SetPointIdsArrayName("OriginalIds");

	vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
	surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
	surfaceFilter->Update();

	linePolyData->DeepCopy(surfaceFilter->GetOutput());

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
		//linkMapper->SetScalarModeToUsePointData();
		linkActor->GetProperty()->SetEdgeColor(0, 0, 0);
		linkActor->GetProperty()->SetColor(0.8, 1, 1); // 淡蓝色
		linkActor->GetProperty()->EdgeVisibilityOn();
		linkActor->GetProperty()->SetLineWidth(1);
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
	lineActor->GetProperty()->SetLineWidth(2);
	lineActor->GetProperty()->SetColor(0, 0, 0);
	linkActor->SetMapper(linkMapper);

	renderer->AddActor(scalarBar);
	renderer->AddActor(elementLableActor);
	renderer->AddActor(nodeLabelActor);
}

void Fem_Visualize::InitElements()
{
	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(bridge->appendPoints);

	std::cout << "There are " << polyData->GetNumberOfPoints() << " input points."
		<< std::endl;

	// Insert all of the points
	vtkNew<vtkMergePoints> mergePoints;
	mergePoints->SetDataSet(polyData);
	mergePoints->SetDivisions(10, 10, 10);
	mergePoints->InitPointInsertion(polyData->GetPoints(), polyData->GetBounds());

	m_structure->m_Elements.clear();
	int elementId = 1;

	// 生成桥面板单元
	GennerateBeamElement(mergePoints, bridge->leftDeckPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->centerLeftDeckPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->centerRightDeckPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->rightDeckPoints, elementId);

	// 生成桥塔单元
	GennerateBeamElement(mergePoints, bridge->leftTowerPierPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->leftTowerPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->rightTowerPierPoints, elementId);
	GennerateBeamElement(mergePoints, bridge->rightTowerPoints, elementId);

	// 生成斜拉索单元
	GennerateRopeElement(mergePoints, bridge->leftRopePoints, elementId);
	GennerateRopeElement(mergePoints, bridge->centerLeftRopePoints, elementId);
	GennerateRopeElement(mergePoints, bridge->centerRightRopePoints, elementId);
	GennerateRopeElement(mergePoints, bridge->rightRopePoints, elementId);

	bridge->PrintPoint(bridge->leftDeckPoints);
	bridge->PrintPoint(bridge->centerLeftDeckPoints);
	bridge->PrintPoint(bridge->centerRightDeckPoints);
	bridge->PrintPoint(bridge->rightDeckPoints);

	renderer->RemoveAllViewProps();

	vtkNew<vtkCellArray> cellArray;

	for (auto a : m_structure->m_Elements)
	{
		Element_Base* element = a.second;
		vtkNew<vtkLine> line;
		line->GetPointIds()->SetId(0, element->m_idNode[0]);
		line->GetPointIds()->SetId(1, element->m_idNode[1]);
		cellArray->InsertNextCell(line);
	}

	vtkNew<vtkPoints> pts;
	vtkNew<vtkStringArray> label;
	double lablePoint[3];
	for (auto a : m_structure->m_Nodes)
	{
		NodeFem* node = a.second;
		lablePoint[0] = node->m_x;
		lablePoint[1] = node->m_y;
		lablePoint[2] = node->m_z;
		pts->InsertNextPoint(lablePoint);
		label->InsertNextValue(std::to_string(node->m_id));
	}

	label->SetName("Labels");
	GeneraterLable(pts, label, elementLableActor);

	vtkNew<vtkPolyData> lineData;
	lineData->SetPoints(points);
	lineData->SetLines(cellArray);

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputData(lineData);

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(5);

	vtkNew<vtkVertexGlyphFilter> vertexFiliter;
	vertexFiliter->SetInputData(lineData);

	vtkNew<vtkPolyDataMapper> vertexMapper;
	vertexMapper->SetInputConnection(vertexFiliter->GetOutputPort());

	vtkNew<vtkActor> vertexActor;
	vertexActor->SetMapper(vertexMapper);
	vertexActor->GetProperty()->SetColor(1, 0, 0);
	vertexActor->GetProperty()->SetPointSize(5);

	renderer->AddActor(actor);
	renderer->AddActor(vertexActor);
	renderer->AddActor(elementLableActor);
	this->ui.widget->renderWindow()->Render();
}

void Fem_Visualize::GennerateBeamElement(vtkMergePoints *mergePoints, vtkPoints *points, int &elementId)
{
	int inserted;
	double inSet[3];
	vtkIdType id;
	// 仅针对梁单元模型
	for (int i = 0; i < points->GetNumberOfPoints() - 1; i++)
	{
		LinkElement_Beam3D* element = new LinkElement_Beam3D();
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
}

void Fem_Visualize::GennerateRopeElement(vtkMergePoints* mergePoints, vtkPoints* points, int& elementId)
{
	int inserted;
	double inSet[3];
	vtkIdType id;
	for (int i = 0; i < points->GetNumberOfPoints() - 2; i +=2)
	{
		LinkElement_Truss3D* element = new LinkElement_Truss3D();
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
}

void Fem_Visualize::InitMaterial()
{
	m_structure->m_Material.clear();
	double density = 7800;
	double E = 2.1e10;
	double possion = 0.3;
	int id = 1;

	Material* pMaiterial = new Material(id, E, possion, density);
	m_structure->m_Material.insert(make_pair(id, pMaiterial));
}

void Fem_Visualize::InitSections()
{
	m_structure->m_Section.clear();
	int materialId = 1;
	int sectionId = 1;
	double area = 3.141593e-2;
	double Iy = 7.853982e-5;
	double Iz = 7.853982e-5;

	Section_Beam3D *pSection = new Section_Beam3D(sectionId, materialId, area, Iy, Iz);
	Section_Truss3D* pSection2 = new Section_Truss3D(2, 1, area);

	m_structure->m_Section.insert(make_pair(sectionId, pSection));
	m_structure->m_Section.insert(make_pair(2, pSection2));
}

void Fem_Visualize::InitBoundarys()
{
	m_structure->m_Boundary.clear();

	for (int i = 0; i < 6; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 0;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}

	for (int i = 0; i < 6; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 41;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}

	for (int i = 0; i < 6; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 43;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}

	for (int i = 0; i < 6; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 55;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}

	for (int i = 0; i < 3; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 10;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}

	for (int i = 0; i < 3; i++)
	{
		Boundary* pBoundary = new Boundary();
		pBoundary->m_id = m_structure->m_Boundary.size() + 1;
		pBoundary->m_direction = i;
		pBoundary->m_idNode = 32;
		pBoundary->m_value = 0;
		m_structure->m_Boundary.insert(make_pair(pBoundary->m_id, pBoundary));
	}
}

void Fem_Visualize::AssignSection()
{
	for (auto a : m_structure->m_Elements)
	{
		Element_Base* pElement = a.second;
		LinkElement_Beam3D* pBeam = dynamic_cast<LinkElement_Beam3D*>(pElement);
		LinkElement_Truss3D* pTruss = dynamic_cast<LinkElement_Truss3D*>(pElement);
		if (pBeam)
		{
			pBeam->m_idSection = 1;
		}
		else
		{
			pTruss->m_idSection = 2;
		}
	}
}

void Fem_Visualize::Init_Action()
{
	//新建菜单
	menu = new QMenu(this);              //添加菜单
	pointSelectMenu = new QMenu(this);   //添加点选子菜单
	windowSelectMenu = new QMenu(this);  //添加窗选子菜单

	//添加动作
	areaPickerNode = new QAction(this);
	areaPickerCell = new QAction(this); 
	pointPickerNode = new QAction(this); 
	pointPickerCell = new QAction(this);
	close_system = new QAction(this);;

	areaPickerNode->setText("节点");
	areaPickerCell->setText("单元");

	pointPickerNode->setText("节点");
	pointPickerCell->setText("单元");
	close_system->setText("退出");

	//添加菜单
	pointSelectMenu = menu->addMenu("点选");
	menu->addSeparator();//添加分割线
	windowSelectMenu = menu->addMenu("窗选");


	pointSelectMenu->addAction(pointPickerNode);
	pointSelectMenu->addAction(pointPickerCell);

	windowSelectMenu->addAction(areaPickerNode);
	windowSelectMenu->addAction(areaPickerCell);

	connect(areaPickerCell, &QAction::triggered, this, &Fem_Visualize::onAreaPickerCell);
	connect(areaPickerNode, &QAction::triggered, this, &Fem_Visualize::onAreaPickerNode);
	connect(pointPickerCell, &QAction::triggered, this, &Fem_Visualize::onPointPickerCell);
	connect(pointPickerNode, &QAction::triggered, this, &Fem_Visualize::onPointPickerNode);

	menu->addSeparator();//添加分割线
	menu->addAction(close_system);  //添加换皮肤动作
}

void Fem_Visualize::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug() << " 进入右键菜单栏";
	menu->exec(QCursor::pos()); //在光标当前位置处出现
	event->accept();
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

	//double nextValue = vtkMath::Random(0, 90);
	//double thirdValue = vtkMath::Random(0, 90);
	//double fouthValue = vtkMath::Random(0, 90);

	//int pointNum = extrusion->GetOutput()->GetNumberOfPoints();
	//for (int i = 0; i < pointNum / 4; i++)
	//{
	//	stress->InsertNextValue(value);
	//}
	//for (int i = 0; i < pointNum / 4; i++)
	//{
	//	stress->InsertNextValue(nextValue);
	//}
	//for (int i = 0; i < pointNum / 4; i++)
	//{
	//	stress->InsertNextValue(thirdValue);
	//}
	//for (int i = 0; i < pointNum / 4; i++)
	//{
	//	stress->InsertNextValue(fouthValue);
	//}
	//extrusion->GetOutput()->GetPointData()->SetScalars(stress);

	linkAppendFilter->AddInputConnection(extrusion->GetOutputPort());
	linkAppendFilter->Update();
}

void Fem_Visualize::onIsShowSection()
{
	if (showFlag == 0)
	{
		renderer->RemoveAllViewProps();
		renderer->AddActor(lineActor);
		renderer->AddActor(elementLableActor);
		ui.widget->renderWindow()->Render();
		showFlag = 1;
		return;
	}
	if (showFlag == 1);
	{
		renderer->RemoveAllViewProps();
		renderer->AddActor(linkActor);
		renderer->AddActor(elementLableActor);
		renderer->ResetCamera();
		ui.widget->renderWindow()->Render();
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
	ui.widget->renderWindow()->Render();
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
	ui.widget->renderWindow()->Render();
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
	ui.widget->renderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
	ui.widget->renderWindow()->Render();
}

void Fem_Visualize::onShowTowerModel()
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(bridge->deckLineDisplay->GetActor());
	renderer->AddActor(bridge->towerLineDisplay->GetActor());

	vtkSmartPointer<vtkRenderWindow> GetRenderWindow = ui.widget->renderWindow();
	GetRenderWindow->AddRenderer(renderer);
	GetRenderWindow->Render();

	ui.widget->update();
}


void Fem_Visualize::onShowRopeModel()
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(bridge->deckLineDisplay->GetActor());
	renderer->AddActor(ropeWindow->ropeActor);
	renderer->AddActor(bridge->towerLineDisplay->GetActor());

	vtkSmartPointer<vtkRenderWindow> GetRenderWindow = ui.widget->renderWindow();
	GetRenderWindow->AddRenderer(renderer);
	GetRenderWindow->Render();
	m_structure = new StructureFem();
	Element_Base::Set_Structure(m_structure);
	InitNodes();
	InitElements();
	InitMaterial();
	InitSections();
	InitBoundarys();
	AssignSection();
	m_structure->Analyse();
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

		vtkSmartPointer<vtkRenderWindow> GetRenderWindow = ui.widget->renderWindow();;
		GetRenderWindow->AddRenderer(renderer);
		GetRenderWindow->Render();

		ui.widget->update();
	}

	isPressed = !isPressed;
}

void Fem_Visualize::onSelectFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("选择文件"),
		QDir::currentPath(),
		tr("所有文件 (*.*);;文本文件 (*.txt);;图像文件 (*.png *.jpg)"));

	if (!fileName.isEmpty()) {
		// 用户选择了一个文件，fileName 包含了选择的文件的路径
		qDebug() << "选择的文件是：" << fileName;
		m_structure = new StructureFem();
		EntityBase::Set_Structure(m_structure);
		m_structure->Input_datas(fileName);
		SetRenderWindow();
	}
	else {
		// 用户取消了操作
		qDebug() << "用户取消了选择操作";
	}
	inVarWindow = new InVarWindow(this, m_structure);
}


void Fem_Visualize::onLoadWindow()
{
	loadWindow->exec();
}

void Fem_Visualize::onIsShowNodeNum()
{
	if (showFlag == 0)
	{
		renderer->AddActor(nodeLabelActor);
		ui.widget->renderWindow()->Render();
		showFlag = 1;
		return;
	}
	if (showFlag == 1);
	{
		renderer->RemoveActor(nodeLabelActor);
		ui.widget->renderWindow()->Render();
		showFlag = 0;
		return;
	}
}

void Fem_Visualize::onAnalyse()
{
	m_structure->Analyse();
}

void Fem_Visualize::onShowInVarWindow()
{
	inVarWindow->show();
}

void Fem_Visualize::onIsShowCellNum()
{
	if (showFlag == 0)
	{
		renderer->AddActor(elementLableActor);
		ui.widget->renderWindow()->Render();
		showFlag = 1;
		return;
	}
	if (showFlag == 1);
	{
		renderer->RemoveActor(elementLableActor);
		ui.widget->renderWindow()->Render();
		showFlag = 0;
		return;
	}
}

void Fem_Visualize::onAreaPickerCell()
{
	vtkRenderWindowInteractor* interactor = ui.widget->interactor();
	vtkNew<vtkAreaPicker> areaPicker;

	interactor->SetPicker(areaPicker);
	interactor->SetRenderWindow(ui.widget->renderWindow());

	vtkNew<AreaPickerCellInteractor> style;
	style->SetPoints(linePolyData);
	interactor->SetInteractorStyle(style);
}

void Fem_Visualize::onAreaPickerNode()
{
	vtkRenderWindowInteractor* interactor = ui.widget->interactor();

	vtkNew<vtkAreaPicker> areaPicker;
	interactor->SetPicker(areaPicker);
	interactor->SetRenderWindow(ui.widget->renderWindow());

	vtkNew<AreaPickerNodeInteractor> style;
	style->SetPoints(linePolyData);
	interactor->SetInteractorStyle(style);
}

void Fem_Visualize::onPointPickerCell()
{
	vtkRenderWindowInteractor* interactor = ui.widget->interactor();

	vtkNew<vtkCellPicker> cellPicker;
	cellPicker->SetTolerance(0.005);
	interactor->SetPicker(cellPicker);
	interactor->SetRenderWindow(ui.widget->renderWindow());

	vtkNew<PointPickerCellInteractor> style;
	style->SetDefaultRenderer(renderer);
	style->Data = linePolyData;
	interactor->SetInteractorStyle(style);
}

void Fem_Visualize::onPointPickerNode()
{
	vtkRenderWindowInteractor* interactor = ui.widget->interactor();

	vtkNew<vtkCellPicker> cellPicker;
	cellPicker->SetTolerance(0.005);
	interactor->SetPicker(cellPicker);
	interactor->SetRenderWindow(ui.widget->renderWindow());

	vtkNew<PointPickerNodeInteractor> style;
	style->SetDefaultRenderer(renderer);
	style->Data = linePolyData;
	interactor->SetInteractorStyle(style);
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

	vtkSmartPointer<vtkRenderWindow> GetRenderWindow = ui.widget->renderWindow();;
	GetRenderWindow->AddRenderer(renderer);
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
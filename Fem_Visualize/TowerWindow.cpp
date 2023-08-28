#include "TowerWindow.h"
#include <iostream>

TowerWindow::TowerWindow(BridgeProperty* bridge, QWidget *parent /*= nullptr*/) : QDialog(parent)
{
	ui.setupUi(this);

	towerBridge = bridge;
	connect(ui.buttonSure, &QPushButton::clicked, this, &TowerWindow::OnSureButtonClicked);
	connect(ui.buttonCancel, &QPushButton::clicked, this, &TowerWindow::OnCancelButtonClicked);
}

TowerWindow::~TowerWindow()
{}

void TowerWindow::OnSureButtonClicked()
{
	TubeSection* tubeSection = new TubeSection(0, "A", "user", "Tube");
	tubeSection->m_D = 5;
	tubeSection->m_tw = 0.5;

	// create four points for the cross section, centered at the origin
	vtkSmartPointer<vtkPoints> towerShapePoint = vtkSmartPointer<vtkPoints>::New();

	// 设置圆环参数
	int numSegments = 36;
	double outerRadius = tubeSection->m_D / 2;
	double innerRadius = outerRadius - tubeSection->m_tw;
	double deltaTheta = 2 * vtkMath::Pi() / numSegments;

	// 添加圆环的外侧点
	for (int i = 0; i < numSegments; ++i)
	{
		double theta = i * deltaTheta;
		double x = outerRadius * cos(theta);
		double y = outerRadius * sin(theta);
		double z = 0.0;
		vtkIdType pid = towerShapePoint->InsertNextPoint(x, y, z);
	}

	// 添加圆环的内侧点
	for (int i = 0; i < numSegments; ++i)
	{
		double theta = i * deltaTheta;
		double x = innerRadius * cos(theta);
		double y = innerRadius * sin(theta);
		double z = 0.0;
		vtkIdType pid = towerShapePoint->InsertNextPoint(x, y, z);
	}

	for (vtkIdType i = 0; i < towerShapePoint->GetNumberOfPoints(); i++)
	{
		double p[3];
		towerShapePoint->GetPoint(i, p);
	}

	QString towerSectionShape = "Tube";

	double startPoint[3] = {0};
	double endPoint[3] = {0};
	string outPut;
	// 生成左桥塔
	outPut = towerBridge->GenerateCoordinateString(ui.textLeftTower->text().toStdString());
	towerBridge->leftDeckPoints->GetPoint(towerBridge->leftDeckPoints->GetNumberOfPoints() - 1, startPoint);

	towerBridge->GeneratePoints(outPut, "y", startPoint, towerBridge->leftTowerPoints);
	towerBridge->leftTowerPoints->GetPoint(towerBridge->leftTowerPoints->GetNumberOfPoints() - 1, startPoint);

	startPoint[1] += ui.lineB->text().toDouble() - startPoint[1];
	towerBridge->leftTowerPoints->InsertNextPoint(startPoint);
	towerBridge->leftDeckPoints->GetPoint(towerBridge->leftDeckPoints->GetNumberOfPoints() - 1, startPoint);
	towerBridge->leftTowerPoints->GetPoint(towerBridge->leftTowerPoints->GetNumberOfPoints() - 1, endPoint);
	// 获取桥塔底座坐标
	double baseHeight = ui.lineH1->text().toDouble() - (endPoint[1] - startPoint[1]);
	towerBridge->GenerateBridgeTower_2(baseHeight, towerBridge->leftTowerPoints, startPoint, towerShapePoint, towerSectionShape);
	towerBridge->PrintPoint(towerBridge->leftTowerPoints);

	// 生成右桥塔
	outPut = towerBridge->GenerateCoordinateString(ui.textRightTower->text().toStdString());
	towerBridge->centerRightDeckPoints->GetPoint(towerBridge->centerRightDeckPoints->GetNumberOfPoints() - 1, startPoint);
	towerBridge->GeneratePoints(outPut, "y", startPoint, towerBridge->rightTowerPoints);
	towerBridge->rightTowerPoints->GetPoint(towerBridge->rightTowerPoints->GetNumberOfPoints() - 1, startPoint);
	startPoint[1] += ui.lineC->text().toDouble() - startPoint[1];
	towerBridge->rightTowerPoints->InsertNextPoint(startPoint);
	towerBridge->centerRightDeckPoints->GetPoint(towerBridge->centerRightDeckPoints->GetNumberOfPoints() - 1, startPoint);
	towerBridge->GenerateBridgeTower_2(baseHeight, towerBridge->rightTowerPoints, startPoint, towerShapePoint, towerSectionShape);
	towerBridge->PrintPoint(towerBridge->rightTowerPoints);

	vtkSmartPointer<vtkPoints> leftTowerPierPoints = vtkSmartPointer<vtkPoints>::New();
	towerBridge->leftTowerPoints->GetPoint(0, startPoint);
	startPoint[1] -= baseHeight;
	leftTowerPierPoints->InsertNextPoint(startPoint);
	towerBridge->leftTowerPoints->GetPoint(0, startPoint);
	leftTowerPierPoints->InsertNextPoint(startPoint);

	vtkSmartPointer<vtkPoints> rigntTowerPierPoints = vtkSmartPointer<vtkPoints>::New();
	towerBridge->rightTowerPoints->GetPoint(0, startPoint);
	startPoint[1] -= baseHeight;
	rigntTowerPierPoints->InsertNextPoint(startPoint);
	towerBridge->rightTowerPoints->GetPoint(0, startPoint);
	rigntTowerPierPoints->InsertNextPoint(startPoint);

	towerBridge->towerLineDisplay->CreateCell(towerBridge->leftTowerPoints);
	towerBridge->towerLineDisplay->CreateCell(towerBridge->rightTowerPoints);
	towerBridge->towerLineDisplay->CreateCell(leftTowerPierPoints);
	towerBridge->towerLineDisplay->CreateCell(rigntTowerPierPoints);
	towerBridge->towerLineDisplay->LinkActor();


	towerBridge->towerPointDisplay->CreateCell(towerBridge->leftTowerPoints);
	towerBridge->towerPointDisplay->CreateCell(towerBridge->rightTowerPoints);
	towerBridge->towerPointDisplay->CreateCell(leftTowerPierPoints);
	towerBridge->towerPointDisplay->CreateCell(rigntTowerPierPoints);
	towerBridge->towerPointDisplay->LinkActor();

	vtkSmartPointer<vtkPolyDataMapper> towerMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	// Create a mapper and actor for the 3D shape
	towerMapper->SetInputConnection(towerBridge->towerAppendFilter->GetOutputPort());

	towerActor->SetVisibility(1);
	towerActor->GetProperty()->SetColor(0.5, 0.75, 1.0);
	towerActor->GetProperty()->SetOpacity(0.5); // Set opacity to 0.5
	towerActor->SetMapper(towerMapper);

	this->hide();
	emit SignalShowTowerModel();
}

void TowerWindow::OnCancelButtonClicked()
{
	this->close();
}

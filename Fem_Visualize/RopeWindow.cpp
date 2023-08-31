#include "RopeWindow.h"

RopeWindow::RopeWindow(BridgeProperty* bridge, QWidget *parent /*= nullptr*/) : QDialog(parent)
{
	ui.setupUi(this);

	ropeBridge = bridge;
	connect(ui.buttonSure, &QPushButton::clicked, this, &RopeWindow::OnSureButtonClicked);
	connect(ui.pushButton_2, &QPushButton::clicked, this, &RopeWindow::OnCancelButtonClicked);
}

RopeWindow::~RopeWindow()
{}

void RopeWindow::OnSureButtonClicked()
{
	TubeSection* ropeSection = new TubeSection(0, "A", "user", "Tube");
	ropeSection->m_D = 0.5;
	ropeSection->m_tw = 0.1;

	// create four points for the cross section, centered at the origin
	vtkSmartPointer<vtkPoints> ropeShapePoint = vtkSmartPointer<vtkPoints>::New();

	// 设置圆环参数
	int numSegments = 36;
	double outerRadius = ropeSection->m_D / 2;
	double innerRadius = outerRadius - ropeSection->m_tw;
	double deltaTheta = 2 * vtkMath::Pi() / numSegments;

	// 添加圆环的外侧点
	for (int i = 0; i < numSegments; ++i)
	{
		double theta = i * deltaTheta;
		double x = outerRadius * cos(theta);
		double y = outerRadius * sin(theta);
		double z = 0.0;
		vtkIdType pid = ropeShapePoint->InsertNextPoint(x, y, z);
	}

	// 添加圆环的内侧点
	for (int i = 0; i < numSegments; ++i)
	{
		double theta = i * deltaTheta;
		double x = innerRadius * cos(theta);
		double y = innerRadius * sin(theta);
		double z = 0.0;
		vtkIdType pid = ropeShapePoint->InsertNextPoint(x, y, z);
	}

	for (vtkIdType i = 0; i < ropeShapePoint->GetNumberOfPoints(); i++)
	{
		double p[3];
		ropeShapePoint->GetPoint(i, p);
	}

	QString ropeSectionShape = "Tube";

	// 生成斜拉索
	ropeBridge->leftRopePoints = ropeBridge->GenerateInclinedRopes_2
	(ropeBridge->leftDeckPoints, ropeBridge->leftTowerPoints, ropeShapePoint, ropeSectionShape);

	// 生成斜拉索
	vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
	newPoints->SetNumberOfPoints(ropeBridge->centerLeftDeckPoints->GetNumberOfPoints());
	for (vtkIdType i = ropeBridge->centerLeftDeckPoints->GetNumberOfPoints() - 1, j = 0; i >= 0; i--, j++) {
		double point[3];
		ropeBridge->centerLeftDeckPoints->GetPoint(i, point);
		newPoints->SetPoint(j, point);
	}
	ropeBridge->centerLeftRopePoints = ropeBridge->GenerateInclinedRopes_2
	(newPoints, ropeBridge->leftTowerPoints, ropeShapePoint, ropeSectionShape);

	// 生成斜拉索
	ropeBridge->centerRightRopePoints = ropeBridge->GenerateInclinedRopes_2
	(ropeBridge->centerRightDeckPoints, ropeBridge->rightTowerPoints, ropeShapePoint, ropeSectionShape);

	// 生成斜拉索
	newPoints->SetNumberOfPoints(ropeBridge->rightDeckPoints->GetNumberOfPoints());
	for (vtkIdType i = ropeBridge->rightDeckPoints->GetNumberOfPoints() - 1, j = 0; i >= 0; i--, j++) {
		double point[3];
		ropeBridge->rightDeckPoints->GetPoint(i, point);
		newPoints->SetPoint(j, point);
	}
	ropeBridge->rightRopePoints = ropeBridge->GenerateInclinedRopes_2
	(newPoints, ropeBridge->rightTowerPoints, ropeShapePoint, ropeSectionShape);

	vtkSmartPointer<vtkPolyDataMapper> ropeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	// Create a mapper and actor for the 3D shape
	ropeMapper->SetInputConnection(ropeBridge->ropeAppendFilter->GetOutputPort());

	ropeActor->SetVisibility(1);
	ropeActor->GetProperty()->SetColor(0.5, 0.75, 1.0);
	ropeActor->GetProperty()->SetOpacity(0.5); // Set opacity to 0.5
	ropeActor->SetMapper(ropeMapper);

	this->hide();
	emit SignalShowRopeModel();
}

void RopeWindow::OnCancelButtonClicked()
{
	this->close();
}

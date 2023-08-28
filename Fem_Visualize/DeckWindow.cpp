#include "DeckWindow.h"
#include "SectionProperty.h"
//#include "MainWindow.h"
#include "QDebug"

DeckWindow::DeckWindow(BridgeProperty* bridge, QWidget *parent /*= nullptr*/) : QDialog(parent)
{
	ui.setupUi(this);

	deckBridge = bridge;
	connect(ui.sureButton, &QPushButton::clicked, this, &DeckWindow::OnSureButtonClicked);
}

DeckWindow::~DeckWindow()
{}

vtkSmartPointer<vtkActor> DeckWindow::GetActor()
{
	return deckActor;
}

void DeckWindow::OnSureButtonClicked()
{
	SoildRecSection* soildRecSection = new SoildRecSection(0, "A", "user", "SoildRec");
	soildRecSection->m_B = 30;
	soildRecSection->m_H = 5;
	qDebug() << "B:" << soildRecSection->m_B << "   H:" << soildRecSection->m_H;
	// create four points for the cross section, centered at the origin
	vtkSmartPointer<vtkPoints> deckShapePoint = vtkSmartPointer<vtkPoints>::New();
	deckShapePoint->InsertNextPoint(-soildRecSection->m_B / 2, -soildRecSection->m_H / 2, 0);
	deckShapePoint->InsertNextPoint(soildRecSection->m_B / 2, -soildRecSection->m_H / 2, 0);
	deckShapePoint->InsertNextPoint(soildRecSection->m_B / 2, soildRecSection->m_H / 2, 0);
	deckShapePoint->InsertNextPoint(-soildRecSection->m_B / 2, soildRecSection->m_H / 2, 0);

	for (vtkIdType i = 0; i < deckShapePoint->GetNumberOfPoints(); i++)
	{
		double p[3];
		deckShapePoint->GetPoint(i, p);
	}


	QString deckSectionShape = "SoildRec";

	const double startPointX = ui.textAX->text().toDouble();
	const double startPointY = ui.textAY->text().toDouble();
	double startPoint[3] = { startPointX, startPointY, 0 };
	double endPoint[3] = { 0 , 0, 0 };
	// 生成左边桥面板
	string  outPut = deckBridge->GenerateCoordinateString(ui.textLeftDeck->text().toStdString());
	deckBridge->GeneratePoints(outPut, "x", startPoint, deckBridge->leftDeckPoints);
	deckBridge->GenerateBridgeDecks(deckBridge->leftDeckPoints, deckShapePoint, deckSectionShape);


	// 生成中左桥面板
	outPut = deckBridge->GenerateCoordinateString(ui.textCenterLeftDeck->text().toStdString());
	deckBridge->leftDeckPoints->GetPoint(deckBridge->leftDeckPoints->GetNumberOfPoints() - 1, startPoint);
	deckBridge->GeneratePoints(outPut, "x", startPoint, deckBridge->centerLeftDeckPoints);
	deckBridge->GenerateBridgeDecks(deckBridge->centerLeftDeckPoints, deckShapePoint, deckSectionShape);

	// 生成中右桥面板
	outPut = deckBridge->GenerateCoordinateString(ui.textCenterRightDeck->text().toStdString());
	deckBridge->centerLeftDeckPoints->GetPoint(deckBridge->centerLeftDeckPoints->GetNumberOfPoints() - 1, startPoint);
	deckBridge->GeneratePoints(outPut, "x", startPoint, deckBridge->centerRightDeckPoints);
	deckBridge->GenerateBridgeDecks(deckBridge->centerRightDeckPoints, deckShapePoint, deckSectionShape);
	deckBridge->PrintPoint(deckBridge->centerRightDeckPoints);

	// 生成右边桥面板
	outPut = deckBridge->GenerateCoordinateString(ui.textRightDeck->text().toStdString());
	deckBridge->centerRightDeckPoints->GetPoint(deckBridge->centerRightDeckPoints->GetNumberOfPoints() - 1, startPoint);
	deckBridge->GeneratePoints(outPut, "x", startPoint, deckBridge->rightDeckPoints);
	deckBridge->GenerateBridgeDecks(deckBridge->rightDeckPoints, deckShapePoint, deckSectionShape);
	deckBridge->PrintPoint(deckBridge->rightDeckPoints);

	// Apply the triangle filter to create triangles from the extruded surface
	vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	triangleFilter->SetInputConnection(deckBridge->deckAppendFilter->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> deckMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	// Create a mapper and actor for the 3D shape
	deckMapper->SetInputConnection(triangleFilter->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> boundMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	// Create a mapper and actor for the 3D shape
	boundMapper->SetInputConnection(deckBridge->duckEdgesAppendFilter->GetOutputPort());

	deckBridge->PrintPoint(deckBridge->leftDeckPoints);
	deckBridge->PrintPoint(deckBridge->centerLeftDeckPoints);

	deckBridge->deckLineDisplay->CreateCell(deckBridge->centerLeftDeckPoints);
	deckBridge->deckLineDisplay->CreateCell(deckBridge->centerRightDeckPoints);
	deckBridge->deckLineDisplay->CreateCell(deckBridge->leftDeckPoints);
	deckBridge->deckLineDisplay->CreateCell(deckBridge->rightDeckPoints);
	deckBridge->deckLineDisplay->LinkActor();

	deckBridge->deckPointDisplay->CreateCell(deckBridge->centerLeftDeckPoints);
	deckBridge->deckPointDisplay->CreateCell(deckBridge->centerRightDeckPoints);
	deckBridge->deckPointDisplay->CreateCell(deckBridge->leftDeckPoints);
	deckBridge->deckPointDisplay->CreateCell(deckBridge->rightDeckPoints);
	deckBridge->deckPointDisplay->LinkActor();

	deckActor->SetVisibility(1);
	deckActor->GetProperty()->SetColor(0.5, 0.75, 1.0);
	deckActor->GetProperty()->SetOpacity(0.5); // Set opacity to 0.5
	deckActor->SetMapper(deckMapper);

	this->hide();
	emit SignalShowDeckModel();
}

void DeckWindow::OnCancelButtonClicked()
{
	this->close();
}


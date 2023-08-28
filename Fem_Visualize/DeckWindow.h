#pragma once

#include <QDialog>
#include "ui_DeckWindow.h"
#include "BridgeProperty.h"
#include "ShapeDisplay.h"

class DeckWindow : public QDialog
{
	Q_OBJECT

public:
	DeckWindow(BridgeProperty* bridge, QWidget *parent = nullptr);
	DeckWindow(BridgeProperty* bridge);
	~DeckWindow();

	BridgeProperty* deckBridge;

	vtkSmartPointer<vtkActor> deckActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> boundActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> towerActor = vtkSmartPointer<vtkActor>::New();

	vtkSmartPointer<vtkActor> GetActor();
signals:
	void SignalShowDeckModel();

private slots:
	void OnSureButtonClicked();
	void OnCancelButtonClicked();


private:
	Ui::DeckWindowClass ui;
};

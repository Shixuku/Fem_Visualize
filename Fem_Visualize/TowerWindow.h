#pragma once

#include <QDialog>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkMath.h>
#include "ui_TowerWindow.h"
#include "SectionProperty.h"
#include "BridgeProperty.h"

class TowerWindow : public QDialog
{
	Q_OBJECT

public:
	TowerWindow(BridgeProperty* bridge, QWidget *parent = nullptr);
	~TowerWindow();


	vtkSmartPointer<vtkActor>towerActor = vtkSmartPointer<vtkActor>::New();

signals:
	void SignalShowTowerModel();

private slots:
	void OnSureButtonClicked();
	void OnCancelButtonClicked();
private:
	Ui::TowerWindowClass ui;
	BridgeProperty *towerBridge;
};

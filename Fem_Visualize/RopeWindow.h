#pragma once

#include <QDialog>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include "ui_RopeWindow.h"
#include "BridgeProperty.h"
#include "SectionProperty.h"

class RopeWindow : public QDialog
{
	Q_OBJECT

public:
	RopeWindow(BridgeProperty* bridge, QWidget *parent = nullptr);
	~RopeWindow();
	BridgeProperty* ropeBridge;
	vtkSmartPointer<vtkActor> ropeActor = vtkSmartPointer<vtkActor>::New();

signals:
	void SignalShowRopeModel();

private slots:
	void OnSureButtonClicked();
	void OnCancelButtonClicked();

private:
	Ui::RopeWindowClass ui;
};

#pragma once
#pragma execution_character_set("utf-8")//��ֹ������������

#include <QDialog>
#include "ui_InVarWindow.h"
#include "StructureFem.h"
#include "inVar.h"
#include "InVar_Truss3D.h"
#include "InVar_Truss2D.h"
//#include "AreaPickerCellInteractor.h"



class InVarWindow : public QDialog
{
	Q_OBJECT

public:
	InVarWindow(QWidget *parent = nullptr);
	InVarWindow(QWidget *parent = nullptr, StructureFem *structure = nullptr);
	~InVarWindow();

private slots:
	void onSureAddInvar();

private:
	Ui::InVarWindowClass ui;
	StructureFem * m_structure;
};

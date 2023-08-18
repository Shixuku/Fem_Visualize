#include "DisplacementWindow.h"

DisplacementWindow::DisplacementWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.pushButtonClose, &QPushButton::clicked, this, &DisplacementWindow::onCloseWindow);
	connect(ui.pushButtonSure, &QPushButton::clicked, this, &DisplacementWindow::onSendTypeSignal);
}

DisplacementWindow::~DisplacementWindow()
{}

void DisplacementWindow::onCloseWindow()
{
	this->close();
}

void DisplacementWindow::onSendTypeSignal()
{
	if (ui.radioButtonDX->isChecked())
	{
		dispType = DX;
	}
	else if (ui.radioButtonDY->isChecked())
	{
		dispType = DY;
	}
	else if (ui.radioButtonDZ->isChecked())
	{
		dispType = DZ;
	}
	else if (ui.radioButtonDXY->isChecked())
	{
		dispType = DXY;
	}
	else if (ui.radioButtonDYZ->isChecked())
	{
		dispType = DYZ;
	}
	else if (ui.radioButtonDXZ->isChecked())
	{
		dispType = DXZ;
	}
	else if (ui.radioButtonDXYZ->isChecked())
	{
		dispType = DXYZ;
	}

	emit signalSendForceType();
}

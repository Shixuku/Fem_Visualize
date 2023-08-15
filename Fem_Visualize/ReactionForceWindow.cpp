#include "ReactionForceWindow.h"

ReactionForceWindow::ReactionForceWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pushButtonClose, &QPushButton::clicked, this, &ReactionForceWindow::onCloseWindow);
	connect(ui.pushButtonSure, &QPushButton::clicked, this, &ReactionForceWindow::onSendTypeSignal);

}

ReactionForceWindow::~ReactionForceWindow()
{}

void ReactionForceWindow::onCloseWindow()
{
	this->close();
}

void ReactionForceWindow::onSendTypeSignal()
{
	if (ui.radioButtonFX->isChecked())
	{
		forceType = FX;
	}
	else if (ui.radioButtonFY->isChecked())
	{
		forceType = FY;
	}
	else if (ui.radioButtonFZ->isChecked())
	{
		forceType = FZ;
	}
	else if (ui.radioButtonFXYZ->isChecked())
	{
		forceType = FXYZ;
	}
	else if (ui.radioButtonMX->isChecked())
	{
		forceType = MX;
	}
	else if (ui.radioButtonMY->isChecked())
	{
		forceType = MY;
	}
	else if (ui.radioButtonMZ->isChecked())
	{
		forceType = MZ;
	}
	else if (ui.radioButtonMXYZ->isChecked())
	{
		forceType = MXYZ;
	}

	emit signalSendForceType();
}

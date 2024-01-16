#include "LoadWindow.h"

LoadWindow::LoadWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.radioButtonAdd->setChecked(1);
}

LoadWindow::~LoadWindow()
{}

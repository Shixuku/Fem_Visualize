#pragma once

#include <QDialog>
#include "ui_LoadWindow.h"

class LoadWindow : public QDialog
{
	Q_OBJECT

public:
	LoadWindow(QWidget *parent = nullptr);
	~LoadWindow();

private:
	Ui::LoadWindowClass ui;
};

#pragma once

#include <QWidget>
#include "ui_I_Tower.h"

class I_Tower : public QWidget
{
	Q_OBJECT

public:
	I_Tower(QWidget *parent = nullptr);
	~I_Tower();

private:
	Ui::I_TowerClass ui;
};

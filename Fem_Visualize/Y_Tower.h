#pragma once

#include <QWidget>
#include "ui_Y_Tower.h"

class Y_Tower : public QWidget
{
	Q_OBJECT

public:
	Y_Tower(QWidget *parent = nullptr);
	~Y_Tower();

private:
	Ui::Y_TowerClass ui;
};

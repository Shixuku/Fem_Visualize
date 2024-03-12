#pragma once

#include <QWidget>
#include "ui_A_Tower.h"

class A_Tower : public QWidget
{
	Q_OBJECT

public:
	A_Tower(QWidget *parent = nullptr);
	~A_Tower();

private:
	Ui::A_TowerClass ui;
};

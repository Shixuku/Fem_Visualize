#pragma once

#include <QWidget>
#include "ui_HUA_Tower.h"

class HUA_Tower : public QWidget
{
	Q_OBJECT

public:
	HUA_Tower(QWidget *parent = nullptr);
	~HUA_Tower();

private:
	Ui::HUA_TowerClass ui;
};

#pragma once

#include <QWidget>
#include "ui_H_Tower.h"
#pragma execution_character_set("utf-8")//防止出现中文乱码
class H_Tower : public QWidget
{
	Q_OBJECT

public:
	H_Tower(QWidget *parent = nullptr);
	~H_Tower();

private:
	Ui::H_TowerClass ui;
};

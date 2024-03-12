#pragma once
#pragma execution_character_set("utf-8")//防止出现中文乱码
#include <QWidget>
#include "ui_TowerTest.h"

class TowerTest : public QWidget
{
	Q_OBJECT

public:
	TowerTest(QWidget *parent = nullptr);
	~TowerTest();

private:
	Ui::TowerTestClass ui;
};

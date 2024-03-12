#include "Y_Tower.h"
#include <QPixmap>
#include <QDebug>
#include <QTableWidget>

#pragma execution_character_set("utf-8")//防止出现中文乱码

Y_Tower::Y_Tower(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QPixmap map(":/Fem_Visualize/Y");
	if (map.isNull()) {
		qDebug() << "图片加载失败";
	}
	else {
		ui.label_11->setPixmap(map.scaled(ui.label_11->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}

	QTableWidget* table = new QTableWidget();
	// 设置表格的行数和列数
	table->setRowCount(5); // 5行
	table->setColumnCount(3); // 3列

	//ui.tabWidget->addTab(table, "表格分页");
	table->setColumnWidth(0, 50);
	table->setColumnWidth(1, 100);
	table->setColumnWidth(2, 100);
	table->resizeRowsToContents();


	table->setHorizontalHeaderItem(0, new QTableWidgetItem("单元划分长度"));
	table->setHorizontalHeaderItem(1, new QTableWidgetItem("单元划分个数"));
	ui.tabWidget_2->insertTab(0, table, "上塔");
	ui.tabWidget_2->setTabText(1, "中塔");
	ui.tabWidget_2->setTabText(2, "下塔");
	ui.tabWidget_2->setTabText(3, "横梁");

	setWindowTitle("索塔参数输入");
}

Y_Tower::~Y_Tower()
{}

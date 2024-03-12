#include "H_Tower.h"
#include <QPixmap>
#include <QDebug>
#include <QTableWidget>

H_Tower::H_Tower(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QPixmap map(":/Fem_Visualize/H");
	if (map.isNull()) {
		qDebug() << "ͼƬ����ʧ��";
	}
	else {
		ui.label_2->setPixmap(map.scaled(ui.label_2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}

	QTableWidget* table = new QTableWidget();
	// ���ñ�������������
	table->setRowCount(5); // 5��
	table->setColumnCount(3); // 3��

	//ui.tabWidget->addTab(table, "����ҳ");
	table->setColumnWidth(0, 50);
	table->setColumnWidth(1, 100);
	table->setColumnWidth(2, 100);
	table->resizeRowsToContents();


	table->setHorizontalHeaderItem(0, new QTableWidgetItem("��Ԫ���ֳ���"));
	table->setHorizontalHeaderItem(1, new QTableWidgetItem("��Ԫ���ָ���"));
	ui.tabWidget->insertTab(0, table, "����");
	ui.tabWidget->setTabText(1, "����");
	ui.tabWidget->setTabText(2, "����");

	setWindowTitle("������������");

}

H_Tower::~H_Tower()
{}

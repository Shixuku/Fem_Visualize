#include "I_Tower.h"
#include <QPixmap>
#include <QDebug>
#include <QTableWidget>
#pragma execution_character_set("utf-8")//��ֹ������������

I_Tower::I_Tower(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QPixmap map(":/Fem_Visualize/I");
	if (map.isNull()) {
		qDebug() << "ͼƬ����ʧ��";
	}
	else {
		ui.label_38->setPixmap(map.scaled(ui.label_38->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
	ui.tabWidget_5->insertTab(0, table, "����");

	setWindowTitle("������������");
}

I_Tower::~I_Tower()
{}

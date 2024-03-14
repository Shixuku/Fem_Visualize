#include "InVarWindow.h"
#include "qdebug.h"
#include <QMessageBox>

InVarWindow::InVarWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}


InVarWindow::InVarWindow(QWidget* parent /*= nullptr*/, StructureFem* structure /*= nullptr*/)
{
	ui.setupUi(this);
	m_structure = structure;

	connect(ui.pushButtonSure, &QPushButton::clicked, this, &InVarWindow::onSureAddInvar);
	// 将LineEdit的editingFinished信号连接到槽函数
	connect(ui.lineEditForce, &QLineEdit::dragEnabled, this, [=]() {
		// 在编辑完成时设置Label的文本
		ui.lineEditForce->setText("checked");
		});
	
}

InVarWindow::~InVarWindow()
{}

void InVarWindow::onSureAddInvar()
{
	int inVarId = 0;

	if (ui.lineEditForce->text().isEmpty() && ui.lineEditInVar->text().isEmpty())
	{
		// 弹出错误窗口
		QMessageBox::critical(this, "Error", "缺少参数", QMessageBox::Ok);
		return;
	}

	if (ui.radioButtonISel->isChecked() && ui.lineEditElement->text().isEmpty() || 
		(!ui.radioButtonISel->isChecked() && !ui.radioButtonAll->isChecked()))
	{
		// 弹出错误窗口
		QMessageBox::critical(this, "Error", "请输入单元号", QMessageBox::Ok);
		return;
	}

	if (!m_structure->m_InVar.empty()) {
		int inVarId = m_structure->m_InVar.rbegin()->second->m_id;
	}

	// 新元素的key值是最后一个元素的key值+1
	int newKey = inVarId + 1;

	InVar_Truss3D* inVar = new InVar_Truss3D();
	inVar->m_id = newKey;
	inVar->m_idElement = ui.lineEditElement->text().toInt();
	inVar->m_Type = 1;
	if (ui.radioButtonInVar->isChecked())
	{
		inVar->m_bFixedValue = 1;
		inVar->m_Value = ui.lineEditInVar->text().toDouble();
	}
	else if (ui.radioButtonForce->isChecked())
	{
		inVar->m_bFixedValue = 0;
		inVar->m_Force = ui.lineEditForce->text().toDouble();
	}

	m_structure->m_InVar.insert({ newKey, inVar });

	qDebug() << "添加内变量";
	std::cout << inVar->m_id << " " << inVar->m_idElement << " " << inVar->m_Type << " " <<
		inVar->m_bFixedValue << " " << inVar->m_Force << std::endl;
	this->close();
}

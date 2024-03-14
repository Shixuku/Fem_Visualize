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
	// ��LineEdit��editingFinished�ź����ӵ��ۺ���
	connect(ui.lineEditForce, &QLineEdit::dragEnabled, this, [=]() {
		// �ڱ༭���ʱ����Label���ı�
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
		// �������󴰿�
		QMessageBox::critical(this, "Error", "ȱ�ٲ���", QMessageBox::Ok);
		return;
	}

	if (ui.radioButtonISel->isChecked() && ui.lineEditElement->text().isEmpty() || 
		(!ui.radioButtonISel->isChecked() && !ui.radioButtonAll->isChecked()))
	{
		// �������󴰿�
		QMessageBox::critical(this, "Error", "�����뵥Ԫ��", QMessageBox::Ok);
		return;
	}

	if (!m_structure->m_InVar.empty()) {
		int inVarId = m_structure->m_InVar.rbegin()->second->m_id;
	}

	// ��Ԫ�ص�keyֵ�����һ��Ԫ�ص�keyֵ+1
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

	qDebug() << "����ڱ���";
	std::cout << inVar->m_id << " " << inVar->m_idElement << " " << inVar->m_Type << " " <<
		inVar->m_bFixedValue << " " << inVar->m_Force << std::endl;
	this->close();
}

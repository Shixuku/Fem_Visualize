#pragma once
#include "Element_Base.h"
#include <vector>
#include <fstream>

class LinkElement_Base : public Element_Base
{
public:
	Eigen::MatrixXd m_T;		 // ת������
	Eigen::MatrixXd m_Lambda;	 // �任����
	Eigen::MatrixXd m_Ke;		 // ����նȾ���
	Eigen::MatrixXd m_Me;		 // ������������
	Eigen::VectorXd m_EqForce;   // ��Ԫ��Ч��Ԫ��
	double m_t;                  // ��Ԫ���

	int Get_DOF_Node() override { return 2; }

	void Input_Data(std::ifstream& fin) override;

	void Disp() override;

	// ��������նȾ��� Ke = T.t * ke * T
	Eigen::MatrixXd calculate_Ke() override { return m_T.transpose() * m_ke * m_T; }

	// ���������������� Me = T.t * me * T
	Eigen::MatrixXd calculate_Me() override { return m_T.transpose() * m_me * m_T; }

	// ���㵥Ԫ�նȾ���
	void calculate_ke() {}

	// ���㵥Ԫ��������
	void calculate_me() {}

	void calculate_all() {}

	// ���㵥Ԫ����
	virtual void calculate_internal_force(Eigen::VectorXd disp) = 0;

	// ����ת������ �����嵽��Ԫ
	virtual void calculate_T() = 0;

	void calculate_volume();
};


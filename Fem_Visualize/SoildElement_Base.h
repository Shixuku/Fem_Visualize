#pragma once
#include <Eigen/Dense>
#include <vector>
#include "EntityBase.h"
#include "Element_Base.h"
#include <iostream>
#include <Eigen/Dense>

class SoildElement_Base : public Element_Base
{
public:
	Eigen::MatrixXd m_B;         // ��ԪӦ�����
	Eigen::MatrixXd m_D;         // ��Ԫ��������
	Eigen::VectorXd m_Stress;    // ��ԪӦ��
	int m_idMaterial;             // ���Ϻ�
	double m_t;                  // ��Ԫ���  
	std::string m_type;          // ��Ԫ����

	// ��ʼ���ڵ����ɶ�
	int Get_DOF_Node() override { m_dof = 2;  return 2;}

	// ��ʾ����
	void Disp() override;

	// ��ȡ����
	void Input_Data(std::ifstream& fin) override {}

	// ���㵥Ԫ�նȾ��� Ke = B.T * D * B 
	void calculate_ke() { m_t * m_volume * m_B.transpose() * m_D * m_B; }

	// ���㵥Ԫ��������
	void calculate_me() {}

	// ���㵥Ԫ��������
	virtual void calculate_D() = 0;

	// ���㵥ԪӦ�����
	virtual void calculate_B() = 0;

	// �������
	virtual void calculate_volume() = 0;

	// ������������в���
	void calculate_all() {}

	SoildElement_Base();
};


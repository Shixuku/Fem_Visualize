#pragma once
#include <Eigen/Dense>
#include <vector>
#include "EntityBase.h"
#include <iostream>

class SoildElement_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //��Ԫ�Ľڵ��
	int m_idSection;             //�����
	Eigen::MatrixXd m_T;		 // ת������
	Eigen::MatrixXd m_Lambda;	 // �任����
	Eigen::MatrixXd m_Ke;		 // ��Ԫ�նȾ���
	Eigen::MatrixXd m_Me;		 // ��Ԫ��������
	Eigen::MatrixXd m_B;         // ��ԪӦ�����
	Eigen::MatrixXd m_D;         // ��Ԫ��������
	Eigen::VectorXd m_Stress;    // ��ԪӦ��
	double m_t;                  // ��Ԫ���  
	double m_dim;                // ��Ԫά��
	double m_volume;             // ��Ԫ�����һά��Ԫ���س��ȣ���ά��Ԫ�������
	double m_dof;
	std::string m_type;          // ��Ԫ����

	// ��ʼ���ڵ����ɶ�
	virtual int Get_DOF_Node() { m_dof = 2;  return 2;}

	// ���㵥Ԫ�նȾ��� Ke = B.T * D * B 
	virtual void calculate_Ke();

	// ���㵥Ԫ��������
	virtual void calculate_D(double E, double nu) = 0;

	// ���㵥ԪӦ�����
	virtual void calculate_B() = 0;

	// �������
	virtual void calculate_volume() = 0;

	SoildElement_Base();
};


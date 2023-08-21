#pragma once
#include "EntityBase.h"


class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //��Ԫ�Ľڵ��
	int m_idSection;             //�����
	Eigen::MatrixXd m_ke;		 // ��Ԫ�նȾ���
	Eigen::MatrixXd m_me;		 // ��Ԫ��������
	double m_dim;                // ��Ԫά��
	double m_volume;             // ��Ԫ�����һά��Ԫ���س��ȣ���ά��Ԫ�������
	double m_dof;				 // ��Ԫ���ɶ�
	std::string m_type;          // ��Ԫ����

	//ÿ���ڵ�����ɶȸ���
	virtual int Get_DOF_Node() = 0;

	//���øõ�Ԫ�Ľڵ�����ɶȸ���
	void Set_NodeDOF();  

	//��ʾ����
	virtual void Disp(); 

	//��ȡ����
	virtual void Input_Data(std::ifstream& fin);

	// ���㵥Ԫ�նȾ���
	virtual void calculate_ke() = 0;

	// ���㵥Ԫ��������
	virtual void calculate_me() = 0;
};


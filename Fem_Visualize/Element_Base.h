#pragma once
#include "EntityBase.h"
#include <Eigen/Dense>


class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //��Ԫ�Ľڵ��
	int m_idSection;             //�����
	Eigen::MatrixXd m_ke;		 // ��Ԫ�նȾ���
	Eigen::MatrixXd m_me;		 // ��Ԫ��������
	Eigen::MatrixXd m_B;         // ��ԪӦ�����
	Eigen::MatrixXd m_D;         // ��Ԫ��������
	Eigen::VectorXd m_force;	 // ��Ԫ����
	double m_dim;                // ��Ԫά��
	double m_volume;             // ��Ԫ�����һά��Ԫ���س��ȣ���ά��Ԫ�������
	double m_dof;				 // ��Ԫ���ɶ�
	std::string m_type;          // ��Ԫ����

	// ÿ���ڵ�����ɶȸ���
	virtual int Get_DOF_Node() = 0;

	// ���øõ�Ԫ�Ľڵ�����ɶȸ���
	void Set_NodeDOF();  

	// ��ʾ����
	virtual void Disp(); 

	// ��ȡ����
	virtual void Input_Data(std::ifstream& fin);

	// ��װ�նȾ���
	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);

	// ���㵥Ԫ�նȾ���
	virtual void calculate_ke() = 0;

	// ���㵥Ԫ��������
	virtual void calculate_me() = 0;

	// ��������նȾ��� Ke = T.t * ke * T
	virtual Eigen::MatrixXd calculate_Ke() { return m_ke; };

	// ���������������� Me = T.t * me * T
	virtual Eigen::MatrixXd calculate_Me() { return m_me; };

	// ������������в���
	virtual void calculate_all() = 0;

	Element_Base();
};


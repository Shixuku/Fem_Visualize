#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //��Ԫ�Ľڵ��
	int m_idSection;             //�����
	Eigen::MatrixXd m_T;		 // ת������
	Eigen::MatrixXd m_Lambda;	 // �任����
	Eigen::MatrixXd m_ke;		 // ��Ԫ�նȾ���
	Eigen::MatrixXd m_Ke;		 // ����նȾ���
	Eigen::MatrixXd m_me;		 // ��Ԫ��������
	Eigen::MatrixXd m_Me;		 // ������������
	Eigen::MatrixXd m_B;         // ��ԪӦ�����
	Eigen::MatrixXd m_D;         // ��Ԫ��������
	Eigen::VectorXd m_Force;     // ��Ԫ��
	double m_t;                  // ��Ԫ���  
	double m_dim;                // ��Ԫά��
	double m_volume;             // ��Ԫ�����һά��Ԫ���س��ȣ���ά��Ԫ�������
	std::string m_type;          // ��Ԫ����

	virtual int Get_DOF_Node() = 0;//ÿ���ڵ�����ɶȸ���
	void Set_NodeDOF();            //���øõ�Ԫ�Ľڵ�����ɶȸ���

	virtual void Disp();           //��ʾ����

	virtual void Get_ke(MatrixXd& ke) = 0;

	virtual void Input_Data(std::ifstream& fin);

	void calculate_Ke();

	void calculate_Me();

	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);

	virtual void calculate_ke() = 0;

	virtual void calculate_me() = 0;

	virtual void calculate_T() = 0;
};


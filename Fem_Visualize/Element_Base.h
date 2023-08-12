#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;//��Ԫ�Ľڵ��
	int m_idSection;          //�����
	MatrixXd m_T;             // ת������
	MatrixXd m_Lamda;         // �任����

	virtual int Get_DOF_Node() = 0;//ÿ���ڵ�����ɶȸ���
	void Set_NodeDOF();            //���øõ�Ԫ�Ľڵ�����ɶȸ���

	virtual void Get_ke(MatrixXd& ke) = 0;

	virtual void Disp();           //��ʾ����

	virtual void Input_Data(std::ifstream& fin);

	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);
};


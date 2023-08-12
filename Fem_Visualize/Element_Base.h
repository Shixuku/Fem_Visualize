#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;//单元的节点号
	int m_idSection;          //截面号
	MatrixXd m_T;             // 转换矩阵
	MatrixXd m_Lamda;         // 变换矩阵

	virtual int Get_DOF_Node() = 0;//每个节点的自由度个数
	void Set_NodeDOF();            //设置该单元的节点的自由度个数

	virtual void Get_ke(MatrixXd& ke) = 0;

	virtual void Disp();           //显示数据

	virtual void Input_Data(std::ifstream& fin);

	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);
};


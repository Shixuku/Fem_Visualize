#pragma once
#include "EntityBase.h"


class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //单元的节点号
	int m_idSection;             //截面号
	Eigen::MatrixXd m_ke;		 // 单元刚度矩阵
	Eigen::MatrixXd m_me;		 // 单元质量矩阵
	double m_dim;                // 单元维度
	double m_volume;             // 单元体积，一维单元返回长度，二维单元返回面积
	double m_dof;				 // 单元自由度
	std::string m_type;          // 单元类型

	//每个节点的自由度个数
	virtual int Get_DOF_Node() = 0;

	//设置该单元的节点的自由度个数
	void Set_NodeDOF();  

	//显示数据
	virtual void Disp(); 

	//读取数据
	virtual void Input_Data(std::ifstream& fin);

	// 计算单元刚度矩阵
	virtual void calculate_ke() = 0;

	// 计算单元质量矩阵
	virtual void calculate_me() = 0;
};


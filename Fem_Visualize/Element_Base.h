#pragma once
#include "EntityBase.h"
#include <Eigen/Dense>


class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //单元的节点号
	int m_idSection;             //截面号
	Eigen::MatrixXd m_ke;		 // 单元刚度矩阵
	Eigen::MatrixXd m_me;		 // 单元质量矩阵
	Eigen::MatrixXd m_B;         // 单元应变矩阵
	Eigen::MatrixXd m_D;         // 单元本构矩阵
	Eigen::VectorXd m_force;	 // 单元内力
	double m_dim;                // 单元维度
	double m_volume;             // 单元体积，一维单元返回长度，二维单元返回面积
	double m_dof;				 // 单元自由度
	std::string m_type;          // 单元类型

	// 每个节点的自由度个数
	virtual int Get_DOF_Node() = 0;

	// 设置该单元的节点的自由度个数
	void Set_NodeDOF();  

	// 显示数据
	virtual void Disp(); 

	// 读取数据
	virtual void Input_Data(std::ifstream& fin);

	// 组装刚度矩阵
	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);

	// 计算单元刚度矩阵
	virtual void calculate_ke() = 0;

	// 计算单元质量矩阵
	virtual void calculate_me() = 0;

	// 计算整体刚度矩阵 Ke = T.t * ke * T
	virtual Eigen::MatrixXd calculate_Ke() { return m_ke; };

	// 计算整体质量矩阵 Me = T.t * me * T
	virtual Eigen::MatrixXd calculate_Me() { return m_me; };

	// 计算所需的所有参数
	virtual void calculate_all() = 0;

	Element_Base();
};


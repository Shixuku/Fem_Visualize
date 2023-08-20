#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

class Element_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //单元的节点号
	int m_idSection;             //截面号
	Eigen::MatrixXd m_T;		 // 转换矩阵
	Eigen::MatrixXd m_Lambda;	 // 变换矩阵
	Eigen::MatrixXd m_ke;		 // 单元刚度矩阵
	Eigen::MatrixXd m_Ke;		 // 整体刚度矩阵
	Eigen::MatrixXd m_me;		 // 单元质量矩阵
	Eigen::MatrixXd m_Me;		 // 整体质量矩阵
	Eigen::MatrixXd m_B;         // 单元应变矩阵
	Eigen::MatrixXd m_D;         // 单元本构矩阵
	Eigen::VectorXd m_Force;     // 单元力
	double m_t;                  // 单元厚度  
	double m_dim;                // 单元维度
	double m_volume;             // 单元体积，一维单元返回长度，二维单元返回面积
	std::string m_type;          // 单元类型

	virtual int Get_DOF_Node() = 0;//每个节点的自由度个数
	void Set_NodeDOF();            //设置该单元的节点的自由度个数

	virtual void Disp();           //显示数据

	virtual void Get_ke(MatrixXd& ke) = 0;

	virtual void Input_Data(std::ifstream& fin);

	void calculate_Ke();

	void calculate_Me();

	void Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22);

	virtual void calculate_ke() = 0;

	virtual void calculate_me() = 0;

	virtual void calculate_T() = 0;
};


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
	Eigen::MatrixXd m_B;         // 单元应变矩阵
	Eigen::MatrixXd m_D;         // 单元本构矩阵
	Eigen::VectorXd m_Stress;    // 单元应力
	int m_idMaterial;             // 材料号
	double m_t;                  // 单元厚度  
	std::string m_type;          // 单元类型

	// 初始化节点自由度
	int Get_DOF_Node() override { m_dof = 2;  return 2;}

	// 显示数据
	void Disp() override;

	// 读取数据
	void Input_Data(std::ifstream& fin) override {}

	// 计算单元刚度矩阵 Ke = B.T * D * B 
	void calculate_ke() { m_t * m_volume * m_B.transpose() * m_D * m_B; }

	// 计算单元质量矩阵
	void calculate_me() {}

	// 计算单元本构矩阵
	virtual void calculate_D() = 0;

	// 计算单元应变矩阵
	virtual void calculate_B() = 0;

	// 计算面积
	virtual void calculate_volume() = 0;

	// 计算所需的所有参数
	void calculate_all() {}

	SoildElement_Base();
};


#pragma once
#include "Element_Base.h"
#include <vector>
#include <fstream>

class LinkElement_Base : public Element_Base
{
public:
	Eigen::MatrixXd m_T;		 // 转换矩阵
	Eigen::MatrixXd m_Lambda;	 // 变换矩阵
	Eigen::MatrixXd m_Ke;		 // 整体刚度矩阵
	Eigen::MatrixXd m_Me;		 // 整体质量矩阵
	Eigen::VectorXd m_EqForce;   // 单元等效单元力
	double m_t;                  // 单元厚度

	int Get_DOF_Node() override { return 2; }

	void Input_Data(std::ifstream& fin) override;

	void Disp() override;

	// 计算整体刚度矩阵 Ke = T.t * ke * T
	Eigen::MatrixXd calculate_Ke() override { return m_T.transpose() * m_ke * m_T; }

	// 计算整体质量矩阵 Me = T.t * me * T
	Eigen::MatrixXd calculate_Me() override { return m_T.transpose() * m_me * m_T; }

	// 计算单元刚度矩阵
	void calculate_ke() {}

	// 计算单元质量矩阵
	void calculate_me() {}

	void calculate_all() {}

	// 计算单元内力
	virtual void calculate_internal_force(Eigen::VectorXd disp) = 0;

	// 计算转换矩阵 从整体到单元
	virtual void calculate_T() = 0;

	void calculate_volume();
};


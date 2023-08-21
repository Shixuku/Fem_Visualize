#pragma once
#include <Eigen/Dense>
#include <vector>
#include "EntityBase.h"
#include <iostream>

class SoildElement_Base : public EntityBase
{
public:
	std::vector<int> m_idNode;   //单元的节点号
	int m_idSection;             //截面号
	Eigen::MatrixXd m_T;		 // 转换矩阵
	Eigen::MatrixXd m_Lambda;	 // 变换矩阵
	Eigen::MatrixXd m_Ke;		 // 单元刚度矩阵
	Eigen::MatrixXd m_Me;		 // 单元质量矩阵
	Eigen::MatrixXd m_B;         // 单元应变矩阵
	Eigen::MatrixXd m_D;         // 单元本构矩阵
	Eigen::VectorXd m_Stress;    // 单元应力
	double m_t;                  // 单元厚度  
	double m_dim;                // 单元维度
	double m_volume;             // 单元体积，一维单元返回长度，二维单元返回面积
	double m_dof;
	std::string m_type;          // 单元类型

	// 初始化节点自由度
	virtual int Get_DOF_Node() { m_dof = 2;  return 2;}

	// 计算单元刚度矩阵 Ke = B.T * D * B 
	virtual void calculate_Ke();

	// 计算单元本构矩阵
	virtual void calculate_D(double E, double nu) = 0;

	// 计算单元应变矩阵
	virtual void calculate_B() = 0;

	// 计算面积
	virtual void calculate_volume() = 0;

	SoildElement_Base();
};


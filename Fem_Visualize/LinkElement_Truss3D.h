#pragma once
#include "LinkElement_Base.h"
class LinkElement_Truss3D : public LinkElement_Base
{
public:
	LinkElement_Truss3D();
	LinkElement_Truss3D(int id, int idNode1, int idNode2);

	Eigen::MatrixXd m_R;
	virtual int Get_DOF_Node() override { return 2; }

	void calculate_ke() override;
	void calculate_me() override;
	void calculate_T() override;
	void calculate_internal_force(Eigen::VectorXd disp) override;

	void Equivalent_Force();
	
	void calculate_all();

	double Get_Lcs(double& c, double& s) const;
	double Get_Lcst(double& c, double& s, double &t) const;

	void Get_EA(double& E, double& A) const;

	void Get_N(double& N, double& Stress);//得到内力

	void Get_ke();//计算单元单元刚度矩阵
};


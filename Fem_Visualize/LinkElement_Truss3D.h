#pragma once
#include "LinkElement_Base.h"
class LinkElement_Truss3D : public LinkElement_Base
{
public:
	LinkElement_Truss3D();
	LinkElement_Truss3D(int id, int idNode1, int idNode2);

	Eigen::MatrixXd R;
	virtual int Get_DOF_Node() override { return 3; }

	void calculate_ke()override;
	void calculate_me()override;
	void calculate_T()override;
	void calculate_internal_force(Eigen::VectorXd disp)override;

	void Equivalent_Force();

	void calculate_all();
};


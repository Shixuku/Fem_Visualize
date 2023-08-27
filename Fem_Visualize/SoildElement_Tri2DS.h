#pragma once
#include "SoildElement_Base.h"
#include "NodeFem.h"
#include "EntityBase.h"
class SoildElement_Tri2DS :
    public SoildElement_Base
{
public:
	void calculate_D() override;

	void calculate_B() override;

	void calculate_volume() override;

	void calculate_all() override;

	void calculate_ke() override;

	void Disp() override;

	void Input_Data(std::ifstream& fin);

	void calculate_Stress(Eigen::VectorXd disp);

	int Get_DOF_Node() override;

	SoildElement_Tri2DS();
};


#pragma once
#include "SoildElement_Base.h"
#include "NodeFem.h"
#include "EntityBase.h"
class SoildElement_Tri2DS :
    public SoildElement_Base
{
public:
	void calculate_D(double E, double nu) override;

	void calculate_B() override;

	void calculate_volume() override;

	int Get_DOF_Node() override;
};


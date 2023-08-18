#pragma once
#include "Element_Base.h"
class Element_Truss3D : public Element_Base
{
public:
	Element_Truss3D();

	virtual int Get_DOF_Node() override { return 3; }
	void Get_ke(MatrixXd& ke) override;
};

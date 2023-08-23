#pragma once
#include "LinkElement_Base.h"
class Element_Truss3D : public LinkElement_Base
{
public:
	Element_Truss3D();
	Element_Truss3D(int id, int idNode1, int idNode2);

	virtual int Get_DOF_Node() override { return 3; }

	void calculate_ke()override;
	void calculate_me()override;
	void calculate_T()override;
};


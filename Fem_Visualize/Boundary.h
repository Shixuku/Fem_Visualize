#pragma once
#include "EntityBase.h"
class Boundary : public EntityBase
{
public:
	int m_idNode;
	int m_ixyz;               //0:x,1,y,2:z
	double m_value = 0;

	void Disp();

	void Set_x1(VectorXd& x1);//����߽�����
};


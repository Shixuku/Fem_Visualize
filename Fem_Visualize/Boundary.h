#pragma once
#include "EntityBase.h"
class Boundary : public EntityBase
{
public:
	int m_idNode;
	int m_direction;               //0:x,1,y,2:z
	double m_value = 0;

	Boundary();
	Boundary(int id, int idNode, int ixyz, double value);
	void Disp();

	void Set_x1(VectorXd& x1);//处理边界条件
};


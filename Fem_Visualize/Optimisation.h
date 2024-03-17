#pragma once
#include "EntityBase.h"
class Optimisation :
    public EntityBase
{
public:
	int m_idNode;
	int m_direction;               //0:x,1,y,2:z

	Optimisation();
	Optimisation(int id, int idNode, int ixyz);
	void Disp();

	void Set_x1(VectorXd& x1);//处理边界条件
};


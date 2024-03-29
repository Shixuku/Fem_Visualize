#pragma once
#include "EntityBase.h"
class ForceNode : public EntityBase
{
public:
	int m_idNode;
	int m_ixyz;           //0:x,1:y,2:z
	double m_value;
	ForceNode();
	ForceNode(int id, int idNode, int ixyz, int value);

	void Disp();

	void Set_F1F2(VectorXd& F1, VectorXd& F2);
	void Set_F1F2F3(VectorXd& F1, VectorXd& F2, VectorXd& F3);
	void Set_F1F2F3F4(VectorXd& F1, VectorXd& F2, VectorXd& F3, VectorXd& F4);
};


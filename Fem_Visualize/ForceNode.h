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

	void Assemble_F1_F2(VectorXd& F1, VectorXd& F2);
};


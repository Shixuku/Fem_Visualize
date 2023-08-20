#pragma once
#include "EntityBase.h"
class NodeFem : public EntityBase
{
public:
	double m_x, m_y, m_z;
	std::vector<int> m_DOF;   //�����ɶȵı��

	std::vector<double> m_ReactionForce;  // �ڵ㴦��֧������


	VectorXd m_Displacement;
	Eigen::VectorXd m_Force;
	bool boundaryFlag = false;

	NodeFem();
	~NodeFem();
};


#pragma once
#include "EntityBase.h"
class NodeFem : public EntityBase
{
public:
	double m_x, m_y, m_z;
	std::vector<int> m_DOF;   //各自由度的编号

	std::vector<double> m_ReactionForce;  // 节点处的支座反力


	VectorXd m_Displacement;
	Eigen::VectorXd m_Force;
	bool boundaryFlag = false;

	NodeFem();
	~NodeFem();
};


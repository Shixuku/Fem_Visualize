#pragma once
#include "EntityBase.h"
class NodeFem : public EntityBase
{
public:
	double m_x, m_y, m_z;
	std::vector<int> m_DOF;   //各自由度的编号

	std::vector<double> m_Disp;   //各自由度对应的位移

	std::vector<double> m_ReactionForce;  // 节点处的支座反力


	VectorXd m_Displacement;
	Eigen::VectorXd m_Force;
	bool boundaryFlag = false;

	NodeFem();
	void PrintDis();

	NodeFem(int Id, double x, double y, double z);
	~NodeFem();
};


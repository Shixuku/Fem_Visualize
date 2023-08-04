#pragma once
#include "EntityBase.h"
class NodeFem : public EntityBase
{
public:
	double m_x, m_y, m_z;
	std::vector<int> m_DOF;   //各自由度的编号

	NodeFem();
	~NodeFem();
};


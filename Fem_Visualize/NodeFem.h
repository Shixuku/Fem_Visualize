#pragma once
#include "EntityBase.h"
class NodeFem : public EntityBase
{
public:
	double m_x, m_y, m_z;
	std::vector<int> m_DOF;   //�����ɶȵı��

	NodeFem();
	~NodeFem();
};


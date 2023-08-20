#include "NodeFem.h"
NodeFem::NodeFem()
{
	// 将节点处的支座反力均置为0
	m_ReactionForce.resize(6);
	std::fill(m_ReactionForce.begin(), m_ReactionForce.end(), 0.0);

	m_Displacement.resize(6);
	m_Displacement.setZero();

	m_Force.resize(6);
	m_Force.setZero();
}

NodeFem::~NodeFem()
{
	
}
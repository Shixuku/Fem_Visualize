#include "NodeFem.h"
NodeFem::NodeFem()
{
	// ���ڵ㴦��֧����������Ϊ0
	m_ReactionForce.resize(6);
	std::fill(m_ReactionForce.begin(), m_ReactionForce.end(), 0.0);
}

NodeFem::~NodeFem()
{
	
}
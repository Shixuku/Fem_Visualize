#include "NodeFem.h"
NodeFem::NodeFem()
{
	// 将节点处的支座反力均置为0
	m_ReactionForce.resize(6);
	std::fill(m_ReactionForce.begin(), m_ReactionForce.end(), 0.0);

	m_Displacement.setZero();

	m_Force.resize(6);
	m_Force.setZero();
}

NodeFem::NodeFem(int Id, double x, double y, double z)
{
	*this = NodeFem();
	m_id = Id;
	m_x = x;
	m_y = y;
	m_z = z;
}

void NodeFem::PrintDis()
{
	std::cout << "Node " << m_id << " displacement: ";
	for (auto& disp : m_Disp) {
		std::cout << disp << " ";
	}
	std::cout << "\n";
}

NodeFem::~NodeFem()
{
	
}
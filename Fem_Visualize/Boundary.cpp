#include "Boundary.h"
#include "NodeFem.h"
#include "StructureFem.h"

Boundary::Boundary(int id, int idNode, int ixyz, double value)
{
	m_id = id;
	m_idNode = idNode;
	m_direction = ixyz;
	m_value = value;
}

Boundary::Boundary()
{

}

void Boundary::Disp()
{
	std::cout << m_id << " " << m_idNode << " " << m_direction << " " << m_value << "\n";
}

void Boundary::Set_x1(VectorXd& x1)
{
	NodeFem* pNode = Get_Structure()->Find_Node(m_idNode);
	int it = pNode->m_DOF[m_direction];
	x1[it] = m_value;
}

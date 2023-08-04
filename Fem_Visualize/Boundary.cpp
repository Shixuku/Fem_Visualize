#include "Boundary.h"
#include "NodeFem.h"
#include "StructureFem.h"

void Boundary::Disp()
{
	std::cout << m_id << " " << m_idNode << " " << m_ixyz << " " << m_value << "\n";
}

void Boundary::Set_x1(VectorXd& x1)
{
	NodeFem* pNode = Get_Structure()->Find_Node(m_idNode);
	int it = pNode->m_DOF[m_ixyz];
	x1[it] = m_value;

}

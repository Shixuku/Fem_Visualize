#include "ForceNode.h"
#include <iostream>
#include "NodeFem.h"
#include "StructureFem.h"

ForceNode::ForceNode(int id, int idNode, int ixyz, int value)
{
	m_id = id;
	m_idNode = idNode;
	m_ixyz = ixyz;
	m_value = value;
}

ForceNode::ForceNode()
{

}

void ForceNode::Disp()
{
	std::cout << m_id << " " << m_idNode << " " << m_ixyz << " " << m_value << "\n";
}

void ForceNode::Set_F1F2(VectorXd& F1, VectorXd& F2)
{
	NodeFem* pNode = Get_Structure()->Find_Node(m_idNode);
	int it = pNode->m_DOF[m_ixyz];
	int& nFixed = Get_Structure()->m_nFixed;
	if (it < nFixed )
	{
		F1[it] += m_value;
	}
	else
	{
		F2[it - nFixed] += m_value;
	}
}

void ForceNode::Set_F1F2F3(VectorXd& F1, VectorXd& F2, VectorXd& F3)
{
	NodeFem* pNode = Get_Structure()->Find_Node(m_idNode);
	int it = pNode->m_DOF[m_ixyz];
	int& nFixed = Get_Structure()->m_nFixed;
	int& nOptims = Get_Structure()->m_nOptimis;
	if (it < nFixed)
	{
		F1[it] += m_value;
	}
	else if (it >= nFixed && it < nOptims) 
	{
		F2[it - nFixed] += m_value;
	}
	else
	{
		F3[it - nOptims] += m_value;
	}
}

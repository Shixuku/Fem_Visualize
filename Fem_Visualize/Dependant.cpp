#include "Dependant.h"
#include "NodeFem.h"
#include "StructureFem.h"

void Dependant::Set_HostSize()
{
}

void Dependant::Set_DOF(int& iStart)
{//处理主从信息的自由度
	StructureFem* pStructure = Get_Structure();
	NodeFem* mainNode = pStructure->Find_Node(m_idMainNode);//本身从节点
	NodeFem* subNode = pStructure->Find_Node(m_idSubNode);//本身从节点
	int tempConstrian = m_constrainValue;

	// 分配主节点自由度
	for (int i = 0; i < mainNode->m_DOF.size(); ++i)
	{
		int dof = mainNode->m_DOF[i];
		//主节点分配自由度
		if (dof == -1) dof = iStart++;
		mainNode->m_DOF[i] = iStart;
	}

	// 分配从节点自由度
	for (int i = 0; i < mainNode->m_DOF.size(); ++i)
	{
		int dof = mainNode->m_DOF[i];

		tempConstrian >>= 1;
		//主节点分配自由度
		if ((tempConstrian & 1) && (dof == -1))
		{
			subNode->m_DOF[i] = iStart++;
		}

		else if ( dof == -1)
		{
			subNode->m_DOF[i] = mainNode->m_DOF[i];
		}
	}
}

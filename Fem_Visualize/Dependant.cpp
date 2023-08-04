#include "Dependant.h"
#include "NodeFem.h"
#include "StructureFem.h"

void Dependant::Set_HostSize()
{
}

void Dependant::Set_DOF(int& iStart)
{//处理主从信息的自由度
	StructureFem* pStructure = Get_Structure();
	NodeFem* pNode = pStructure->Find_Node(m_idNode);//本身从节点
	for (int i = 0; i < 3; ++i)
	{
		if (m_idHost[i] > 0)
		{//该自由度为从属自由度
			//自由度对应的主节点
			NodeFem* pNodeHost = pStructure->Find_Node(m_idHost[i]);
			for (auto& dof : pNodeHost->m_DOF)
			{//主节点分配自由度
				if (dof == -1) dof = iStart++;
			}
			pNode->m_DOF[i] = pNodeHost->m_DOF[i];
		}
		else
		{//该自由度为独立自由度
			pNode->m_DOF[i] = iStart++;
		}
	}

}

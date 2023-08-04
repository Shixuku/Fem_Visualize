#include "Dependant.h"
#include "NodeFem.h"
#include "StructureFem.h"

void Dependant::Set_HostSize()
{
}

void Dependant::Set_DOF(int& iStart)
{//����������Ϣ�����ɶ�
	StructureFem* pStructure = Get_Structure();
	NodeFem* pNode = pStructure->Find_Node(m_idNode);//����ӽڵ�
	for (int i = 0; i < 3; ++i)
	{
		if (m_idHost[i] > 0)
		{//�����ɶ�Ϊ�������ɶ�
			//���ɶȶ�Ӧ�����ڵ�
			NodeFem* pNodeHost = pStructure->Find_Node(m_idHost[i]);
			for (auto& dof : pNodeHost->m_DOF)
			{//���ڵ�������ɶ�
				if (dof == -1) dof = iStart++;
			}
			pNode->m_DOF[i] = pNodeHost->m_DOF[i];
		}
		else
		{//�����ɶ�Ϊ�������ɶ�
			pNode->m_DOF[i] = iStart++;
		}
	}

}

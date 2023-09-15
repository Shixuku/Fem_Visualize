#include "Dependant.h"
#include "NodeFem.h"
#include "StructureFem.h"

void Dependant::Set_HostSize()
{
}

void Dependant::Set_DOF(int& iStart)
{//����������Ϣ�����ɶ�
	StructureFem* pStructure = Get_Structure();
	NodeFem* mainNode = pStructure->Find_Node(m_idMainNode);//����ӽڵ�
	NodeFem* subNode = pStructure->Find_Node(m_idSubNode);//����ӽڵ�
	int tempConstrian = m_constrainValue;

	// �������ڵ����ɶ�
	for (int i = 0; i < mainNode->m_DOF.size(); ++i)
	{
		int dof = mainNode->m_DOF[i];
		//���ڵ�������ɶ�
		if (dof == -1) dof = iStart++;
		mainNode->m_DOF[i] = iStart;
	}

	// ����ӽڵ����ɶ�
	for (int i = 0; i < mainNode->m_DOF.size(); ++i)
	{
		int dof = mainNode->m_DOF[i];

		tempConstrian >>= 1;
		//���ڵ�������ɶ�
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

#include "Element_Base.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include <fstream>

void Element_Base::Set_NodeDOF()
{
	int nDOF_Node = Get_DOF_Node();
	StructureFem* pStructure = Get_Structure();

	for (auto& a : m_idNode)
	{
		NodeFem* pNode = pStructure->Find_Node(a);
		if (pNode->m_DOF.size() < nDOF_Node)
		{
			pNode->m_DOF.resize(nDOF_Node);
		}
	}
}

void Element_Base::Disp()
{
	std::cout << m_id << " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}
	std::cout << "\n";
}

void Element_Base::Input_Data(std::ifstream& fin)
{
	fin >> m_id;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
}

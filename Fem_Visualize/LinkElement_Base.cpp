#include "LinkElement_Base.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include <iostream>


void LinkElement_Base::Disp()
{//Ωÿ√Ê–≈œ¢
	std::cout << m_id  <<" " << m_idSection<< " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}
	std::cout << "\n";
}

void LinkElement_Base::Input_Data(std::ifstream& fin)
{
	fin >> m_id;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
}

void LinkElement_Base::calculate_volume()
{
	StructureFem* pSt = Get_Structure();
	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);

	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	m_volume = sqrt(dx * dx + dy * dy + dz * dz);
}


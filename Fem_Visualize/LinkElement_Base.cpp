#include "LinkElement_Base.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include <iostream>



void LinkElement_Base::Set_NodeDOF()
{//设置该单元的节点的自由度个数
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

void LinkElement_Base::Disp()
{//截面信息
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

void LinkElement_Base::Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22)
{
	int nFixed = Get_Structure()->m_nFixed;
	int nNode = m_idNode.size();
	int nDOF_Node = Get_DOF_Node();
	int nDOF_Element = nNode * nDOF_Node;


	std::vector<int> EI;//单元的全部节点的整体自由度编号
	EI.resize(nDOF_Element);
	int k = 0;
	for (int i = 0; i < nNode; i++)
	{
		NodeFem* pNode = Get_Structure()->Find_Node(m_idNode[i]);
		for (int j = 0; j < nDOF_Node; j++)
		{
			EI[k] = pNode->m_DOF[j];
			++k;
		}
	}

	for (int i = 0; i < nDOF_Element; i++)
	{//行循环
		int ii = EI[i];//行自由度的整体自由度编号
		for (int j = 0; j < nDOF_Element; ++j)
		{//对列循环
			int jj = EI[j];//列自由度的整体自由度编号
			double kij = m_Ke(i, j);
			if (ii < nFixed && jj < nFixed)
			{
				K11.push_back(Tri(ii, jj, kij));
			}
			else if (ii >= nFixed && jj < nFixed)
			{
				K21.push_back(Tri(ii - nFixed, jj, kij));
			}
			else if (ii >= nFixed && jj >= nFixed)
			{
				K22.push_back(Tri(ii - nFixed, jj - nFixed, kij));
			}
		}
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

void LinkElement_Base::calculate_Ke()
{
	m_Ke = m_T.transpose() * m_ke * m_T;
}

void LinkElement_Base::calculate_Me()
{
	m_Me = m_T.transpose() * m_me * m_T;
}

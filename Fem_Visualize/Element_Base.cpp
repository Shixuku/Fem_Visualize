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
			pNode->m_Displacement.resize(nDOF_Node);
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

void Element_Base::Assemble_ke(std::list<Tri>& K11, std::list<Tri>& K21, std::list<Tri>& K22)
{
	Eigen::MatrixXd ke = calculate_Ke();

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
			double kij = ke(i, j);
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

Element_Base::Element_Base()
{

}

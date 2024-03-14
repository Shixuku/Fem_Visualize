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

void Element_Base::Get_DOFs(std::vector<int>& DOFs)
{
	//得到单元各节点的自由度编号
	StructureFem* pStructure = Get_Structure();

	int nDOF = Get_DOF_Node();//单元类型确定的节点自由度个数
	auto nNode = m_idNode.size();//节点个数
	auto nDOF_Element = nDOF * nNode;//单元的自由度个数
	DOFs.resize(nDOF_Element);

	int k = 0;
	for (auto& idNode : m_idNode)
	{//对节点编号循环
		NodeFem* pNode = pStructure->Find_Node(idNode);//找节点
		for (int j = 0; j < nDOF; ++j)
		{//对节点自由度循环
			DOFs[k++] = pNode->m_DOF[j];
		}
	}
}

void Element_Base::Assemble_L(ListTri& L11, ListTri& L21, ListTri& L22)
{
	Eigen::MatrixXd ke;
	if (m_type == "Beam")
	{
		ke = calculate_Ke();
	}
	else if (m_type == "Truss")
	{
		ke = m_ke;
	}

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
				L11.push_back(Tri(ii, jj, kij));
			}
			else if (ii >= nFixed && jj < nFixed)
			{
				L21.push_back(Tri(ii - nFixed, jj, kij));
			}
			else if (ii >= nFixed && jj >= nFixed)
			{
				L22.push_back(Tri(ii - nFixed, jj - nFixed, kij));
			}
		}
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

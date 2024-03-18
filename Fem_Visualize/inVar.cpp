#include "InVar.h"
#include "StructureFem.h"
#include <iostream>

void InVar::Input(std::ifstream& fin)
{
	fin >> m_id >> m_idElement >> m_Type;
	int FixedValue = 0;
	fin >> FixedValue;
	if (FixedValue > 0)
	{//指定内变量
		m_bFixedValue = true;
		fin >> m_Value;
	}
	else
	{//指定内力（应力）
		m_bFixedValue = false;
		fin >> m_Force;
	}
}

void InVar::Get_ke(std::vector<double>& ke)
{//得到刚度矩阵元素
	if (m_ke.size() == 0)
	{
		Calculate_ke();
	}
	ke = m_ke;
}

void InVar::Assemble_L(ListTri& L11, ListTri& L21, ListTri& L22)
{
	StructureFem* pStructure = Get_Structure();
	Element_Base* pElement = pStructure->Find_Element(m_idElement);
	std::vector<double> ke;
	Get_ke(ke);//计算内变量相关的刚度系数
	std::cout << "invar: [";
	for (size_t i = 0; i < ke.size(); ++i) {
		std::cout << ke[i];
		if (i < ke.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << std::endl;

	std::vector<int> DOFs_Element;
	pElement->Get_DOFs(DOFs_Element);
	auto nDOF = DOFs_Element.size();//单元的自由度个数
	int nFixed = pStructure->m_nFixed; // 固定自由度数
	int nShousuo = pStructure->m_nShousuo - nFixed; // 非弹性收缩量自由度数
	int nOptimis = pStructure->m_nOptimis - pStructure->m_nShousuo; // 待优化自由度数
	int nFree = pStructure->m_nTotv - pStructure->m_nOptimis; // 自由自由度数

	if (m_Itotv < nShousuo+ nFixed && m_Itotv >= nFixed)
	{//指定了内变量，内力为待求的未知量
		for (int i = 0; i < nDOF; ++i)
		{
			int itElement = DOFs_Element[i];//单元的节点自由度
			const double& ki = ke[i];
			if (itElement < nFixed)
			{//L11
				L11.push_back(Tri(m_Itotv, itElement, ki));
				L11.push_back(Tri(itElement, m_Itotv, ki));
			}
			else
			{//L21
				L21.push_back(Tri(itElement - nFixed, m_Itotv, ki));
			}
		}
		L11.push_back(Tri(m_Itotv, m_Itotv, ke[nDOF]));//组装对角线元素
	}
	else
	{//内变量自由度没有指定，指定的是内力
		int ii = m_Itotv - nFixed;
		for (int i = 0; i < nDOF; ++i)
		{
			int itElement = DOFs_Element[i];//单元的节点自由度
			const double& ki = ke[i];
			if (itElement < nFixed)
			{//L21
				L21.push_back(Tri(ii, itElement, ki));
			}
			else
			{//L22
				L22.push_back(Tri(ii, itElement - nFixed, ki));
				L22.push_back(Tri(itElement - nFixed, ii, ki));
			}
		}
		L22.push_back(Tri(ii, ii, ke[nDOF]));
	}
}

void InVar::Assemble_L3(ListTri& L11, ListTri& L21, ListTri& L22, ListTri& L31, ListTri& L32, ListTri& L33, ListTri& L41, ListTri& L42, ListTri& L43, ListTri& L44)
{
	StructureFem* pStructure = Get_Structure();
	Element_Base* pElement = pStructure->Find_Element(m_idElement);
	std::vector<double> ke;
	Get_ke(ke);//计算内变量相关的刚度系数

	std::vector<int> DOFs_Element;
	pElement->Get_DOFs(DOFs_Element);
	auto nDOF = DOFs_Element.size();//单元的自由度个数
	int nFixed = pStructure->m_nFixed; // 固定自由度数
	int nShousuo = pStructure->m_nShousuo - nFixed; // 非弹性收缩量自由度数
	int nOptimis = pStructure->m_nOptimis - pStructure->m_nShousuo; // 待优化自由度数
	int nFree = pStructure->m_nTotv - pStructure->m_nOptimis; // 自由自由度数

	if (m_Itotv < nShousuo + nFixed && m_Itotv >= nFixed)
	{    // 指定了内变量，内力为待求的未知量，并且属于非弹性收缩量自由度组
		int m_Itotv_rel = m_Itotv - nFixed; // 将内变量自由度转换为相对于非弹性收缩量自由度组的索引
		int nums = 1;
		for (int i = 0; i < nDOF; ++i) {
			int itElement = DOFs_Element[i]; // 单元的节点自由度
			const double& ki = ke[i]; // 对应的刚度系数

			if (itElement < nFixed) {
				// L12: 非弹性收缩量-固定
				L21.push_back(Tri(m_Itotv_rel, itElement, ki));
			}
			else if (itElement < nFixed + nShousuo) {
				// L22: 非弹性收缩量-非弹性收缩量
				L22.push_back(Tri(m_Itotv_rel, itElement - nFixed, ki));
			}
			else if (itElement < nFixed + nShousuo + nOptimis) {
				// L32: 待优化-非弹性收缩量
				L32.push_back(Tri(itElement - nFixed - nShousuo, m_Itotv_rel, ki));
			}
			else {
				// L42: 自由-非弹性收缩量
				L42.push_back(Tri(itElement - nFixed - nShousuo - nOptimis, m_Itotv_rel, ki));
			}
			nums++;
		}
		// 对角线元素在非弹性收缩量自由度组内
		L22.push_back(Tri(m_Itotv_rel, m_Itotv_rel, ke[nDOF]));

		std::cout << "nums" << nums << std::endl;
	}
}

void InVar::Disp() const
{
	std::cout << m_id << "  " << m_Value << "  " << m_Force << "\n";
}

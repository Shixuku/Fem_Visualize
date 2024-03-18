#include "InVar.h"
#include "StructureFem.h"
#include <iostream>

void InVar::Input(std::ifstream& fin)
{
	fin >> m_id >> m_idElement >> m_Type;
	int FixedValue = 0;
	fin >> FixedValue;
	if (FixedValue > 0)
	{//ָ���ڱ���
		m_bFixedValue = true;
		fin >> m_Value;
	}
	else
	{//ָ��������Ӧ����
		m_bFixedValue = false;
		fin >> m_Force;
	}
}

void InVar::Get_ke(std::vector<double>& ke)
{//�õ��նȾ���Ԫ��
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
	Get_ke(ke);//�����ڱ�����صĸն�ϵ��
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
	auto nDOF = DOFs_Element.size();//��Ԫ�����ɶȸ���
	int nFixed = pStructure->m_nFixed; // �̶����ɶ���
	int nShousuo = pStructure->m_nShousuo - nFixed; // �ǵ������������ɶ���
	int nOptimis = pStructure->m_nOptimis - pStructure->m_nShousuo; // ���Ż����ɶ���
	int nFree = pStructure->m_nTotv - pStructure->m_nOptimis; // �������ɶ���

	if (m_Itotv < nShousuo+ nFixed && m_Itotv >= nFixed)
	{//ָ�����ڱ���������Ϊ�����δ֪��
		for (int i = 0; i < nDOF; ++i)
		{
			int itElement = DOFs_Element[i];//��Ԫ�Ľڵ����ɶ�
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
		L11.push_back(Tri(m_Itotv, m_Itotv, ke[nDOF]));//��װ�Խ���Ԫ��
	}
	else
	{//�ڱ������ɶ�û��ָ����ָ����������
		int ii = m_Itotv - nFixed;
		for (int i = 0; i < nDOF; ++i)
		{
			int itElement = DOFs_Element[i];//��Ԫ�Ľڵ����ɶ�
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
	Get_ke(ke);//�����ڱ�����صĸն�ϵ��

	std::vector<int> DOFs_Element;
	pElement->Get_DOFs(DOFs_Element);
	auto nDOF = DOFs_Element.size();//��Ԫ�����ɶȸ���
	int nFixed = pStructure->m_nFixed; // �̶����ɶ���
	int nShousuo = pStructure->m_nShousuo - nFixed; // �ǵ������������ɶ���
	int nOptimis = pStructure->m_nOptimis - pStructure->m_nShousuo; // ���Ż����ɶ���
	int nFree = pStructure->m_nTotv - pStructure->m_nOptimis; // �������ɶ���

	if (m_Itotv < nShousuo + nFixed && m_Itotv >= nFixed)
	{    // ָ�����ڱ���������Ϊ�����δ֪�����������ڷǵ������������ɶ���
		int m_Itotv_rel = m_Itotv - nFixed; // ���ڱ������ɶ�ת��Ϊ����ڷǵ������������ɶ��������
		int nums = 1;
		for (int i = 0; i < nDOF; ++i) {
			int itElement = DOFs_Element[i]; // ��Ԫ�Ľڵ����ɶ�
			const double& ki = ke[i]; // ��Ӧ�ĸն�ϵ��

			if (itElement < nFixed) {
				// L12: �ǵ���������-�̶�
				L21.push_back(Tri(m_Itotv_rel, itElement, ki));
			}
			else if (itElement < nFixed + nShousuo) {
				// L22: �ǵ���������-�ǵ���������
				L22.push_back(Tri(m_Itotv_rel, itElement - nFixed, ki));
			}
			else if (itElement < nFixed + nShousuo + nOptimis) {
				// L32: ���Ż�-�ǵ���������
				L32.push_back(Tri(itElement - nFixed - nShousuo, m_Itotv_rel, ki));
			}
			else {
				// L42: ����-�ǵ���������
				L42.push_back(Tri(itElement - nFixed - nShousuo - nOptimis, m_Itotv_rel, ki));
			}
			nums++;
		}
		// �Խ���Ԫ���ڷǵ������������ɶ�����
		L22.push_back(Tri(m_Itotv_rel, m_Itotv_rel, ke[nDOF]));

		std::cout << "nums" << nums << std::endl;
	}
}

void InVar::Disp() const
{
	std::cout << m_id << "  " << m_Value << "  " << m_Force << "\n";
}

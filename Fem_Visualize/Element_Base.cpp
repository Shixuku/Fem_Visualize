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
			pNode->m_Disp.resize(nDOF_Node, 0);
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
	//�õ���Ԫ���ڵ�����ɶȱ��
	StructureFem* pStructure = Get_Structure();

	int nDOF = Get_DOF_Node();//��Ԫ����ȷ���Ľڵ����ɶȸ���
	auto nNode = m_idNode.size();//�ڵ����
	auto nDOF_Element = nDOF * nNode;//��Ԫ�����ɶȸ���
	DOFs.resize(nDOF_Element);

	int k = 0;
	for (auto& idNode : m_idNode)
	{//�Խڵ���ѭ��
		NodeFem* pNode = pStructure->Find_Node(idNode);//�ҽڵ�
		for (int j = 0; j < nDOF; ++j)
		{//�Խڵ����ɶ�ѭ��
			DOFs[k++] = pNode->m_DOF[j];
		}
	}
}

void Element_Base::Assemble_L2(ListTri& L11, ListTri& L21, ListTri& L22, ListTri& L31, ListTri& L32, ListTri& L33)
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
	int nOptimis = Get_Structure()->m_nOptimis - Get_Structure()->m_nFixed;
	int nNode = m_idNode.size();
	int nDOF_Node = Get_DOF_Node();
	int nDOF_Element = nNode * nDOF_Node;


	std::vector<int> EI;//��Ԫ��ȫ���ڵ���������ɶȱ��
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

	for (int i = 0; i < nDOF_Element; i++) { //��ѭ��
		int ii = EI[i]; //�����ɶȵ��������ɶȱ��
		for (int j = 0; j < nDOF_Element; ++j) { //��ѭ��
			int jj = EI[j]; //�����ɶȵ��������ɶȱ��
			double kij = ke(i, j);

			// L11: �̶���-�̶���
			if (ii < nFixed && jj < nFixed) {
				L11.push_back(Tri(ii, jj, kij));
			}
			// L21: ���Ż���-�̶���
			else if (ii >= nFixed && ii < (nFixed + nOptimis) && jj < nFixed) {
				L21.push_back(Tri(ii - nFixed, jj, kij));
			}
			// L31: ���ɵ�-�̶���
			else if (ii >= (nFixed + nOptimis) && jj < nFixed) {
				L31.push_back(Tri(ii - (nFixed + nOptimis), jj, kij));
			}
			// L22: ���Ż���-���Ż���
			else if (ii >= nFixed && ii < (nFixed + nOptimis) && jj >= nFixed && jj < (nFixed + nOptimis)) {
				L22.push_back(Tri(ii - nFixed, jj - nFixed, kij));
			}
			// L32: ���ɵ�-���Ż���
			else if (ii >= (nFixed + nOptimis) && jj >= nFixed && jj < (nFixed + nOptimis)) {
				L32.push_back(Tri(ii - (nFixed + nOptimis), jj - nFixed, kij));
			}
			// L33: ���ɵ�-���ɵ�
			else if (ii >= (nFixed + nOptimis) && jj >= (nFixed + nOptimis)) {
				L33.push_back(Tri(ii - (nFixed + nOptimis), jj - (nFixed + nOptimis), kij));
			}
		}
	}
}

void Element_Base::Assemble_L3(ListTri& L11, ListTri& L21, ListTri& L22, ListTri& L31, ListTri& L32, ListTri& L33, ListTri& L41, ListTri& L42, ListTri& L43, ListTri& L44)
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

	int nFixed = Get_Structure()->m_nFixed; // �̶����ɶ���
	int nShousuo = Get_Structure()->m_nShousuo - nFixed; // �ǵ������������ɶ���
	int nOptimis = Get_Structure()->m_nOptimis - Get_Structure()->m_nShousuo; // ���Ż����ɶ���
	int nFree = Get_Structure()->m_nTotv - Get_Structure()->m_nOptimis; // �������ɶ���
	int nNode = m_idNode.size();
	int nDOF_Node = Get_DOF_Node();
	int nDOF_Element = nNode * nDOF_Node;


	std::vector<int> EI; //��Ԫ��ȫ���ڵ���������ɶȱ��
	EI.resize(nDOF_Element);
	int k = 0;
	for (int i = 0; i < m_idNode.size(); i++)
	{
		NodeFem* pNode = Get_Structure()->Find_Node(m_idNode[i]);
		for (int j = 0; j < Get_DOF_Node(); j++)
		{
			EI[k++] = pNode->m_DOF[j];
		}
	}

	// ��װ����
	for (int i = 0; i < nDOF_Element; i++) {
		int ii = EI[i]; // �����ɶȵ��������ɶȱ��
		for (int j = 0; j < nDOF_Element; j++) {
			int jj = EI[j]; // �����ɶȵ��������ɶȱ��
			double kij = ke(i, j);

			// L11: �̶���-�̶���
			if (ii < nFixed && jj < nFixed) {
				L11.push_back(Tri(ii, jj, kij));
				continue;
			}
			// L21: �ǵ���������-�̶���
			if (ii >= nFixed && ii < nFixed + nShousuo && jj < nFixed) {
				L21.push_back(Tri(ii - nFixed, jj, kij));
				continue;
			}
			// L31: ���Ż���-�̶���
			if (ii >= nFixed + nShousuo && ii < nFixed + nShousuo + nOptimis && jj < nFixed) {
				L31.push_back(Tri(ii - nFixed - nShousuo, jj, kij));
				continue;
			}
			// L41: ���ɵ�-�̶���
			if (ii >= nFixed + nShousuo + nOptimis && jj < nFixed) {
				L41.push_back(Tri(ii - nFixed - nShousuo - nOptimis, jj, kij));
				continue;
			}
			// L22: �ǵ���������-�ǵ���������
			if (ii >= nFixed && ii < nFixed + nShousuo && jj >= nFixed && jj < nFixed + nShousuo) {
				L22.push_back(Tri(ii - nFixed, jj - nFixed, kij));
				continue;
			}
			// L32: ���Ż���-�ǵ���������
			if (ii >= nFixed + nShousuo && ii < nFixed + nShousuo + nOptimis && jj >= nFixed && jj < nFixed + nShousuo) {
				L32.push_back(Tri(ii - nFixed - nShousuo, jj - nFixed, kij));
				continue;
			}
			// L42: ���ɵ�-�ǵ���������
			if (ii >= nFixed + nShousuo + nOptimis && jj >= nFixed && jj < nFixed + nShousuo) {
				L42.push_back(Tri(ii - nFixed - nShousuo - nOptimis, jj - nFixed, kij));
				continue;
			}
			// L33: ���Ż���-���Ż���
			if (ii >= nFixed + nShousuo && ii < nFixed + nShousuo + nOptimis && jj >= nFixed + nShousuo && jj < nFixed + nShousuo + nOptimis) {
				L33.push_back(Tri(ii - nFixed - nShousuo, jj - nFixed - nShousuo, kij));
				continue;
			}
			// L43: ���ɵ�-���Ż���
			if (ii >= nFixed + nShousuo + nOptimis && jj >= nFixed + nShousuo && jj < nFixed + nShousuo + nOptimis) {
				L43.push_back(Tri(ii - nFixed - nShousuo - nOptimis, jj - nFixed - nShousuo, kij));
				continue;
			}
			// L44: ���ɵ�-���ɵ�
			if (ii >= nFixed + nShousuo + nOptimis && jj >= nFixed + nShousuo + nOptimis) {
				L44.push_back(Tri(ii - nFixed - nShousuo - nOptimis, jj - nFixed - nShousuo - nOptimis, kij));
				continue;
			}
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
	int nOptimis = Get_Structure()->m_nOptimis - Get_Structure()->m_nFixed;
	int nNode = m_idNode.size();
	int nDOF_Node = Get_DOF_Node();
	int nDOF_Element = nNode * nDOF_Node;


	std::vector<int> EI;//��Ԫ��ȫ���ڵ���������ɶȱ��
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
	{//��ѭ��
		int ii = EI[i];//�����ɶȵ��������ɶȱ��
		for (int j = 0; j < nDOF_Element; ++j)
		{//����ѭ��
			int jj = EI[j];//�����ɶȵ��������ɶȱ��
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


	std::vector<int> EI;//��Ԫ��ȫ���ڵ���������ɶȱ��
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
	{//��ѭ��
		int ii = EI[i];//�����ɶȵ��������ɶȱ��
		for (int j = 0; j < nDOF_Element; ++j)
		{//����ѭ��
			int jj = EI[j];//�����ɶȵ��������ɶȱ��
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

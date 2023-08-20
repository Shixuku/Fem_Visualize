#include "StructureFem.h"
#include "NodeFem.h"
#include "Element_Truss3D.h"
#include "Element_Beam3D.h"
#include "Boundary.h"
#include "ForceNode.h"
#include "Material.h"
#include "Section_Truss.h"
#include "Section_Beam3D.h"
#include "Dependant.h"

#include <fstream>
#include <iostream>

NodeFem* StructureFem::Find_Node(int idNode)
{//找节点
	auto iFind = m_Nodes.find(idNode);
	if (iFind != m_Nodes.end())
	{//找到了
		return iFind->second;
	}
	std::cout << "Node not found!\n";
	exit(1);
	return nullptr;
}

Material* StructureFem::Find_Material(int idMaterial)
{//找材料
	auto iFind = m_Material.find(idMaterial);
	if (iFind != m_Material.end())
	{//找到了
		return iFind->second;
	}
	std::cout << "Material not found!\n";
	exit(1);
	return nullptr;
}

Section_Base* StructureFem::Find_Section(int idSection)
{//找截面
	auto iFind = m_Section.find(idSection);
	if (iFind != m_Section.end())
	{//找到了
		return iFind->second;
	}
	std::cout << "Section not found!\n";
	exit(1);
	return nullptr;
}

Element_Base* StructureFem::Find_Element(int idElement)
{//找单元
	auto iFind = m_Element.find(idElement);
	if (iFind != m_Element.end())
	{//找到了
		return iFind->second;
	}
	std::cout << "Element not found!\n";
	exit(1);
	return nullptr;
}

void StructureFem::Input_data(const char* filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		std::cout << "file open failed\n";
		exit(1);
	}
	else
	{
		std::cout << "read start\n";
	}
	std::cout << "\n";

	//读节点
	int nNode = 0;
	fin >> nNode;
	std::cout << "Node:\n";
	for (int i = 0; i < nNode; ++i)
	{
		NodeFem* pNode = new NodeFem;
		fin >> pNode->m_id >> pNode->m_x >> pNode->m_y>>pNode->m_z;
		std::cout << pNode->m_id << " " << pNode->m_x << " " << pNode->m_y << " " << pNode->m_z << "\n";
		m_Nodes.insert({ pNode->m_id , pNode });
	}
	std::cout << "\n";

	//读三维桁架单元
	int nTruss3D = 0;
	fin >> nTruss3D;
	if (nTruss3D > 0)
	{
		std::cout << "Element_Truss3D:\n";
	}
	for (int i = 0; i < nTruss3D; ++i)
	{
		Element_Truss3D* pTruss = new Element_Truss3D;
		pTruss->Input_Data(fin);
		pTruss->Disp();
		m_Element.insert({ pTruss->m_id ,pTruss });
	}
	std::cout << "\n";

	//读三维梁单元
	int nBeam3D = 0;
	fin >> nBeam3D;
	if (nBeam3D > 0)
	{
		std::cout << "Element_Beam3D:\n";
	}
	for (int i = 0; i < nBeam3D; i++)
	{
		Element_Beam3D* pElement = new Element_Beam3D;
		pElement->Input_Data(fin);
		pElement->Disp();
		m_Element.insert({ pElement->m_id ,pElement });
	}

	//读主从关系信息
	int nDependant = 0;
	fin >> nDependant;
	if (nDependant>0)
	{
		std::cout << "Dependant:\n";
	}
	for (int i = 0; i < nDependant; ++i)
	{
		Dependant* pDependant = new Dependant;
		fin >> pDependant->m_id;
		fin >> pDependant->m_idNode;
		fin >> pDependant->m_idHost[0];
		fin >> pDependant->m_idHost[1];
		fin >> pDependant->m_idHost[2];
		fin >> pDependant->m_idHost[3];
		fin >> pDependant->m_idHost[4];
		fin >> pDependant->m_idHost[5];
		m_Dependant.insert({ pDependant->m_id ,pDependant });
	}
	std::cout << "\n";

	//读材料
	int nMaterial = 0;
	fin >> nMaterial;
	std::cout << "Material:\n";
	for (int i = 0; i < nMaterial; i++)
	{
		Material* pMaterial = new Material;
		fin >> pMaterial->m_id >> pMaterial->m_E >> pMaterial->m_v;
		pMaterial->Disp();
		m_Material.insert({ pMaterial->m_id,pMaterial });
	}
	std::cout << "\n";

	//读桁架截面
	int nSection_Truss = 0;
	fin >> nSection_Truss;
	if (nSection_Truss>0)
	{
		std::cout << "Section_Truss:\n";
	}
	for (int i = 0; i < nSection_Truss; ++i)
	{
		Section_Truss* pSection = new Section_Truss;
		fin >> pSection->m_id >> pSection->m_idMaterial >> pSection->m_Area;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";

	//读梁截面
	int nSectio_Beam3D = 0;
	fin >> nSectio_Beam3D;
	if (nSectio_Beam3D > 0)
	{
		std::cout << "Sectio_Beam3D:\n";
	}
	for (int i = 0; i < nSectio_Beam3D; ++i)
	{
		Section_Beam3D* pSection = new Section_Beam3D;
		fin >> pSection->m_id >> pSection->m_idMaterial;
		fin >> pSection->m_Area >> pSection->m_Iz >> pSection->m_Iy;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";

	//读指派单元截面信息
	int nElement = m_Element.size();
	for (int i = 0; i < nElement; ++i)
	{
		int idElement, idSection;
		fin >> idElement >> idSection;
		Element_Base* pElement = Find_Element(idElement);
		pElement->m_idSection = idSection;
	}
	std::cout << "Assign_Sections:\n";
	for (auto& a : m_Element)
	{
		a.second->Disp();
	}
	std::cout << "\n";

	//读边界条件
	int nBoundary = 0;
	fin >> nBoundary;
	std::cout << "Boundary:\n";
	for (int i = 0; i < nBoundary; ++i)
	{
		Boundary* pBoundary = new Boundary;
		fin >> pBoundary->m_id >> pBoundary->m_idNode;
		fin >> pBoundary->m_ixyz >> pBoundary->m_value;
		pBoundary->Disp();
		m_Boundary.insert({ pBoundary->m_id ,pBoundary });
		NodeFem* pNode = Find_Node(pBoundary->m_idNode);
		pNode->boundaryFlag = true;
	}
	std::cout << "\n";

	//读外力
	int nForce = 0;
	fin >> nForce;
	std::cout << "Force:\n";
	for (int i = 0; i < nForce; ++i)
	{
		ForceNode* pForce = new ForceNode;
		fin >> pForce->m_id >> pForce->m_idNode;
		fin >> pForce->m_ixyz >> pForce->m_value;
		pForce->Disp();
		m_ForceNode.insert({ pForce->m_id ,pForce });
	}
	std::cout << "\n";

	fin.close();

	std::cout << "read completed\n";
}

void StructureFem::Init_DOFs()
{//分配节点自由度
	for (auto& a : m_Element)
	{//设置每个节点的自由度个数
		Element_Base* pElement = a.second;
		pElement->Set_NodeDOF();
	}
	for (auto& a : m_Nodes)
	{//对节点循环
		NodeFem* pNode = a.second;
		for (auto& dof : pNode->m_DOF)
		{//对每个自由度循环
			dof = -1; //初始化每个节点的自由度编号均为-1
		}
	}

	auto iStart = 0;
	for (auto& a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		NodeFem* pNode = Find_Node(pBoundary->m_idNode);
		int it = pBoundary->m_ixyz;    //约束的自由度的序号，0：x，1：y
		pNode->m_DOF[it] = iStart;
		++iStart;
	}

	m_nFixed = iStart;//约束自由度个数
	std::cout << "\nm_nFixed=" << m_nFixed << "\n";

	for (auto& a : m_Dependant)
	{//处理主从信息
		Dependant* pDependant = a.second;
		pDependant->Set_DOF(iStart);
	}

	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		for (auto& dof : pNode->m_DOF)
		{
			if (dof == -1) dof = iStart++;
		}
	}
	m_nTotal = iStart;//总自由度个数

	std::cout << "\nnode DOF:\n";
	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		std::cout << pNode->m_id << " ";
		for (auto& dof : pNode->m_DOF)
		{
			std::cout << dof << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

}

void StructureFem::Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22)
{
	int nFree = m_nTotal - m_nFixed;

	K11.resize(m_nFixed, m_nFixed);
	K21.resize(nFree, m_nFixed);
	K22.resize(nFree, nFree);

	std::list<Tri> list_K11, list_K21, list_K22;

	for (auto& a : m_Element)
	{
		Element_Base* pElement = a.second;
		pElement->calculate_T();
		pElement->calculate_ke();
		pElement->calculate_me();
		pElement->calculate_Ke();
		pElement->calculate_Me();
		pElement->Assemble_ke(list_K11, list_K21, list_K22);	
	}

	/*std::cout << "\nlist_K21:\n";
	for (auto& a : list_K21)
	{
		std::cout << a.row() << "," << a.col() << "," << a.value() << "\n";
	}*/

	K11.setFromTriplets(list_K11.begin(), list_K11.end());
	K21.setFromTriplets(list_K21.begin(), list_K21.end());
	K22.setFromTriplets(list_K22.begin(), list_K22.end());
	
	/*MatrixXd M11 = K11; MatrixXd M21 = K21; MatrixXd M22 = K22;

	std::cout << "M11：" << "\n" << M11 << "\n";
	std::cout << "M21：" << "\n" << M21 << "\n";
	std::cout << "M22：" << "\n" << M22 << "\n";*/
}

void StructureFem::Equivalent_Force()
{
	for (auto a : m_Element)
	{
		Element_Base* pElement = a.second;
		Section_Base* pSection = Find_Section(pElement->m_idSection);
		Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);

		NodeFem* startNode = Find_Node(pElement->m_idNode[0]);
		NodeFem* endNode = Find_Node(pElement->m_idNode[1]);
		double dx = startNode->m_x - endNode->m_x;
		double dy = startNode->m_y - endNode->m_y;
		double dz = startNode->m_z - endNode->m_z;
		double L = std::sqrt(dx * dx + dy * dy + dz * dz);

		double q = -9.8 * 7800 * pSectionBeam->m_Area;

		VectorXd qVector(3);
		qVector.setZero();
		qVector(1) = q;

		qVector = pElement->m_Lambda * qVector;
		cout << "qVector" << "\n";
		cout << qVector << "\n";


		VectorXd x1(6);
		VectorXd x2(6);
		x1.setZero();
		x2.setZero();

		x1(1) = 1.0 / 2.0 * qVector(1) * L;
		x1(5) = 1.0 / 12.0 * qVector(1) * L * L;
		x2(1) = x1(1);
		x2(5) = -x1(5);

		x1(0) = 0.5 * qVector(0) * L;
		x2(0) = 0.5 * qVector(0) * L;

		x1(2) = 1.0 / 2.0 * qVector(2) * L;
		x1(4) = -1.0 / 12.0 * qVector(2) * L * L;
		x2(2) = x1(2);
		x2(4) = -x1(4);

		VectorXd equialentForce(12);
		equialentForce << x1, x2;  // 组合等效外力矩阵

		std::cout << "\n\n" << std::endl;
		cout << equialentForce << std::endl;

		a.second->m_Force = equialentForce;

		equialentForce = pElement->m_T.transpose() * equialentForce;

		for (int i = 0; i < 6; i++)
		{
			if (equialentForce[i] != 0)
			{
				ForceNode* pForceNode = new ForceNode();
				pForceNode->m_idNode = startNode->m_id;
				pForceNode->m_ixyz = i;
				pForceNode->m_id = m_ForceNode.size() + 1;
				pForceNode->m_value = equialentForce[i];
				m_ForceNode.insert({ pForceNode->m_id, pForceNode });
			}
			if (equialentForce[i + 6] != 0)
			{
				ForceNode* pForceNode = new ForceNode();
				pForceNode->m_idNode = endNode->m_id;
				pForceNode->m_ixyz = i;
				pForceNode->m_id = m_ForceNode.size() + 1;
				pForceNode->m_value = equialentForce[i + 6];
				m_ForceNode.insert({ pForceNode->m_id, pForceNode });
			}
		}
	}
}

void StructureFem::Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2)
{
	x1.resize(m_nFixed);
	// 将固定约束存入向量组中
	for (auto& a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		pBoundary->Set_x1(x1);
	}

	F1.resize(m_nFixed);
	int nFree = m_nTotal - m_nFixed;
	F2.resize(nFree);
	F1.setZero();
	F2.setZero();
	for (auto& a : m_ForceNode)
	{
		ForceNode* pForce = a.second;
		pForce->Assemble_F1_F2(F1, F2);
	}
	
}

void StructureFem::Analyse()
{
	Init_DOFs();//分配节点自由度

	SpMat K11, K21, K22;
	Assemble_K(K11, K21, K22);//组装总纲矩阵
	MatrixXd M11 = K11; MatrixXd M21 = K21; MatrixXd M22 = K22;

	//std::cout << "M11(K11)：" << "\n" << M11 << "\n";
	//std::cout << "M21(K21)：" << "\n" << M21 << "\n";
	//std::cout << "M22(K22)：" << "\n" << M22 << "\n";

	Equivalent_Force(); // 计算等效外力荷载

	VectorXd x1, F1, F2;
	Assemble_xF(x1, F1, F2);

	std::cout << "\nF1=\n" << F1 << "\n";
	std::cout << "\nF2=\n" << F2 << "\n";
	
	
	SimplicialLDLT<SparseMatrix<double>> solver;
	solver.analyzePattern(K22); //分析非零元素结构
	solver.factorize(K22);      //分解
	
	VectorXd x2 = solver.solve(F2 - K21 * x1);
	VectorXd R1 = K11 * x1 + K21.transpose() * x2 - F1;


	VectorXd R2 = K11 * x1 + K21.transpose() * x2;

	// 将支座反力赋值给对应的点约束处
	for (auto &a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		NodeFem* NodeFem = Find_Node(pBoundary->m_idNode);
		NodeFem->m_ReactionForce[pBoundary->m_ixyz] = R1(pBoundary->m_id - 1);
	}

	for (auto& a : m_Nodes)
	{
		if (a.second->boundaryFlag)
		{
			m_ReationForce.push_back(a.second);
		}
	}

	m_x1 = x1;
	m_x2 = x2;

	std::cout << "\nx1=\n" << m_x1 << "\n";
	std::cout << "\nx2=\n" << m_x2 << "\n";
	std::cout << "\nR1=\n" << R1 << "\n"<<"\n";


	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		double dis = 0;
		int i = 0;
		std::cout << "\n";
		std::cout << "node" << pNode->m_id  << ":\n";
		for (auto& dof : pNode->m_DOF)
		{
			if (dof < m_nFixed)
			{
				dis = x1[dof];
				std::cout << x1[dof] << " ";
			}
			else
			{
				dis = x2[dof - m_nFixed];
				std::cout << x2[dof - m_nFixed] << " ";
			}
			pNode->m_Displacement[i] = dis;
			i++;
			std::cout << "\n";
		}
	}

	for (auto& a : m_Element)
	{
		NodeFem* startNode = Find_Node(a.second->m_idNode[0]);
		NodeFem* endNode = Find_Node(a.second->m_idNode[1]);

		Eigen::VectorXd combinedDisp(12);
		// 单元两个节点的位移
		combinedDisp << startNode->m_Displacement, endNode->m_Displacement;
		// 通过节点位移计算单元力
		Eigen::VectorXd Force(12);
		// 将整体坐标系下的位移转换到单元坐标下
		combinedDisp = a.second->m_T * combinedDisp;

		// ke * u - Feq 为梁单元两端内力
		Force = a.second->m_ke * combinedDisp - a.second->m_Force;
	}
}

StructureFem::~StructureFem()
{
	std::cout << "\nClean memory\n";
	for (auto& a : m_Nodes)
	{
		delete a.second;
	}
	for (auto& a : m_Element)
	{
		delete a.second;
	}
	for (auto& a : m_Boundary)
	{
		delete a.second;
	}
	for (auto& a : m_ForceNode)
	{
		delete a.second;
	}

}

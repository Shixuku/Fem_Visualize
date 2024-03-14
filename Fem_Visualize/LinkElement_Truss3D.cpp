#include "LinkElement_Truss3D.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include "ForceNode.h"
#include "Section_Truss3D.h"
#include "Material.h"
#include "inVar.h"

#include <iostream>
using namespace std;

LinkElement_Truss3D::LinkElement_Truss3D()
{
	m_idNode.resize(2);
	m_type = "Truss";
}

LinkElement_Truss3D::LinkElement_Truss3D(int id, int idNode1, int idNode2)
{
	*this = LinkElement_Truss3D();
	m_idNode[0] = idNode1;
	m_idNode[1] = idNode2;
	m_id = id;
}

void LinkElement_Truss3D::calculate_ke()
{
	Eigen::MatrixXd ke;
	NodeFem* pNode0 = Get_Structure()->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = Get_Structure()->Find_Node(m_idNode[1]);
	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	double L = sqrt(dx * dx + dy * dy + dz * dz);
	double lx = dx / L;
	double ly = dy / L;
	double lz = dz / L;

	Section_Base* pSection = Get_Structure()->Find_Section(m_idSection);
	double E = pSection->Get_E();
	Section_Truss3D* pSectionTruss = dynamic_cast<Section_Truss3D*>(pSection);//�������ָ�붯̬ת��Ϊ3D��ܽ���ָ��
	assert(pSectionTruss != nullptr);         //���ָ���Ƿ�Ϊ��
	double A = pSectionTruss->m_Area;
	double a = E * A / L;

	ke.resize(2, 2);
	ke << a, -a, -a, a;
	m_ke = ke;
}

void LinkElement_Truss3D::calculate_me()
{

}

void LinkElement_Truss3D::calculate_T()
{
	StructureFem* pSt = Get_Structure();
	MatrixXd lambda(3, 3);
	m_T.resize(2, 6);

	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Truss3D* pSectionBeam = dynamic_cast<Section_Truss3D*>(pSection);
	assert(pSectionBeam != nullptr);

	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);

	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	double L = sqrt(dx * dx + dy * dy + dz * dz);

	double lx = (dx) / L;
	double mx = (dy) / L;
	double nx = (dz) / L;

	if (dx == 0 && dy == 0)
	{
		if (dz > 0)
		{
			lambda << 0., 0., 1.,
				0., 1., 0.,
				-1., 0.0, 0.0;
		}
		else
		{
			lambda << -1., 0., 0.,
				0., 1., 0.,
				0., 0., 1;
		}
	}
	else
	{
		double lx = (dx) / L;
		double mx = (dy) / L;
		double nx = (dz) / L;

		double d = sqrt(lx * lx + mx * mx);

		double ly = -mx / d;
		double my = lx / d;
		double ny = 0.;

		double lz = -lx * nx / d;
		double mz = -mx * nx / d;
		double nz = d;

		lambda << lx, mx, nx,
			ly, my, ny,
			lz, mz, nz;
	}

	m_Lambda = lambda;
	m_T << lx, mx, nx, 0, 0, 0,
		0, 0, 0, lx, mx, nx;

	Matrix<double, 6, 6> T = Matrix<double, 6, 6>::Zero();
	T.block<3, 3>(0, 0) = lambda;
	T.block<3, 3>(3, 3) = lambda;
	m_R = T;
}

void LinkElement_Truss3D::calculate_internal_force(Eigen::VectorXd disp)
{
	//cout << "\n";
	//cout << disp << std::endl;

	//cout << "\n";

	//cout << m_R << std::endl;

	//disp = m_R * disp;

	//cout << "\n";

	//cout << disp << std::endl;

	//m_force = m_ke * disp - m_EqForce;
}

void LinkElement_Truss3D::Equivalent_Force()
{
	StructureFem* pSt = Get_Structure();
	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Truss3D* pSectionBeam = dynamic_cast<Section_Truss3D*>(pSection);

	NodeFem* startNode = pSt->Find_Node(m_idNode[0]);
	NodeFem* endNode = pSt->Find_Node(m_idNode[1]);
	double dx = startNode->m_x - endNode->m_x;
	double dy = startNode->m_y - endNode->m_y;
	double dz = startNode->m_z - endNode->m_z;
	double L = std::sqrt(dx * dx + dy * dy + dz * dz);

	double q = -9.8 * 7800 * pSectionBeam->m_Area;
	double halfQ = 0.5 * q * L; // ������ؾ��ȷֲ�����ÿ���ڵ�е�һ����ܺ���

	//VectorXd qVector(3);
	//qVector.setZero();
	//qVector(1) = q;

	//qVector = m_Lambda * qVector;

	VectorXd x1(1);
	//VectorXd x2(3);
	//x1.setZero();
	//x2.setZero();

	ForceNode* pForceNode = new ForceNode();
	pForceNode->m_idNode = startNode->m_id;
	pForceNode->m_id = pSt->m_ForceNode.size() + 1;
	pForceNode->m_value = halfQ;
	pForceNode->m_ixyz = 1;
	pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });

	ForceNode* pForceNode2 = new ForceNode();
	pForceNode2->m_idNode = endNode->m_id;
	pForceNode2->m_id = pSt->m_ForceNode.size() + 1;
	pForceNode2->m_value = halfQ;
	pForceNode2->m_ixyz = 1;
	pSt->m_ForceNode.insert({ pForceNode2->m_id, pForceNode2 });

	//VectorXd equialentForce(2);
	//equialentForce << x1, x2;  // ��ϵ�Ч��������

	//m_EqForce = equialentForce;

	//equialentForce = m_R.transpose() * equialentForce;
	//cout << "��Ч����" << equialentForce << "\n";


	//for (int i = 0; i < 3; i++)
	//{
	//	if (equialentForce[i] != 0)
	//	{
	//		ForceNode* pForceNode = new ForceNode();
	//		pForceNode->m_idNode = startNode->m_id;
	//		pForceNode->m_ixyz = i;
	//		pForceNode->m_id = pSt->m_ForceNode.size() + 1;
	//		pForceNode->m_value = equialentForce[i];
	//		pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });
	//	}
	//	if (equialentForce[i + 3] != 0)
	//	{
	//		ForceNode* pForceNode = new ForceNode();
	//		pForceNode->m_idNode = endNode->m_id;
	//		pForceNode->m_ixyz = i;
	//		pForceNode->m_id = pSt->m_ForceNode.size() + 1;
	//		pForceNode->m_value = equialentForce[i + 3];
	//		pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });
	//	}
	//}
}

void LinkElement_Truss3D::calculate_all()
{
	//calculate_T();

	StructureFem* pSt = Get_Structure();

	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Truss3D* pSectionTruss = dynamic_cast<Section_Truss3D*>(pSection);
	assert(pSectionTruss != nullptr);

	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);

	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	double L = sqrt(dx * dx + dy * dy + dz * dz);

	double lx = (dx) / L;
	double mx = (dy) / L;
	double nx = (dz) / L;

	double E = pSection->Get_E();
	double A = pSectionTruss->m_Area;
	double a = E * A / L;

	//double b[] = { -lx,-mx,-nx,lx,mx,nx };
	//m_ke.resize(6, 6);
	//for (int i = 0; i < 6; i++)
	//{
	//	for (int j = 0; j < 6; j++)
	//	{
	//		m_ke(i, j) = a * b[i] * b[j];
	//	}
	//}
	calculate_ke();
	calculate_T();
	m_ke = m_T.transpose() * m_ke * m_T;

	//std::cout << m_ke << std::endl;
    //calculate_ke();
	//Get_ke();
	//m_ke = m_T.transpose() * m_ke * m_T;
	Equivalent_Force();
}

double LinkElement_Truss3D::Get_Lcs(double& c, double& s) const
{
	StructureFem* pStructure = Get_Structure();
	NodeFem* pNode0 = pStructure->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pStructure->Find_Node(m_idNode[1]);
	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double L = sqrt(dx * dx + dy * dy);
	c = dx / L;
	s = dy / L;
	return L;
}



double LinkElement_Truss3D::Get_Lcst(double& c, double& s, double& t) const
{
	StructureFem* pStructure = Get_Structure();
	NodeFem* pNode0 = pStructure->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pStructure->Find_Node(m_idNode[1]);
	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;
	double L = sqrt(dx * dx + dy * dy + dz * dz);
	c = dx / L;
	s = dy / L;
	t = dz / L;
	return L;
}

void LinkElement_Truss3D::Get_EA(double& E, double& A) const
{
	StructureFem* pStructure = Get_Structure();
	Section_Base* pSection = pStructure->Find_Section(m_idSection);
	Section_Truss3D* pSectionTruss = dynamic_cast<Section_Truss3D*>(pSection);
	A = pSectionTruss->m_Area;
	Material* pMaterial = pStructure->Find_Material(pSection->m_idMaterial);
	E = pMaterial->m_E;
}

void LinkElement_Truss3D::Get_N(double& N, double& Stress)
{
	double L, c, s, E, A;
	L = Get_Lcs(c, s);
	Get_EA(E, A);

	StructureFem* pStructure = Get_Structure();

	NodeFem* pNode0 = pStructure->Find_Node(m_idNode[0]);
	double ui = pStructure->Get_Sol(pNode0->m_DOF[0]);
	double vi = pStructure->Get_Sol(pNode0->m_DOF[1]);

	NodeFem* pNode1 = pStructure->Find_Node(m_idNode[1]);
	double uj = pStructure->Get_Sol(pNode1->m_DOF[0]);
	double vj = pStructure->Get_Sol(pNode1->m_DOF[1]);

	double dL = (uj - ui) * c + (vj - vi) * s;//�쳤��
	for (auto& a : pStructure->m_InVar)
	{
		InVar* pInVar = a.second;
		if (pInVar->m_idElement == m_id)
		{//�Ǳ���Ԫ���ڱ���
			dL += pInVar->m_Value;//����Ӧ��������쳤��
		}
	}
	double Strain = dL / L;//����Ӧ��

	Stress = E * Strain;
	N = Stress * A;
}

void LinkElement_Truss3D::Get_ke()
{
	double L, c, s, E, A;
	L = Get_Lcs(c, s);
	Get_EA(E, A);
	double a = E * A / L;

	double b[] = { -c,-s,c,s };
	m_ke.resize(4, 4);
	for (int i = 0; i < 4; ++i)
	{//��ѭ��
		for (int j = 0; j < 4; ++j)
		{//��ѭ��
			m_ke(i, j) = a * b[i] * b[j];
		}
	}
}

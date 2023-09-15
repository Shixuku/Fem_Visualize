#include "LinkElement_Truss3D.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include "ForceNode.h"
#include "Section_Truss3D.h"

#include <iostream>
using namespace std;

LinkElement_Truss3D::LinkElement_Truss3D()
{
	m_idNode.resize(2);
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
	Section_Truss3D* pSectionTruss = dynamic_cast<Section_Truss3D*>(pSection);//截面基类指针动态转换为3D桁架截面指针
	assert(pSectionTruss != nullptr);         //检查指针是否为空
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

	double q = -9.8 * pSection->m_density * pSectionBeam->m_Area;

	VectorXd qVector(3);
	qVector.setZero();
	qVector(1) = q;

	qVector = m_Lambda * qVector;

	VectorXd x1(3);
	VectorXd x2(3);
	x1.setZero();
	x2.setZero();


	x1(0) = 0.5 * qVector(0) * L;
	x2(0) = 0.5 * qVector(0) * L;

	x1(1) = 0.5 * qVector(1) * L;
	x2(1) = 0.5 * qVector(1) * L;

	x1(2) = 0.5 * qVector(2) * L;
	x2(2) = 0.5 * qVector(2) * L;
;

	VectorXd equialentForce(6);
	equialentForce << x1, x2;  // 组合等效外力矩阵

	m_EqForce = equialentForce;

	equialentForce = m_R.transpose() * equialentForce;


	for (int i = 0; i < 3; i++)
	{
		if (equialentForce[i] != 0)
		{
			ForceNode* pForceNode = new ForceNode();
			pForceNode->m_idNode = startNode->m_id;
			pForceNode->m_ixyz = i;
			pForceNode->m_id = pSt->m_ForceNode.size() + 1;
			pForceNode->m_value = equialentForce[i];
			pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });
		}
		if (equialentForce[i + 3] != 0)
		{
			ForceNode* pForceNode = new ForceNode();
			pForceNode->m_idNode = endNode->m_id;
			pForceNode->m_ixyz = i;
			pForceNode->m_id = pSt->m_ForceNode.size() + 1;
			pForceNode->m_value = equialentForce[i + 3];
			pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });
		}
	}
}

void LinkElement_Truss3D::calculate_all()
{
	calculate_T();
	calculate_ke();
	Equivalent_Force();
}

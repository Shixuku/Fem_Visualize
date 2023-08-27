#include "Element_Beam3D.h"
#include "Section_Beam3D.h"
#include "StructureFem.h"
#include "ForceNode.h"
#include "NodeFem.h"

Element_Beam3D::Element_Beam3D()
{
	m_idNode.resize(2);
	m_e1.resize(3);
	m_e2.resize(3);
	m_e3.resize(3);
}

Element_Beam3D::Element_Beam3D(int id, int idNode1, int idNode2)
{
	*this = Element_Beam3D();
	m_idNode[0] = idNode1;
	m_idNode[1] = idNode2;
	m_id = id;
}

void Element_Beam3D::Input_Data(std::ifstream& fin)
{
	fin >> m_id;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
	fin >> m_g(0);
	fin >> m_g(1);
	fin >> m_g(2);
}

void Element_Beam3D::Disp()
{//截面信息
	std::cout << m_id << " " << m_idSection << " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}
	std::cout <<"Element direction e3:" << " "<<m_g(0) << " " << m_g(1) << " " << m_g(2);

	std::cout << "\n";
}


void Element_Beam3D::calculate_ke()
{
	Eigen::MatrixXd ke(12, 12);
	StructureFem* pSt = Get_Structure();

	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);
	assert(pSectionBeam != nullptr);

	double E = pSectionBeam->Get_E();
	double G = pSectionBeam->Get_G();
	double A = pSectionBeam->m_Area;
	double Iz = pSectionBeam->m_Iz;
	double Iy = pSectionBeam->m_Iy;
	double J = pSectionBeam->Get_J();

	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);

	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	double L = sqrt(dx * dx + dy * dy + dz * dz);
	double L2 = L * L;
	double L3 = L2 * L;
	double EA = E * A;
	double EIy = E * Iy;
	double EIz = E * Iz;

	MatrixXd ke1(6, 6), ke2(6, 6), ke3(6, 6);
	ke1.setZero();
	ke2.setZero();

	ke1(0, 0) = EA / L;
	ke1(1, 1) = 12 * EIz / L3;
	ke1(2, 2) = 12 * EIy / L3;
	ke1(3, 3) = G * J / L;
	ke1(4, 2) = -6 * EIy / L2;
	ke1(4, 4) = 4 * EIy / L;
	ke1(5, 1) = 6 * EIz / L2;
	ke1(5, 5) = 4 * EIz / L;

	ke2(0, 0) = -ke1(0, 0);
	ke2(1, 1) = -ke1(1, 1);
	ke2(1, 5) = -6 * EIz / L2;
	ke2(2, 2) = -ke1(2, 2);
	ke2(2, 4) = -ke1(4, 2);
	ke2(3, 3) = -ke1(3, 3);
	ke2(4, 2) = ke1(4, 2);
	ke2(4, 4) = 0.5 * ke1(4, 4);
	ke2(5, 1) = ke1(5, 1);
	ke2(5, 5) = 0.5 * ke1(5, 5);

	ke3 = ke1;
	ke3(4, 2) = -ke1(4, 2);
	ke3(5, 1) = -ke1(5, 1);

	ke.resize(12, 12);
	ke.setZero();

	ke.block<6, 6>(0, 0) = ke1;
	ke.block<6, 6>(6, 0) = ke2;
	ke.block<6, 6>(6, 6) = ke3;


	for (int i = 0; i < 12; ++i)
	{//设置上三角部分
		for (int j = i + 1; j < 12; ++j)
		{
			ke(i, j) = ke(j, i);
		}
	}
	m_ke = ke;
}

void Element_Beam3D::calculate_me()
{
	Eigen::MatrixXd me(12, 12);
	StructureFem* pSt = Get_Structure();

	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);
	assert(pSectionBeam != nullptr);

	double E = pSectionBeam->Get_E();
	double G = pSectionBeam->Get_G();
	double A = pSectionBeam->m_Area;
	double Ix = 1;
	double density = pSectionBeam->m_density;


	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);
	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;
	double L = sqrt(dx * dx + dy * dy + dz * dz);

	double a = density * A * L / 210.0;
	double r = Ix / A;

	me << 70.0, 0.0, 0.0, 0.0, 0.0, 0.0, 35.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 78.0, 0.0, 0.0, 0.0, 11.0 * L, 0.0, 27.0, 0.0, 0.0, 0.0, -6.5 * L,
		0.0, 0.0, 78.0, 0.0, -11.0 * L, 0.0, 0.0, 0.0, 27.0, 0.0, 6.5 * L, 0.0,
		0.0, 0.0, 0.0, 70.0 * r, 0.0, 0.0, 0.0, 0.0, 0.0, -35.0 * r, 0.0, 0.0,
		0.0, 0.0, -11.0 * L, 0.0, 2.0 * L * L, 0.0, 0.0, 0.0, -6.5 * L, 0.0, -1.5 * L * L, 0.0,
		0.0, 11.0 * L, 0.0, 0.0, 0.0, 2.0 * L * L, 0.0, 6.5 * L, 0.0, 0.0, 0.0, -1.5 * L * L,
		35.0, 0.0, 0.0, 0.0, 0.0, 0.0, 70.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 27.0, 0.0, 0.0, 0.0, 6.5 * L, 0.0, 78.0, 0.0, 0.0, 0.0, -11.0 * L,
		0.0, 0.0, 27.0, 0.0, -6.5 * L, 0.0, 0.0, 0.0, 78.0, 0.0, 11.0 * L, 0.0,
		0.0, 0.0, 0.0, -35.0 * r, 0.0, 0.0, 0.0, 0.0, 0.0, 70.0 * r, 0.0, 0.0,
		0.0, 0.0, 6.5 * L, 0.0, -1.5 * L * L, 0.0, 0.0, 0.0, 11.0 * L, 0.0, 2.0 * L * L, 0.0,
		0.0, -6.5 * L, 0.0, 0.0, 0.0, -1.5 * L * L, 0.0, -11.0 * L, 0.0, 0.0, 0.0, 2.0 * L * L;
	m_me = me;
}

void Element_Beam3D::calculate_T()
{
	StructureFem* pSt = Get_Structure();
	MatrixXd lambda(3, 3);

	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);
	assert(pSectionBeam != nullptr);

	NodeFem* pNode0 = pSt->Find_Node(m_idNode[0]);
	NodeFem* pNode1 = pSt->Find_Node(m_idNode[1]);

	double dx = pNode1->m_x - pNode0->m_x;
	double dy = pNode1->m_y - pNode0->m_y;
	double dz = pNode1->m_z - pNode0->m_z;

	double L = sqrt(dx * dx + dy * dy + dz * dz);

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

	Matrix<double, 12, 12> T = Matrix<double, 12, 12>::Zero();
	T.block<3, 3>(0, 0) = lambda;
	T.block<3, 3>(3, 3) = lambda;
	T.block<3, 3>(6, 6) = lambda;
	T.block<3, 3>(9, 9) = lambda;

	m_Lambda = lambda;
	m_T = T;
}

void Element_Beam3D::Equivalent_Force()
{
	StructureFem* pSt = Get_Structure();
	Section_Base* pSection = pSt->Find_Section(m_idSection);
	Section_Beam3D* pSectionBeam = dynamic_cast<Section_Beam3D*>(pSection);

	NodeFem* startNode = pSt->Find_Node(m_idNode[0]);
	NodeFem* endNode = pSt->Find_Node(m_idNode[1]);
	double dx = startNode->m_x - endNode->m_x;
	double dy = startNode->m_y - endNode->m_y;
	double dz = startNode->m_z - endNode->m_z;
	double L = std::sqrt(dx * dx + dy * dy + dz * dz);

	double q = -9.8 * 7800 * pSectionBeam->m_Area;

	VectorXd qVector(3);
	qVector.setZero();
	qVector(1) = q;

	qVector = m_Lambda * qVector;

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

	m_force = equialentForce;

	equialentForce = m_T.transpose() * equialentForce;

	for (int i = 0; i < 6; i++)
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
		if (equialentForce[i + 6] != 0)
		{
			ForceNode* pForceNode = new ForceNode();
			pForceNode->m_idNode = endNode->m_id;
			pForceNode->m_ixyz = i;
			pForceNode->m_id = pSt->m_ForceNode.size() + 1;
			pForceNode->m_value = equialentForce[i + 6];
			pSt->m_ForceNode.insert({ pForceNode->m_id, pForceNode });
		}
	}
}

void Element_Beam3D::calculate_all()
{
	calculate_T();
	calculate_ke();
	calculate_me();
	Equivalent_Force();
}


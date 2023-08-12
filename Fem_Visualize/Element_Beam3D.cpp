#include "Element_Beam3D.h"
#include "Section_Beam3D.h"
#include "StructureFem.h"
#include "NodeFem.h"

Element_Beam3D::Element_Beam3D()
{
	m_idNode.resize(2);
	m_e1.resize(3);
	m_e2.resize(3);
	m_e3.resize(3);
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

void Element_Beam3D::Get_ke(MatrixXd& ke)
{
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

	double L = sqrt(dx * dx + dy * dy+dz*dz);
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

	MatrixXd T(3, 3);
	T << lx, mx, nx,
		ly, my, ny,
		lz, mz, nz;

	Matrix<double, 12, 12> R = Matrix<double, 12, 12>::Zero();
	R.block<3, 3>(0, 0) = T;
	R.block<3, 3>(3, 3) = T;
	R.block<3, 3>(6, 6) = T;
	R.block<3, 3>(9, 9) = T;

	m_T = R;
	m_Lamda = T;
	ke = R.transpose() * ke * R;
}



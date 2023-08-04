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
	//Vector3d e1, e2, e3;

	//e1(0) = dx / L;
	//e1(1) = dy / L;
	//e1(2) = dz / L;

	//e2 = m_g.cross(e1);
	//e2.normalize();
	//e3 = e1.cross(e2);
	//e3.normalize();

	//if (e1 == m_g)
	//{
	//	e2(1) = 1;
	//	e3(2) = 1;
	//}

	//2、坐标变换，得到整体坐标系的单元刚度矩阵
	Vector3d local;
	local << dx, dy, dz;

	Vector3d x0;
	x0 << 1, 0, 0;
	Vector3d y0;
	y0 << 0, 1, 0;
	Vector3d z0;
	z0 << 0, 0, 1;

	double theta = acos(local.dot(x0) / local.norm());

	Vector3d fai;
	fai = x0.cross(local);
	fai.normalize();

	Matrix3d fai_mat;
	fai_mat << 0, -fai(2), fai(1),
		fai(2), 0, -fai(0),
		-fai(1), fai(0), 0;

	Matrix3d I;
	I.setIdentity();

	Matrix3d T1;
	T1 = I + fai_mat * sin(theta) + fai_mat * fai_mat * (1 - cos(theta));

	Matrix3d M;
	M = T1.transpose();

	MatrixXd R(12, 12);
	R.setZero();

	R.block<3, 3>(0, 0) = M;
	R.block<3, 3>(3, 3) = M;
	R.block<3, 3>(6, 6) = M;
	R.block<3, 3>(9, 9) = M;

	m_T = R;
	ke = R.transpose() * ke * R;
}



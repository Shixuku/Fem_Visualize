#include "Element_Truss3D.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include "Section_Truss.h"

#include <iostream>
using namespace std;

Element_Truss3D::Element_Truss3D()
{
	m_idNode.resize(2);
}

Element_Truss3D::Element_Truss3D(int id, int idNode1, int idNode2)
{
	*this = Element_Truss3D();
	m_idNode[0] = idNode1;
	m_idNode[1] = idNode2;
	m_id = id;
}

void Element_Truss3D::calculate_ke()
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
	Section_Truss* pSectionTruss = dynamic_cast<Section_Truss*>(pSection);//截面基类指针动态转换为3D桁架截面指针
	assert(pSectionTruss != nullptr);         //检查指针是否为空
	double A = pSectionTruss->m_Area;
	double a = E * A / L;

	MatrixXd B(1, 6);
	B.setZero();

	B(0, 0) = -lx;
	B(0, 1) = -ly;
	B(0, 2) = -lz;
	B(0, 3) = lx;
	B(0, 4) = ly;
	B(0, 5) = lz;

	ke.resize(6, 6);
	ke.setZero();
	ke = B.transpose() * B * (E * A / L);
}

void Element_Truss3D::calculate_me()
{

}

void Element_Truss3D::calculate_T()
{

}

void Element_Truss3D::calculate_internal_force(Eigen::VectorXd disp)
{

}

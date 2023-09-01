#include "Section_Truss3D.h"

Section_Truss3D::Section_Truss3D(int id, int idMaterial, double Area)
{
	*this = Section_Truss3D();
	m_Area = Area;
	m_idMaterial = idMaterial;
	m_id = id;
}

Section_Truss3D::Section_Truss3D()
{

}

void Section_Truss3D::Disp()
{
	Section_Base::Disp();
	std::cout << m_Area << " ";
}

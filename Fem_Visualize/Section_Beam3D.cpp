#include "Section_Beam3D.h"

Section_Beam3D::Section_Beam3D(int idSection, int idMaterial, double area, double Iy, double Iz)
{
	Get_J();
	m_id = idSection;
	m_idMaterial = idMaterial;
	m_Area = area;
	m_Iy = Iy;
	m_Iz = Iz;
}

Section_Beam3D::Section_Beam3D()
{

}

double Section_Beam3D::Get_J()
{
	double J;
	J = m_Iy + m_Iz;
	return J;
}

void Section_Beam3D::Disp()
{
	Section_Truss3D::Disp();
	std::cout << m_Iz << " ";
	std::cout << m_Iy << "\n";
}

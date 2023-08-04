#include "Section_Beam3D.h"

double Section_Beam3D::Get_J()
{
	double J;
	J = m_Iy + m_Iz;
	return J;
}

void Section_Beam3D::Disp()
{
	Section_Truss::Disp();
	std::cout << m_Iz << " ";
	std::cout << m_Iy << "\n";
}

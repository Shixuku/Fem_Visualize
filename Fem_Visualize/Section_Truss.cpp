#include "Section_Truss.h"

Section_Truss::Section_Truss(int id, int idMaterial, double Area)
{
	*this = Section_Truss();
	m_Area = Area;
	m_idMaterial = idMaterial;
	m_id = id;
}

Section_Truss::Section_Truss()
{

}

void Section_Truss::Disp()
{
	Section_Base::Disp();
	std::cout << m_Area << " ";
}

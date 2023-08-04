#include "Section_Base.h"
#include "StructureFem.h"
#include "Material.h"
#include <iostream>

double Section_Base::Get_E()
{
	Material* pMaterial = Get_Structure()->Find_Material(m_idMaterial);
	assert(pMaterial != nullptr);
	return pMaterial->m_E;
}

double Section_Base::Get_G()
{
	Material* pMaterial = Get_Structure()->Find_Material(m_idMaterial);
	assert(pMaterial != nullptr);
	double G,E,v;
	E = pMaterial->m_E;
	v = pMaterial->m_v;
	G = E / (2 * (1 + v));
	return G;
}

void Section_Base::Get_Ev(double& E, double& v)
{
	Material* pMaterial = Get_Structure()->Find_Material(m_idMaterial);
	assert(pMaterial != nullptr);
	E = pMaterial->m_E;
	v = pMaterial->m_v;
}

void Section_Base::Disp()
{
    std::cout << m_id << " " << m_idMaterial << " ";
}

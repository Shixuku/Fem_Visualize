#include "InVar_Truss3D.h"

void InVar_Truss3D::Calculate_ke()
{
	//¼ÆËã¸Õ¶È¾ØÕóÔªËØ
	StructureFem* pStructure = Get_Structure();
	Element_Base* pElement = pStructure->Find_Element(m_idElement);
	LinkElement_Truss3D* pTruss = dynamic_cast<LinkElement_Truss3D*>(pElement);

	double L, c, s, t, E, A;
	L = pTruss->Get_Lcst(c, s, t);
	pTruss->Get_EA(E, A);
	double a = E * A / L;
	m_ke.resize(7);
	m_ke[0] = -c * a;
	m_ke[1] = -s * a;
	m_ke[2] = -t * a;
	m_ke[3] = c * a;
	m_ke[4] = s * a;
	m_ke[5] = t * a;
	m_ke[6] = a;
}

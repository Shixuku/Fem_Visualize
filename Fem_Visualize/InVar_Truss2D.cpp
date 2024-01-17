#include "InVar_Truss2D.h"
#include "StructureFem.h"
#include "LinkElement_Truss3D.h"

void InVar_Truss2D::Calculate_ke()
{
	//¼ÆËã¸Õ¶È¾ØÕóÔªËØ
	StructureFem* pStructure = Get_Structure();
	Element_Base* pElement = pStructure->Find_Element(m_idElement);
	LinkElement_Truss3D* pTruss = dynamic_cast<LinkElement_Truss3D*>(pElement);

	double L, c, s, E, A;
	L = pTruss->Get_Lcs(c, s);
	pTruss->Get_EA(E, A);
	double a = E * A / L;
	m_ke.resize(5);
	m_ke[0] = -c * a;
	m_ke[1] = -s * a;
	m_ke[2] = c * a;
	m_ke[3] = s * a;
	m_ke[4] = a;
}

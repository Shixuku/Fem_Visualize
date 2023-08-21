#include "SoildElement_Base.h"

void SoildElement_Base::calculate_Ke()
{
	m_Ke = m_t * m_volume * m_B.transpose() * m_D * m_B;
}

SoildElement_Base::SoildElement_Base()
{
	Get_DOF_Node();

	m_t = 0;
}

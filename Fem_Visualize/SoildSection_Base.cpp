#include "SoildSection_Base.h"

SoildSection_Base::SoildSection_Base()
{

}

SoildSection_Base::SoildSection_Base(int id, int idMaterial)
{
	m_id = id;
	m_idMaterial = idMaterial;
}

void SoildSection_Base::Disp()
{
	std::cout << m_id << " " << m_idMaterial << " " << m_t << " " << std::endl;
}

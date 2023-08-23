#include "Material.h"


Material::Material(int id, double E, double possion, double density)
{
	m_id = id;
	m_E = E;
	m_v = possion;
	m_density = density;
}

Material::Material()
{

}

void Material::Disp()
{
	std::cout << m_id << " " << m_E << " " << m_v << "\n";
}

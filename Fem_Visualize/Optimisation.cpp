#include "Optimisation.h"

Optimisation::Optimisation(int id, int idNode, int ixyz)
{
	m_id = id;
	m_idNode = idNode;
	m_direction = ixyz;
}

Optimisation::Optimisation()
{

}

void Optimisation::Disp()
{
	std::cout << m_id << " " << m_idNode << " " << m_direction <<  "\n";
}

void Optimisation::Set_x1(VectorXd& x1)
{

}

#include "SoildElement_Terach3D.h"
#include "Section_Base.h"
#include "SoildSection_Base.h"
#include "StructureFem.h"
#include "NodeFem.h"
#include <Eigen/Dense>

void SoildElement_Terach3D::calculate_D()
{
	StructureFem* pst = Get_Structure();
	Section_Base* pSection = pst->Find_Section(m_idSection);

	double E, nu;
	pSection->Get_Ev(E, nu);

	double a = E / (1 + nu) / (1 - 2 * nu);
	double b = 1. - nu;
	double c = (1. - 2 * nu) / 2.;
	m_D.resize(6, 6);
	m_D << a * b, a* nu, a* nu, 0., 0., 0.,
		a* nu, a* b, a* nu, 0., 0., 0.,
		a* nu, a* nu, a* b, 0., 0., 0.,
		0., 0., 0., a* c, 0., 0.,
		0., 0., 0., 0., a* c, 0.,
		0., 0., 0., 0., 0., a* c;
}

void SoildElement_Terach3D::calculate_B()
{
	Eigen::MatrixXd A(4, 4);
	
	Eigen::VectorXd belta(4);
	Eigen::VectorXd gama(4);
	Eigen::VectorXd delta(4);
	StructureFem* pst = Get_Structure();

	for (int i = 0; i < 4; ++i) {
		NodeFem* node = pst->Find_Node(m_idNode[i]);
		A(i, 0) = 1.0;
		A(i, 1) = node->m_x;
		A(i, 2) = node->m_y;
		A(i, 3) = node->m_z;
	}

	for (int i = 0; i < 4; ++i) 
	{
		Eigen::MatrixXd belta_i(3, 3);
		belta_i = Remove(i, 1, A);
		belta[i] = std::pow(-1, i + 1) * belta_i.determinant();

		Eigen::MatrixXd gama_i(3, 3);
		gama_i = Remove(i, 2, A);
		gama[i] = std::pow(-1, i + 2) * gama_i.determinant();

		Eigen::MatrixXd delta_i(3, 3);
		delta_i = Remove(i, 3, A);
		delta[i] = std::pow(-1, i + 1) * delta_i.determinant();
	}

	m_B.resize(6, 12);
	m_B.row(0) << belta[0], 0.0, 0.0, belta[1], 0.0, 0.0, belta[2], 0.0, 0.0, belta[3], 0.0, 0.0;
	m_B.row(1) << 0.0, gama[0], 0.0, 0.0, gama[1], 0.0, 0.0, gama[2], 0.0, 0.0, gama[3], 0.0;
	m_B.row(2) << 0.0, 0.0, delta[0], 0.0, 0.0, delta[1], 0.0, 0.0, delta[2], 0.0, 0.0, delta[3];
	m_B.row(3) << gama[0], belta[0], 0.0, gama[1], belta[1], 0.0, gama[2], belta[2], 0.0, gama[3], belta[3], 0.0;
	m_B.row(4) << 0.0, delta[0], gama[0], 0.0, delta[1], gama[1], 0.0, delta[2], gama[2], 0.0, delta[3], gama[3];
	m_B.row(5) << delta[0], 0.0, belta[0], delta[1], 0.0, belta[1], delta[2], 0.0, belta[2], delta[3], 0.0, belta[3];

	m_B = 1 / (6 * m_volume) * m_B;
}

void SoildElement_Terach3D::calculate_volume()
{
	StructureFem* pst = Get_Structure();

	Eigen::MatrixXd V(4, 4);
	for (int i = 0; i < m_idNode.size(); ++i) 
	{
		NodeFem* node = pst->Find_Node(m_idNode[i]);
		V(i, 0) = 1.0;
		V(i, 1) = node->m_x;
		V(i, 2) = node->m_y;
		V(i, 3) = node->m_z;
	}
	m_volume = std::abs(V.determinant() / 6.0);
}

void SoildElement_Terach3D::calculate_all()
{
	calculate_volume();
	calculate_B();
	calculate_D();
	calculate_ke();
}

void SoildElement_Terach3D::calculate_ke()
{
	m_ke.resize(12, 12);
	m_ke = m_volume * m_B.transpose() * m_D * m_B;
}

void SoildElement_Terach3D::Disp()
{
	std::cout << m_id << " " << m_idSection << " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}

	std::cout << "\n";
}

void SoildElement_Terach3D::Input_Data(std::ifstream& fin)
{
	fin >> m_id;
	fin >> m_idSection;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
}

int SoildElement_Terach3D::Get_DOF_Node()
{
	m_dof = 3;
	return m_dof;
}

Eigen::MatrixXd SoildElement_Terach3D::Remove(int rowToRemove, int colToRemove, Eigen::MatrixXd originalMatrix)
{
	int numRows = originalMatrix.rows();
	int numCols = originalMatrix.cols();

	Eigen::MatrixXd newMatrix(numRows - 1, numCols - 1);

	for (int row = 0; row < numRows - 1; ++row) 
	{
		for (int col = 0; col < numCols - 1; ++col) 
		{
			int oldRow = row < rowToRemove ? row : row + 1;
			int oldCol = col < colToRemove ? col : col + 1;
			newMatrix(row, col) = originalMatrix(oldRow, oldCol);
		}
	}
	//cout << originalMatrix << endl;
	//cout << newMatrix << endl;
	return newMatrix;
}


SoildElement_Terach3D::SoildElement_Terach3D()
{
	m_idNode.resize(4);
}

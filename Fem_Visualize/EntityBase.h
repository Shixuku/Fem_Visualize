#pragma once
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
using namespace Eigen;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> Tri;//��˵����Ԫ��

class StructureFem;

class EntityBase
{
private:
	static StructureFem* pStructure;

public:
	int m_id;//������

	static StructureFem* Get_Structure()
	{
		if (pStructure == nullptr)
		{
			std::cout << "pStructure is nullptr\n";
			std::cout << "Please Run Set_Structure first!\n";
			exit(1);
		}
		return pStructure;
	}

	static void Set_Structure(StructureFem* pStructure)
	{
		EntityBase::pStructure = pStructure;
	}

	virtual ~EntityBase(){ }
};


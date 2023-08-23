#pragma once
#include "SoildElement_Base.h"
#include <Eigen/Dense>

class SoildElement_Terach3D :
	public SoildElement_Base
{
public:
	void calculate_D() override;

	void calculate_B() override;

	void calculate_volume() override;

	void calculate_all() override;

	void calculate_ke() override;

	void Disp() override;

	void Input_Data(std::ifstream& fin);

	int Get_DOF_Node() override;

	Eigen::MatrixXd Remove(int rowToRemove, int colToRemove, Eigen::MatrixXd originalMatrix);

	SoildElement_Terach3D();
	SoildElement_Terach3D(int id, int node1, int node2, int node3, int node4);
};
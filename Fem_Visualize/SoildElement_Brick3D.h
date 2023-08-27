#pragma once
#include "SoildElement_Base.h"
#include <tuple>
class SoildElement_Brick3D :
    public SoildElement_Base
{

public:
	double m_DetJ; // 雅可比矩阵的值

	void calculate_D() override;

	void calculate_B() override;

	std::tuple <Eigen::MatrixXd, double> calculate_B(Eigen::VectorXd& x);

	void calculate_volume() override;

	void calculate_all() override;

	void calculate_ke() override;

	void Disp() override;

	void Input_Data(std::ifstream& fin);

	int Get_DOF_Node() override;

	void calculate_Stress(Eigen::VectorXd disp);

	SoildElement_Brick3D();
	SoildElement_Brick3D(int id, std::vector<int> idNodes);

};


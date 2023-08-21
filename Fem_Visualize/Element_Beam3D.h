#pragma once
#include "LinkElement_Base.h"
class Element_Beam3D : public LinkElement_Base
{
public:
	Element_Beam3D();

	RowVector3d m_g;
	RowVector3d m_e1, m_e2, m_e3;      //���ֲ����귽������,e3Ϊָ������,e1ͬ��Ԫ����e2ͨ����˵ó�


	virtual int Get_DOF_Node() override { return 6; }
	void Input_Data(std::ifstream& fin)override;
	void Disp();
	void Get_ke(MatrixXd& ke) override;

	void calculate_ke()override;
	void calculate_me()override;
	void calculate_T()override;
};


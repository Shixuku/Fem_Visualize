#pragma once
#include "EntityBase.h"
class Section_Base : public EntityBase
{
public:
	int m_idMaterial;// ����
	double m_density;  // �ܶ�
	double m_area;     // ���
	double Get_E();
	double Get_G();
	void Get_Ev(double& E, double& v);

	virtual void Disp();//��ʾ����
};


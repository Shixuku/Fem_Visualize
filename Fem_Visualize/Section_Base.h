#pragma once
#include "EntityBase.h"
class Section_Base : public EntityBase
{
public:
	int m_idMaterial;//����
	double Get_E();
	double Get_G();
	void Get_Ev(double& E, double& v);

	virtual void Disp();//��ʾ����
};


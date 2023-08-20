#pragma once
#include "EntityBase.h"
class Section_Base : public EntityBase
{
public:
	int m_idMaterial;// 材料
	double m_density;  // 密度
	double m_area;     // 面积
	double Get_E();
	double Get_G();
	void Get_Ev(double& E, double& v);

	virtual void Disp();//显示数据
};


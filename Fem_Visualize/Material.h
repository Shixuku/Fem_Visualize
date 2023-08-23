#pragma once
#include "EntityBase.h"
class Material : public EntityBase
{
public:
	double m_E;
	double m_v;
	double m_density;

	Material();
	Material(int id, double E, double possion, double density);

	
	void Disp();//ÏÔÊ¾Êı¾İ
};


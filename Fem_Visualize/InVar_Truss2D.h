#pragma once
#include "inVar.h"
class InVar_Truss2D :
    public InVar
{
	//平面桁架单元内变量自由度
private:
	virtual void Calculate_ke() override;//计算刚度矩阵元素

};


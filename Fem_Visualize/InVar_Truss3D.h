#pragma once
#include "inVar.h"
#include "StructureFem.h"
#include "LinkElement_Truss3D.h"

class InVar_Truss3D :
    public InVar
{
	//平面梁单元内变量自由度
private:
	virtual void Calculate_ke() override;//计算刚度矩阵元素
};


#pragma once
#include "inVar.h"
#include "StructureFem.h"
#include "LinkElement_Truss3D.h"

class InVar_Truss3D :
    public InVar
{
	//ƽ������Ԫ�ڱ������ɶ�
private:
	virtual void Calculate_ke() override;//����նȾ���Ԫ��
};


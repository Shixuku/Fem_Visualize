#pragma once
#include "Section_Base.h"
class Section_Truss3D : public Section_Base
{//��ܽ���
public:
    double m_Area;

    Section_Truss3D();
    Section_Truss3D(int id, int idMaterial, double Area);

    virtual void Disp() override;//��ʾ����
};


#pragma once
#include "Section_Base.h"
class Section_Truss : public Section_Base
{//��ܽ���
public:
    double m_Area;

    Section_Truss();
    Section_Truss(int id, int idMaterial, double Area);

    virtual void Disp() override;//��ʾ����
};


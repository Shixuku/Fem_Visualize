#pragma once
#include "Section_Base.h"
class Section_Truss : public Section_Base
{//��ܽ���
public:
    double m_Area;

    virtual void Disp() override;//��ʾ����
};


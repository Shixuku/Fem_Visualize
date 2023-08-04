#pragma once
#include "Section_Base.h"
class Section_Truss : public Section_Base
{//桁架截面
public:
    double m_Area;

    virtual void Disp() override;//显示数据
};


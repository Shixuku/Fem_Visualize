#pragma once
#include "Section_Truss.h"
class Section_Beam3D : public Section_Truss
{
public:
    double m_Iz;
    double m_Iy;

    double Get_J();
    virtual void Disp() override;//ÏÔÊ¾Êı¾İ

};


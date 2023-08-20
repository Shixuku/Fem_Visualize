#pragma once
#include "Section_Truss.h"
class Section_Beam3D : public Section_Truss
{
public:
    double m_Ix;
    double m_Iz;
    double m_Iy;

    double Get_J();
    virtual void Disp() override;//ÏÔÊ¾Êý¾Ý

};


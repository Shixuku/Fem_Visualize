#pragma once
#include "Section_Base.h"
class SoildSection_Base :
    public Section_Base
{
public:
    double m_t;
    SoildSection_Base();
    SoildSection_Base(int id, int idMaterial);
    void Disp();//ÏÔÊ¾Êý¾Ý
};


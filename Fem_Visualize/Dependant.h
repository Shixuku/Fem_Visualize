#pragma once
#include "EntityBase.h"
#include"LinkElement_Base.h"

class Dependant : public EntityBase
{//主从信息
public:
	int m_idMainNode;         //主节点号
	int m_idSubNode;          //从节点号
	int m_idHost[6];          //自由度对应的主节点号
	int m_constrainValue;   
	void Set_HostSize();
	void Set_DOF(int& iStart);//处理主从信息的自由度

};


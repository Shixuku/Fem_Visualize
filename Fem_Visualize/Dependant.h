#pragma once
#include "EntityBase.h"
#include"LinkElement_Base.h"

class Dependant : public EntityBase
{//������Ϣ
public:
	int m_idMainNode;         //���ڵ��
	int m_idSubNode;          //�ӽڵ��
	int m_idHost[6];          //���ɶȶ�Ӧ�����ڵ��
	int m_constrainValue;   
	void Set_HostSize();
	void Set_DOF(int& iStart);//����������Ϣ�����ɶ�

};


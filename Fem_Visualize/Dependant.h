#pragma once
#include "EntityBase.h"
#include"Element_Base.h"

class Dependant : public EntityBase
{//������Ϣ
public:
	int m_idNode;             //�ӽڵ��
	int m_idHost[6];          //���ɶȶ�Ӧ�����ڵ��
	void Set_HostSize();
	void Set_DOF(int& iStart);//����������Ϣ�����ɶ�

};


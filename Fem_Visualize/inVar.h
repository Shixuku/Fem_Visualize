#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

#include <Eigen/Dense> //ʹ�ó��ܾ����� MatrixXd
#include <Eigen/Sparse>

using namespace Eigen;
typedef Eigen::Triplet<double> Tri;//��˵����Ԫ��
typedef std::list<Tri> ListTri;//��Ԫ������

class InVar : public EntityBase
{//��Ԫ�ڱ������ɶ�
protected:
	std::vector<double> m_ke;//����նȾ���Ԫ��
	virtual void Calculate_ke() = 0;//����նȾ���Ԫ��

public:
	int m_idElement = -1;//���ڵ�Ԫ
	int m_Type = 1;//����(��������������ص�)
	int m_Itotv = -1;//���ɶȱ��
	bool m_bFixedValue = true;//�Ƿ�ָ���ڱ���������ָ����
	double m_Value = 0;
	double m_Force = 0;

	void Input(std::ifstream& fin);//��ȡ
	void Get_ke(std::vector<double>& ke);//�õ��նȾ���Ԫ��
	void Assemble_L(ListTri& L11, ListTri& L21, ListTri& L22);

	void Assemble_L3(ListTri& L11, ListTri& L21, ListTri& L22, ListTri& L31, ListTri& L32, ListTri& L33,
		ListTri& L41, ListTri& L42, ListTri& L43, ListTri& L44);

	void Disp() const;//��ʾ���

};


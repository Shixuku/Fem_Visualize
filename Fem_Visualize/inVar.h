#pragma once
#include "EntityBase.h"
#include <vector>
#include <fstream>

#include <Eigen/Dense> //使用稠密矩阵类 MatrixXd
#include <Eigen/Sparse>

using namespace Eigen;
typedef Eigen::Triplet<double> Tri;//重说明三元组
typedef std::list<Tri> ListTri;//三元组链表

class InVar : public EntityBase
{//单元内变量自由度
protected:
	std::vector<double> m_ke;//保存刚度矩阵元素
	virtual void Calculate_ke() = 0;//计算刚度矩阵元素

public:
	int m_idElement = -1;//所在单元
	int m_Type = 1;//类型(轴力、剪力、弯矩等)
	int m_Itotv = -1;//自由度编号
	bool m_bFixedValue = true;//是否指定内变量，否则指定力
	double m_Value = 0;
	double m_Force = 0;

	void Input(std::ifstream& fin);//读取
	void Get_ke(std::vector<double>& ke);//得到刚度矩阵元素
	void Assemble_L(ListTri& L11, ListTri& L21, ListTri& L22);

	void Assemble_L3(ListTri& L11, ListTri& L21, ListTri& L22, ListTri& L31, ListTri& L32, ListTri& L33,
		ListTri& L41, ListTri& L42, ListTri& L43, ListTri& L44);

	void Disp() const;//显示结果

};


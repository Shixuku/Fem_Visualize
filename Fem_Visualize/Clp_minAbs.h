#pragma once
#include <Eigen/Dense>
typedef Eigen::MatrixXd Mtx;
typedef Eigen::VectorXd Vtr;

class Clp_minAbs
{//求解 min( f = |A*x+b0-b| )
private:
	const Mtx& m_A;
	Vtr m_b;
	int m_TypeMethod = 0;//缺省使用单纯形法

public://构造函数
	Clp_minAbs(const Mtx& A,const Vtr& b);
	Clp_minAbs(const Mtx& A,const Vtr& b,const Vtr& b0);

public://设置求解方法
	void Change_Method(int iMethod)
	{//0:单纯形法，1:对偶理论，2:内点法
		m_TypeMethod = iMethod;
	}

public://求解
	double Solve(Vtr& x);//返回 fmin
};


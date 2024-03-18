#include "Clp_minAbs.h"
#include "coin-or/ClpSimplex.hpp"
#include <vector>
#include <iostream>

Clp_minAbs::Clp_minAbs(const Mtx& A, const Vtr& b) :
	m_A(A),
	m_b(b)
{
    assert(A.rows() == b.size());
}

Clp_minAbs::Clp_minAbs(const Mtx& A, const Vtr& b, const Vtr& b0):
	m_A(A),
	m_b(b)
{
    assert(A.rows() == b.size());
    assert(b0.size() == b.size());

    m_b -= b0;
 //   std::cout << "b=\n" << m_b << "\n\n";
}

double Clp_minAbs::Solve(Vtr& x)
{//建立线性规划问题，并求解
    auto nRows = m_A.rows();//矩阵行数
    auto nCols = m_A.cols();//矩阵列数
    auto nVars = nCols + 1;//决策变量的个数

    ClpSimplex model;// 创建线性规划模型
    model.resize(0, nVars); // 初始化模型的变量数量

    // 设置目标函数中，各决策变量的系数
    model.setObjectiveCoefficient(nCols, 1.0); //f的系数

    //设置各决策变量的上下界
    for (int i = 0; i < nCols; ++i)
    {//原设计变量无限制
        model.setColumnBounds(i, -DBL_MAX, DBL_MAX);
    }
    model.setColumnBounds(nCols, 0, DBL_MAX);//辅助变量有下届

    // 设置约束方程
    int* columns=new int[nVars];
    for (int i = 0; i < nVars; ++i)
    {
        columns[i] = i;
    }    
    double* elements=new double[nVars];
    for (int i = 0; i < nRows; ++i)
    {
        for (int j = 0; j < nCols; ++j)
        {//取矩阵的一行系数
            elements[j] = m_A(i, j);
        }
        elements[nCols] = -1;
        model.addRow(nVars, columns, elements, -COIN_DBL_MAX, m_b[i]);
        // -COIN_DBL_MAX表示没有下界
    }
    for (int i = 0; i < nRows; ++i)
    {
        for (int j = 0; j < nCols; ++j)
        {//取矩阵的一行系数
            elements[j] = m_A(i, j);
        }
        elements[nCols] = 1;
        model.addRow(nVars, columns, elements, m_b[i], COIN_DBL_MAX);
        // COIN_DBL_MAX表示没有上界
    }
    delete[] columns;
    delete[] elements;
    
    switch (m_TypeMethod)
    {
    case 1:// 对偶理论
        model.initialDualSolve(); break;
    case 2:// 内点法
        model.initialPrimalSolve(); break;
    default:// 单纯形法
        model.initialSolve();
    }
 
    // 获取结果
    x.resize(nCols);
    const double* solution = model.primalColumnSolution();
    for (int i = 0; i < nCols; i++)
    {
        x[i] = solution[i];
    }

    double fmin = model.objectiveValue();
	return fmin;
}

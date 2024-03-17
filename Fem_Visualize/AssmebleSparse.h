#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <list>
typedef Eigen::SparseMatrix<double> SpMat;//重说明列优先稀疏矩阵
typedef Eigen::Triplet<double> Tri;//重说明三元组
typedef std::list<Tri> ListTri;//定义一个三元组链表

#include "Manage_L.h"
template class Manage_L<ListTri>;
template class Manage_L<SpMat>;

class AssmebleSparse
{//将单元矩阵组装为分块稀疏矩阵
private:
	std::vector<int> m_Ni;//各分块矩阵的行数
	Manage_L<ListTri> m_Li;//各分块矩阵的三元组链表

	//根据自由度编号iDOF，找到元素应该组装的分块矩阵位置
	int Find_Pos(int iDOF, int& iStart);//iStart：该分块的起始行位置

	AssmebleSparse(const AssmebleSparse&) = delete; // 禁止拷贝
	AssmebleSparse& operator=(const AssmebleSparse&) = delete; // 禁止复制

public://构造函数
	AssmebleSparse(const std::vector<int>& Ni);//Ni:各分块矩阵的行数

public://组装单元矩阵
	void Assemble(const std::vector<int>& DOFs, const Eigen::MatrixXd& ke);

public://得到各稀疏分块矩阵
	void GetSparse(Manage_L<SpMat>& Ks) const;
};


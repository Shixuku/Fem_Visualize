#include "AssmebleSparse.h"
#include <cassert>

AssmebleSparse::AssmebleSparse(const std::vector<int>& Ni) : m_Ni(Ni)
{
	int nRows = Ni.size();//分块矩阵的分块数
	m_Li.resize(nRows);
}

int AssmebleSparse::Find_Pos(int iDOF, int& iStart)
{//根据自由度编号iDOF，找到元素应该组装的分块矩阵位置
//iStart：该分块的起始行位置
	int nRows = m_Ni.size();//分块矩阵的分块数

	iStart = 0;
	int iEnd = 0;
	for (int i = 0; i < nRows; ++i)
	{//对分块的行循环
		iEnd += m_Ni[i];
		if (iDOF < iEnd) return i;
		iStart = iEnd;
	}
	return -1;
}

void AssmebleSparse::Assemble(const std::vector<int>& DOFs, const Eigen::MatrixXd& ke)
{//组装一个单元矩阵
	int iStart, jStart;
	int nDOF = DOFs.size();
	assert(nDOF == ke.rows());
	for (int i = 0; i < nDOF; ++i)
	{//对行自由度循环
		const auto& ki = ke.row(i);
		int it = DOFs[i];//行自由度编号
		int iPos = Find_Pos(it, iStart);//行分块位置
		assert(iPos != -1);
		for (int j = 0; j < nDOF; ++j)
		{//对列自由度循环
			const double& kij = ki[j];
			int jt = DOFs[j];//列自由度编号
			int jPos = Find_Pos(jt, jStart);//列分块位置
			assert(jPos != -1);
			if (iPos >= jPos)
			{//元素位于下三角块
				m_Li(iPos, jPos).push_back(Tri(it - iStart, jt - jStart, kij));
			}
		}
	}
}

void AssmebleSparse::GetSparse(Manage_L<SpMat>& Ks) const
{//得到各稀疏分块矩阵
	int nRows = m_Ni.size();//分块矩阵的分块数
	Ks.resize(nRows);
	for (int i = 0; i < nRows; ++i)
	{//对行块循环
		for (int j = 0; j <= i; ++j)
		{//对列块循环
			const ListTri& Lij = m_Li(i, j);
			SpMat& Kij = Ks(i, j);
			Kij.resize(m_Ni[i], m_Ni[j]);
			Kij.setZero();//避免多次组装的重复问题
			Kij.setFromTriplets(Lij.begin(), Lij.end());
			Kij.makeCompressed();
		}
	}
}

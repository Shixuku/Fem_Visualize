#include "AssmebleSparse.h"
#include <cassert>

AssmebleSparse::AssmebleSparse(const std::vector<int>& Ni) : m_Ni(Ni)
{
	int nRows = Ni.size();//�ֿ����ķֿ���
	m_Li.resize(nRows);
}

int AssmebleSparse::Find_Pos(int iDOF, int& iStart)
{//�������ɶȱ��iDOF���ҵ�Ԫ��Ӧ����װ�ķֿ����λ��
//iStart���÷ֿ����ʼ��λ��
	int nRows = m_Ni.size();//�ֿ����ķֿ���

	iStart = 0;
	int iEnd = 0;
	for (int i = 0; i < nRows; ++i)
	{//�Էֿ����ѭ��
		iEnd += m_Ni[i];
		if (iDOF < iEnd) return i;
		iStart = iEnd;
	}
	return -1;
}

void AssmebleSparse::Assemble(const std::vector<int>& DOFs, const Eigen::MatrixXd& ke)
{//��װһ����Ԫ����
	int iStart, jStart;
	int nDOF = DOFs.size();
	assert(nDOF == ke.rows());
	for (int i = 0; i < nDOF; ++i)
	{//�������ɶ�ѭ��
		const auto& ki = ke.row(i);
		int it = DOFs[i];//�����ɶȱ��
		int iPos = Find_Pos(it, iStart);//�зֿ�λ��
		assert(iPos != -1);
		for (int j = 0; j < nDOF; ++j)
		{//�������ɶ�ѭ��
			const double& kij = ki[j];
			int jt = DOFs[j];//�����ɶȱ��
			int jPos = Find_Pos(jt, jStart);//�зֿ�λ��
			assert(jPos != -1);
			if (iPos >= jPos)
			{//Ԫ��λ�������ǿ�
				m_Li(iPos, jPos).push_back(Tri(it - iStart, jt - jStart, kij));
			}
		}
	}
}

void AssmebleSparse::GetSparse(Manage_L<SpMat>& Ks) const
{//�õ���ϡ��ֿ����
	int nRows = m_Ni.size();//�ֿ����ķֿ���
	Ks.resize(nRows);
	for (int i = 0; i < nRows; ++i)
	{//���п�ѭ��
		for (int j = 0; j <= i; ++j)
		{//���п�ѭ��
			const ListTri& Lij = m_Li(i, j);
			SpMat& Kij = Ks(i, j);
			Kij.resize(m_Ni[i], m_Ni[j]);
			Kij.setZero();//��������װ���ظ�����
			Kij.setFromTriplets(Lij.begin(), Lij.end());
			Kij.makeCompressed();
		}
	}
}

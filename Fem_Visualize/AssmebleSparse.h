#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <list>
typedef Eigen::SparseMatrix<double> SpMat;//��˵��������ϡ�����
typedef Eigen::Triplet<double> Tri;//��˵����Ԫ��
typedef std::list<Tri> ListTri;//����һ����Ԫ������

#include "Manage_L.h"
template class Manage_L<ListTri>;
template class Manage_L<SpMat>;

class AssmebleSparse
{//����Ԫ������װΪ�ֿ�ϡ�����
private:
	std::vector<int> m_Ni;//���ֿ���������
	Manage_L<ListTri> m_Li;//���ֿ�������Ԫ������

	//�������ɶȱ��iDOF���ҵ�Ԫ��Ӧ����װ�ķֿ����λ��
	int Find_Pos(int iDOF, int& iStart);//iStart���÷ֿ����ʼ��λ��

	AssmebleSparse(const AssmebleSparse&) = delete; // ��ֹ����
	AssmebleSparse& operator=(const AssmebleSparse&) = delete; // ��ֹ����

public://���캯��
	AssmebleSparse(const std::vector<int>& Ni);//Ni:���ֿ���������

public://��װ��Ԫ����
	void Assemble(const std::vector<int>& DOFs, const Eigen::MatrixXd& ke);

public://�õ���ϡ��ֿ����
	void GetSparse(Manage_L<SpMat>& Ks) const;
};


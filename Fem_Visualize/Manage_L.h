#pragma once
#include <memory>
#include <stdexcept>

template<class T>
class Manage_L
{//ģ����ʽ��������Ԫ�ع���(Ԫ�ذ�����������)
private:
	int m_nRows = 0; // ��������
	std::unique_ptr<T[]> m_data;

	Manage_L(const Manage_L&) = delete; // ��ֹ����
	Manage_L& operator=(const Manage_L&) = delete; // ��ֹ����

public:
	Manage_L(void) : m_nRows(0), m_data(nullptr) {} // ���캯��
	Manage_L(Manage_L&& other) noexcept; // �ƶ����캯��
	Manage_L& operator=(Manage_L&& other) noexcept; // �ƶ���ֵ������

	const T& operator()(int i, int j) const; // ȡ��(i,j)λ�õ�Ԫ��
	T& operator()(int i, int j); // ����(i,j)λ�õ�Ԫ��

	int Rows() const { return m_nRows; } // ���ؾ��������
	void resize(int nRows); // �������������������
};

template<class T>
Manage_L<T>::Manage_L(Manage_L&& other) noexcept
	: m_nRows(other.m_nRows), m_data(std::move(other.m_data))
{// �ƶ����캯��
	other.m_nRows = 0;
}

template<class T>
Manage_L<T>& Manage_L<T>::operator=(Manage_L&& other) noexcept
{// �ƶ���ֵ������
	if (this != &other)
	{
		m_nRows = other.m_nRows;
		m_data = std::move(other.m_data);
		other.m_nRows = 0;
	}
	return *this;
}

template<class T>
const T& Manage_L<T>::operator()(int i, int j) const
{// ȡ��(i,j)λ�õ�Ԫ��
	if (j > i || i >= m_nRows)
	{
		throw std::out_of_range("Index out of range");
	}
	int pos = i * (i + 1) / 2 + j;
	return m_data[pos];
}

template<class T>
T& Manage_L<T>::operator()(int i, int j)
{// ����(i,j)λ�õ�Ԫ��
	if (j > i || i >= m_nRows)
	{
		throw std::out_of_range("Index out of range");
	}
	int pos = i * (i + 1) / 2 + j;
	return m_data[pos];
}

template<class T>
void Manage_L<T>::resize(int nRows)
{// �������������������
	if (nRows <= 0)
	{
		throw std::invalid_argument("Number of rows must be positive");
	}
	if (m_nRows != nRows)
	{
		m_nRows = nRows;
		int n = nRows * (nRows + 1) / 2;
		m_data = std::make_unique<T[]>(n);
	}
}
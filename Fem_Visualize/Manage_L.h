#pragma once
#include <memory>
#include <stdexcept>

template<class T>
class Manage_L
{//模板形式的下三角元素管理(元素按行优先排列)
private:
	int m_nRows = 0; // 矩阵行数
	std::unique_ptr<T[]> m_data;

	Manage_L(const Manage_L&) = delete; // 禁止拷贝
	Manage_L& operator=(const Manage_L&) = delete; // 禁止复制

public:
	Manage_L(void) : m_nRows(0), m_data(nullptr) {} // 构造函数
	Manage_L(Manage_L&& other) noexcept; // 移动构造函数
	Manage_L& operator=(Manage_L&& other) noexcept; // 移动赋值操作符

	const T& operator()(int i, int j) const; // 取得(i,j)位置的元素
	T& operator()(int i, int j); // 设置(i,j)位置的元素

	int Rows() const { return m_nRows; } // 返回矩阵的行数
	void resize(int nRows); // 根据总行数，重设矩阵
};

template<class T>
Manage_L<T>::Manage_L(Manage_L&& other) noexcept
	: m_nRows(other.m_nRows), m_data(std::move(other.m_data))
{// 移动构造函数
	other.m_nRows = 0;
}

template<class T>
Manage_L<T>& Manage_L<T>::operator=(Manage_L&& other) noexcept
{// 移动赋值操作符
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
{// 取得(i,j)位置的元素
	if (j > i || i >= m_nRows)
	{
		throw std::out_of_range("Index out of range");
	}
	int pos = i * (i + 1) / 2 + j;
	return m_data[pos];
}

template<class T>
T& Manage_L<T>::operator()(int i, int j)
{// 设置(i,j)位置的元素
	if (j > i || i >= m_nRows)
	{
		throw std::out_of_range("Index out of range");
	}
	int pos = i * (i + 1) / 2 + j;
	return m_data[pos];
}

template<class T>
void Manage_L<T>::resize(int nRows)
{// 根据总行数，重设矩阵
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
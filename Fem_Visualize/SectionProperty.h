#pragma once
#include <iostream>
#include <QString>

/* ��������� */
class Section
{
public:
	Section(int number, const QString& name, const QString& type, const QString& shape)
		: sectionNumber(number), sectionName(name), sectionType(type), sectionShape(shape)
	{}
	virtual ~Section() {}

	int sectionNumber;
	QString sectionName;
	QString sectionType;
	QString sectionShape;
};

/* ʵ��Բ���� */
class SoildCirSection : public Section
{
public:
	SoildCirSection(int sectionNumber, QString sectionName, QString sectionType, QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape) {}

    double radius() const { return m_radius; }
    void setRadius(double radius) { m_radius = radius; }
    double m_radius;
};

/* I�ͽ��� */
class ISection : public Section {
public:
	ISection(int sectionNumber, QString sectionName, QString sectionType,
		QString sectionShape): Section(sectionNumber, sectionName, "user", "I")
	{}

	double m_H;
	double m_B1;
	double m_tw;
	double m_tf1;
	double m_B2;
	double m_tf2;
	double m_r1;
	double m_r2;
};

/* ���ͽ��� */
class TubeSection : public Section {
public:
	TubeSection(int sectionNumber, QString sectionName, QString sectionType,QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape){}

	double m_D;
	double m_tw;
};

/* ʵ�ĳ����ν��� */
class SoildRecSection : public Section {
public:
	SoildRecSection(int sectionNumber, QString sectionName, QString sectionType,QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape)
		{}

	double getH() const { return m_H; }
	double getB() const { return m_B; }

	double m_H;
	double m_B;
};

/* ���ͽ��� */
class BoxSection : public Section {
public:
	BoxSection(int sectionNumber, QString sectionName, QString sectionType, QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape) {}

	double m_H;
	double m_B;
	double m_tw;
	double m_tf1;
	double m_C;
	double m_tf2;
};
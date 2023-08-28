#pragma once
#include <iostream>
#include <QString>

/* 抽象类截面 */
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

/* 实心圆截面 */
class SoildCirSection : public Section
{
public:
	SoildCirSection(int sectionNumber, QString sectionName, QString sectionType, QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape) {}

    double radius() const { return m_radius; }
    void setRadius(double radius) { m_radius = radius; }
    double m_radius;
};

/* I型截面 */
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

/* 管型截面 */
class TubeSection : public Section {
public:
	TubeSection(int sectionNumber, QString sectionName, QString sectionType,QString sectionShape)
		: Section(sectionNumber, sectionName, sectionType, sectionShape){}

	double m_D;
	double m_tw;
};

/* 实心长方形截面 */
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

/* 箱型截面 */
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
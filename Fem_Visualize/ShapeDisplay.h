#pragma once
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLineSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkPoints.h>
#include <vtkTubeFilter.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <QDebug>

class ShapeDisplay
{
public:
	ShapeDisplay() {};

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> leftCenterDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rigntCenterDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> leftTowerPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightTowerPoints = vtkSmartPointer<vtkPoints>::New();


	vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();


	virtual vtkSmartPointer<vtkActor> GetActor();

	void SetActor() 
	{
		CreateCell(points);
		LinkActor();
	}

protected:
	virtual void CreateCell(vtkSmartPointer<vtkPoints> points) = 0;
	virtual void LinkActor() = 0;
};

class LineDisplay : public ShapeDisplay
{
public:
	LineDisplay();

	void CreateCell(vtkSmartPointer<vtkPoints> points) override;
	void LinkActor() override;
protected:
	

};

class SectionDisplay : public ShapeDisplay
{
public:
	SectionDisplay() {}


};

class PointsDisplay : public ShapeDisplay
{
public:
	PointsDisplay();
	void CreateCell(vtkSmartPointer<vtkPoints> points) override;
	void LinkActor() override;
};

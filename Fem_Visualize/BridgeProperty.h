#pragma once
#include <iostream>
#include <vtkAutoInit.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkTransform.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtktextRenderer.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkDiskSource.h>
#include <vtkTriangle.h>
#include <vtkFeatureEdges.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkTubeFilter.h>
#include <vtkPolygon.h>
#include <vtkDelaunay2D.h>
#include <vector>
#include <sstream>
#include <string>
#include "qstring.h"
#include "ShapeDisplay.h"

using namespace std;

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)

class BridgeProperty
{
public:
	BridgeProperty();

	/* ������ת���� */
	void ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix);

	/* ����ַ��� */
	std::string GenerateCoordinateString(const std::string& input);

	/* �����ַ����������� */
	void GeneratePoints(const std::string& input, string flag, double startPoint[3], vtkSmartPointer<vtkPoints> point);

	/* �������ν��� */
	void CreateCrossSectionShapeTube(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* �������ν��� */
	void CreateCrossSectionShapeTube_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* ����Բ�ν��� */
	void CreateCrossSectionShapeCir(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* ����Բ�ν��� */
	void CreateCrossSectionShapeCir_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* ѡ�����󴴽����ν��� */
	void CreateCrossSectionShapeRec(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double stratPoint[3], double endPoint[3]);

	/* ѡ�����󴴽����ͽ��� */
	void CreateCrossSectionShapeBox(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double stratPoint[3], double endPoint[3]);

	/* �����ν������� */
	void AddNewRectangular();

	/* �����ν������� */
	void AddNewCylindrical();

	/* ��Բ�ν������� */
	void AddNewCircular();

	/* ѡ��������������� */
	void GenerateBridgeDecks(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> deckShapePoints, QString setionShape);

	/* �������� */
	void  GenerateBridgeTower(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3]);

	/* �������� */
	void GenerateBridgeTower_2(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3], vtkSmartPointer<vtkPoints> towerShapePoints, QString sectionShape);

	/* ����б���� */
	void GenerateInclinedRopes(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints);

	/* ����б���� */
	void GenerateInclinedRopes_2(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints, vtkSmartPointer<vtkPoints> ropeShapePoints, QString shapeSection);

	/* �ڵ���ӻ�*/
	void VisualPoint(vtkSmartPointer<vtkActor> actor);

	/* ��ȡ�������� */
	QString GetSectionShape(QString shape);

	/* ���ݽ�������ѡȡ */
	void ChooseCreateSectionShape(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3], QString sectionShape);

	/* ��ӡ�ڵ� */
	void PrintPoint(vtkSmartPointer<vtkPoints> points);

public:
	vector<vtkSmartPointer<vtkPolyData>> g_PolyDataList;
	vector<vtkSmartPointer<vtkLinearExtrusionFilter>> g_ExtrusionList;

	vtkSmartPointer<vtkAppendPolyData> deckAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendPolyData> towerAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendPolyData> ropeAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendPolyData> duckEdgesAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkPoints> leftDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> leftTowerPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightTowerPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> centerLeftDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> centerRightDeckPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightDeckPoints = vtkSmartPointer<vtkPoints>::New();

	vtkSmartPointer<vtkRenderer> bridgeRender = vtkSmartPointer<vtkRenderer>::New();

	LineDisplay* deckLineDisplay = new LineDisplay();
	LineDisplay* towerLineDisplay = new LineDisplay();

	PointsDisplay* deckPointDisplay = new PointsDisplay();
	PointsDisplay* towerPointDisplay = new PointsDisplay();

};


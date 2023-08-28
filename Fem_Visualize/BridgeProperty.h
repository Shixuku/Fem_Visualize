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

	/* 构建旋转矩阵 */
	void ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix);

	/* 拆解字符串 */
	std::string GenerateCoordinateString(const std::string& input);

	/* 根据字符串生成坐标 */
	void GeneratePoints(const std::string& input, string flag, double startPoint[3], vtkSmartPointer<vtkPoints> point);

	/* 创建环形截面 */
	void CreateCrossSectionShapeTube(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* 创建环形截面 */
	void CreateCrossSectionShapeTube_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* 创建圆形截面 */
	void CreateCrossSectionShapeCir(double radius, double resolution, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* 创建圆形截面 */
	void CreateCrossSectionShapeCir_2(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3]);

	/* 选择截面后创建矩形截面 */
	void CreateCrossSectionShapeRec(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double stratPoint[3], double endPoint[3]);

	/* 选择截面后创建箱型截面 */
	void CreateCrossSectionShapeBox(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double stratPoint[3], double endPoint[3]);

	/* 将矩形截面拉伸 */
	void AddNewRectangular();

	/* 将环形截面拉伸 */
	void AddNewCylindrical();

	/* 将圆形截面拉伸 */
	void AddNewCircular();

	/* 选择截面后生成桥面板 */
	void GenerateBridgeDecks(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> deckShapePoints, QString setionShape);

	/* 创建桥塔 */
	void  GenerateBridgeTower(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3]);

	/* 创建桥塔 */
	void GenerateBridgeTower_2(double towerHeight, vtkSmartPointer<vtkPoints> towerPoint, double point[3], vtkSmartPointer<vtkPoints> towerShapePoints, QString sectionShape);

	/* 创建斜拉索 */
	void GenerateInclinedRopes(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints);

	/* 创建斜拉索 */
	void GenerateInclinedRopes_2(vtkSmartPointer<vtkPoints> deckPoints, vtkSmartPointer<vtkPoints> towerPoints, vtkSmartPointer<vtkPoints> ropeShapePoints, QString shapeSection);

	/* 节点可视化*/
	void VisualPoint(vtkSmartPointer<vtkActor> actor);

	/* 获取截面类型 */
	QString GetSectionShape(QString shape);

	/* 根据截面类型选取 */
	void ChooseCreateSectionShape(vtkSmartPointer<vtkPoints> points, vtkPolyData* polyData, double startPoint[3], double endPoint[3], QString sectionShape);

	/* 打印节点 */
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


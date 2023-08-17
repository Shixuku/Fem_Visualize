#pragma once

#include <QtWidgets/QWidget>
#include <vtkRendererCollection.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkAppendPolyData.h>
#include <vtkScalarBarActor.h>
#include <vtkStringArray.h>
#include "StructureFem.h"
#include "ui_Fem_Visualize.h"
#include "ReactionForceWindow.h"
#include "DisplacementWindow.h"

class Fem_Visualize : public QWidget
{
    Q_OBJECT

public:
    Fem_Visualize(QWidget *parent = nullptr);
    ~Fem_Visualize();

    void SetRenderWindow();
	void InitNode(std::map<int, NodeFem*> nodes);
    void InitElement();

	/* 构建旋转矩阵 */
	void ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix);

	/* 创建矩形截面 */
	void CreateRecSection(double length, double width, double stratPoint[3], double endPoint[3]);

	/* 显示隐藏截面*/
	void onIsShowSection();

	/* 显示位移*/
	void onShowDisplacement();

	/* 显示轴力 */
	void onShowAxialForces();

	/* 显示支座反力 */
	void onShowReactionForces();

	/* 获取自由度处的位移值 */
	double GetDofValue(int dofIndex);

	/* 计算两点之间的距离 */
	double GetDistance(double x1, double y1, double z1, double x2, double y2, double z2);

	/* 创建坐标处的Label */
	void GeneraterLable(vtkPoints* pts, vtkStringArray* label, vtkSmartPointer<vtkActor2D> actor);

	/* 获取单元轴力*/

	bool showFlag = 0;

private slots:

	/* 根据选择的不同Radio显示对应的支座反力FXYZ */
	void onSendForceType();

	/* 根据选择的不同Radio显示对应的位移*/
	void onSendDispType();

private:
    Ui::Fem_VisualizeClass ui;
	// A renderer and render window
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkActor> lineActor;
	vtkNew<vtkActor> sectionActor;
	vtkSmartPointer<vtkActor2D> pointLabelActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkActor2D> elementLableActor = vtkSmartPointer<vtkActor2D>::New();

	vtkNew<vtkScalarBarActor> scalarBar;
	// An interactor
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkNew<vtkPoints> points;
	vtkNew<vtkAppendPolyData> appendFilter;


	StructureFem* m_structure = new StructureFem();
	ReactionForceWindow* reactionForce = new ReactionForceWindow();
	DisplacementWindow* displacement = new DisplacementWindow();

	enum ForceType {
		FX,
		FY,
		FZ,
		FXYZ,
		MX,
		MY,
		MZ,
		MXYZ,
	};

	enum DispType {
		DX,
		DY,
		DZ,
		DXY,
		DYZ,
		DXZ,
		DXYZ,
	};
};

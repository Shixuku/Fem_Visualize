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

	/* ������ת���� */
	void ConstuctRotationMatrix(double startPoint[3], double endPoint[3], vtkMatrix4x4* transformMatrix);

	/* �������ν��� */
	void CreateRecSection(double length, double width, double stratPoint[3], double endPoint[3]);

	/* ��ʾ���ؽ���*/
	void onIsShowSection();

	/* ��ʾλ��*/
	void onShowDisplacement();

	/* ��ʾ���� */
	void onShowAxialForces();

	/* ��ʾ֧������ */
	void onShowReactionForces();

	/* ��ȡ���ɶȴ���λ��ֵ */
	double GetDofValue(int dofIndex);

	/* ��������֮��ľ��� */
	double GetDistance(double x1, double y1, double z1, double x2, double y2, double z2);

	/* �������괦��Label */
	void GeneraterLable(vtkPoints* pts, vtkStringArray* label, vtkSmartPointer<vtkActor2D> actor);

	/* ��ȡ��Ԫ����*/

	bool showFlag = 0;

private slots:

	/* ����ѡ��Ĳ�ͬRadio��ʾ��Ӧ��֧������FXYZ */
	void onSendForceType();

	/* ����ѡ��Ĳ�ͬRadio��ʾ��Ӧ��λ��*/
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

#pragma once
#pragma execution_character_set("utf-8")//��ֹ������������

#include <QtWidgets/QWidget>
#include <vtkRendererCollection.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkAppendPolyData.h>
#include <vtkAppendFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkStringArray.h>
#include "StructureFem.h"
#include "ui_Fem_Visualize.h"
#include "ReactionForceWindow.h"
#include "DisplacementWindow.h"
#include "DeckWindow.h"
#include "TowerWindow.h"
#include "RopeWindow.h"
#include "BridgeProperty.h"


class Fem_Visualize : public QWidget
{
    Q_OBJECT

public:
    Fem_Visualize(QWidget *parent = nullptr);
    ~Fem_Visualize();

    void SetRenderWindow();
	void InitActors();

	void InitNode(std::map<int, NodeFem*> nodes);
	void InitNodes();

    void InitElement();
	void InitElements();
	void GennerateBeamElement(vtkMergePoints *mergePoints, vtkPoints *points, int &elementId);
	void GennerateRopeElement(vtkMergePoints *mergePoints, vtkPoints *points, int &elementId);

	void InitMaterial();

	void InitSections();

	void InitBoundarys();

	void AssignSection();

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
	vtkNew<vtkActor> linkActor;
	vtkNew<vtkActor> soildActor;
	vtkNew<vtkActor2D> labelActor;
	vtkSmartPointer<vtkActor2D> nodeLabelActor = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkActor2D> elementLableActor = vtkSmartPointer<vtkActor2D>::New();

	vtkNew<vtkScalarBarActor> scalarBar;
	// An interactor
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkNew<vtkPoints> points;
	vtkSmartPointer<vtkAppendPolyData> linkAppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkAppendFilter> soildAppendFilter = vtkSmartPointer<vtkAppendFilter>::New();

	StructureFem* m_structure;
	ReactionForceWindow* reactionForce = new ReactionForceWindow();
	DisplacementWindow* displacement = new DisplacementWindow();


	BridgeProperty* bridge = new BridgeProperty();
	DeckWindow* deckWindow = new DeckWindow(bridge, this);
	TowerWindow* towerWindow = new TowerWindow(bridge, this);
	RopeWindow* ropeWindow = new RopeWindow(bridge, this);

private slots:
	void onShowDeckModel();
	void onShowTowerModel();
	void onShowRopeModel();
	void onHiddenOrShowModel();
	void onSelectFile();

private:
	void ShowDeckWindow();
	void ShowTowerWindow();
	void ShowRopeWindow();

	/* ��ʼ���Ҽ��˵� */
	void Init_Action();

	/* �����Ҽ����� */
	void contextMenuEvent(QContextMenuEvent* event) override;

	void SetTreeWidget();
	void SetRenderWidget();
	void SetMenuWidget();

	//�Ҽ��˵�����Ҫ�ı���
	QMenu* menu;
	QMenu* pointSelectMenu;    //����Ӳ˵�ʹ��
	QMenu* windowSelectMenu;    //����Ӳ˵�ʹ��
	QAction* pointSelect, * elementSelect;      //��ϵͳƤ��
	QAction* close_system;  //�ر�ϵͳ�Ķ���


	void onHandleItemClick(const QModelIndex& index);
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

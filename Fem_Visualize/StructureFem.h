#pragma once

class NodeFem;
class Element_Base;
class Boundary;
class ForceNode;
class Material;
class Section_Base;
class InVar;

#include <map>
#include <list>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QString>
#include <QTextStream>
#include "Dependant.h"

using namespace Eigen;
using namespace std;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> Tri;//��˵����Ԫ��

class StructureFem
{
public:
	std::map<int, NodeFem*> m_Nodes;
	std::map<int, Element_Base*> m_Elements;
	std::map<int, Boundary*> m_Boundary;
	std::map<int, ForceNode*> m_ForceNode;
	std::map<int, Material*> m_Material;
	std::map<int, Section_Base*> m_Section;
	std::map<int, Dependant*> m_Dependant;
	std::map<int, InVar*> m_InVar;//�����ڱ�����Ϣ

	std::vector<NodeFem*> m_ReationForce;

	VectorXd m_x1, m_x2, m_F1, m_F2, m_R1;

	SpMat m_K11, m_K21, m_K22;

	int m_nFixed;  //Լ�����ɶȸ���
	int m_nTotv;  //�����ɶȸ���

	NodeFem* Find_Node(int idNode);            //�ҽڵ�
	Material* Find_Material(int idMaterial);   //�Ҳ���
	Section_Base* Find_Section(int idSection); //�ҽ���
	Element_Base* Find_Element(int idElement); //�ҵ�Ԫ

	void Input_data(const char* filename);

	bool Input_datas(const QString& FileName);

	bool ReadLine(QTextStream& ssin, QString& str);

	void Init_DOFs();     //����ڵ����ɶ�

	void Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22);       //��װ�ܸپ���

	void Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2);//�γ�x,F

	void Analyse();       //��������

	void Create_Ks();//���ɷֿ�ϡ�����

	void Treat_Fixed();//����Լ������

	void Assemble_Force();//��װ����

	void Solve();//�ⷽ��

	void Show_Solution();//��ʾ���
	
	double Get_Sol(int itotv);//�������ɶȱ�ţ��õ������
	~StructureFem();

};


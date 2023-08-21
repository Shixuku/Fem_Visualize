#pragma once

class NodeFem;
class Element_Base;
class Boundary;
class ForceNode;
class Material;
class Section_Base;


#include <map>
#include <list>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "Dependant.h"

using namespace Eigen;
using namespace std;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> Tri;//��˵����Ԫ��

class StructureFem
{
public:
	std::map<int, NodeFem*> m_Nodes;
	std::map<int, LinkElement_Base*> m_Element;
	std::map<int, Boundary*> m_Boundary;
	std::map<int, ForceNode*> m_ForceNode;
	std::map<int, Material*> m_Material;
	std::map<int, Section_Base*> m_Section;
	std::map<int, Dependant*> m_Dependant;

	std::vector<NodeFem*> m_ReationForce;

	VectorXd m_x1, m_x2;

	int m_nFixed;  //Լ�����ɶȸ���
	int m_nTotal;  //�����ɶȸ���

	NodeFem* Find_Node(int idNode);            //�ҽڵ�
	Material* Find_Material(int idMaterial);   //�Ҳ���
	Section_Base* Find_Section(int idSection); //�ҽ���
	LinkElement_Base* Find_Element(int idElement); //�ҵ�Ԫ

	void Input_data(const char* filename);

	void Init_DOFs();     //����ڵ����ɶ�

	void Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22);       //��װ�ܸپ���

	void Equivalent_Force(); // ���ⲿ���ص�Ч���ڵ���

	void Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2);//�γ�x,F

	void Analyse();       //��������

	~StructureFem();

};


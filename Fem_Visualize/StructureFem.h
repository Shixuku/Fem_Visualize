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
typedef Eigen::Triplet<double> Tri;//重说明三元组

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

	int m_nFixed;  //约束自由度个数
	int m_nTotal;  //总自由度个数

	NodeFem* Find_Node(int idNode);            //找节点
	Material* Find_Material(int idMaterial);   //找材料
	Section_Base* Find_Section(int idSection); //找截面
	LinkElement_Base* Find_Element(int idElement); //找单元

	void Input_data(const char* filename);

	void Init_DOFs();     //分配节点自由度

	void Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22);       //组装总纲矩阵

	void Equivalent_Force(); // 将外部荷载等效到节点上

	void Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2);//形成x,F

	void Analyse();       //分析计算

	~StructureFem();

};


#pragma once

class NodeFem;
class Element_Base;
class Boundary;
class ForceNode;
class Material;
class Section_Base;
class InVar;
class Optimisation;

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
typedef Eigen::Triplet<double> Tri;//重说明三元组

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
	std::map<int, InVar*> m_InVar;//保存内变量信息
	std::map<int, Optimisation*> m_Optims;//保存内变量信息

	std::vector<NodeFem*> m_ReationForce;

	VectorXd m_x1, m_x2, m_x3, m_F1, m_F2, m_F3, m_R1;

	SpMat m_K11, m_K21, m_K22, m_K31, m_K32, m_K33;

	int m_nFixed;  //约束自由度个数
	int m_nShousuo; // 非弹性收缩量自由度个数
	int m_nOptimis; // 需要优化的自由度个数
	int m_UnOptimis; //无需优化的自由度个数
	int m_nTotv;  //总自由度个数

	NodeFem* Find_Node(int idNode);            //找节点
	Material* Find_Material(int idMaterial);   //找材料
	Section_Base* Find_Section(int idSection); //找截面
	Element_Base* Find_Element(int idElement); //找单元

	void Input_data(const char* filename);

	bool Input_datas(const QString& FileName);

	bool ReadLine(QTextStream& ssin, QString& str);

	void Init_DOFs();     //分配节点自由度
	void Init_DOFs2();     //分配节点自由度

	void Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22);       //组装总纲矩阵

	void Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2);//形成x,F

	void Analyse();       //分析计算
	void Analyse2();       //分析计算

	void Create_Ks();//生成分块稀疏矩阵

	void Create_Kss();//生成分块稀疏矩阵


	void Treat_Fixed();//处理约束条件

	void Assemble_Force();//组装荷载

	void Assemble_Force2();//组装荷载

	void Solve();//解方程
	void Solve2();//解方程

	void Show_Solution();//显示结果
	void Show_Solution2();//显示结果
	
	double Get_Sol(int itotv);//根据自由度编号，得到求解结果
	double Get_Sol2(int itotv);//根据自由度编号，得到求解结果
	~StructureFem();

};


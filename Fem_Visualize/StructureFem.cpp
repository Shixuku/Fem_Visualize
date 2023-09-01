#include "StructureFem.h"
#include "NodeFem.h"
#include "LinkElement_Truss3D.h"
#include "LinkElement_Beam3D.h"
#include "Element_Base.h"
#include "SoildElement_Tri2DS.h"
#include "SoildElement_Terach3D.h"
#include "SoildElement_Brick3D.h"
#include "Boundary.h"
#include "ForceNode.h"
#include "Material.h"
#include "Section_Truss3D.h"
#include "Section_Beam3D.h"
#include "SoildSection_Base.h"
#include "Dependant.h"

#include <fstream>
#include <iostream>
#include <qfile.h>
#include <QDebug>
#include <QRegularExpression>

# pragma execution_character_set("utf-8")

NodeFem* StructureFem::Find_Node(int idNode)
{//�ҽڵ�
	auto iFind = m_Nodes.find(idNode);
	if (iFind != m_Nodes.end())
	{//�ҵ���
		return iFind->second;
	}
	std::cout << "Node not found!\n";
	exit(1);
	return nullptr;
}

Material* StructureFem::Find_Material(int idMaterial)
{//�Ҳ���
	auto iFind = m_Material.find(idMaterial);
	if (iFind != m_Material.end())
	{//�ҵ���
		return iFind->second;
	}
	std::cout << "Material not found!\n";
	exit(1);
	return nullptr;
}

Section_Base* StructureFem::Find_Section(int idSection)
{//�ҽ���
	auto iFind = m_Section.find(idSection);
	if (iFind != m_Section.end())
	{//�ҵ���
		return iFind->second;
	}
	std::cout << "Section not found!\n";
	exit(1);
	return nullptr;
}
Element_Base* StructureFem::Find_Element(int idElement)
{//�ҵ�Ԫ
	auto iFind = m_Elements.find(idElement);
	if (iFind != m_Elements.end())
	{//�ҵ���
		return iFind->second;
	}
	std::cout << "Element not found!\n";
	exit(1);
	return nullptr;
}

void StructureFem::Input_data(const char* filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		std::cout << "file open failed\n";
		exit(1);
	}
	else
	{
		std::cout << "read start\n";
	}
	std::cout << "\n";

	//���ڵ�
	int nNode = 0;
	fin >> nNode;
	std::cout << "Node:\n";
	for (int i = 0; i < nNode; ++i)
	{
		NodeFem* pNode = new NodeFem;
		fin >> pNode->m_id >> pNode->m_x >> pNode->m_y>>pNode->m_z;
		std::cout << pNode->m_id << " " << pNode->m_x << " " << pNode->m_y << " " << pNode->m_z << "\n";
		m_Nodes.insert({ pNode->m_id , pNode });
	}
	std::cout << "\n";

	//����ά��ܵ�Ԫ
	int nTruss3D = 0;
	fin >> nTruss3D;
	if (nTruss3D > 0)
	{
		std::cout << "Element_Truss3D:\n";
	}
	for (int i = 0; i < nTruss3D; ++i)
	{
		LinkElement_Truss3D* pTruss = new LinkElement_Truss3D;
		pTruss->Input_Data(fin);
		pTruss->Disp();
		m_Elements.insert({ pTruss->m_id ,pTruss });
	}
	std::cout << "\n";

	//����ά����Ԫ
	int nBeam3D = 0;
	fin >> nBeam3D;
	if (nBeam3D > 0)
	{
		std::cout << "Element_Beam3D:\n";
	}
	for (int i = 0; i < nBeam3D; i++)
	{
		LinkElement_Beam3D* pElement = new LinkElement_Beam3D;
		pElement->Input_Data(fin);
		pElement->Disp();
		m_Elements.insert({ pElement->m_id ,pElement });
	}

	// ����άƽ�浥Ԫ
	int nTriangle2D = 0;
	fin >> nTriangle2D;
	if (nTriangle2D > 0)
	{
		std::cout << "SoildElement_Tri2DS:\n";
	}
	for (int i = 0; i < nTriangle2D; i++)
	{
		SoildElement_Tri2DS* pElement = new SoildElement_Tri2DS;
		pElement->Input_Data(fin);
		pElement->Disp();
		m_Elements.insert({ pElement->m_id ,pElement });
	}
	
	// ���ռ������嵥Ԫ
	int nTerach3D = 0;
	fin >> nTerach3D;
	if (nTerach3D > 0)
	{
		std::cout << "SoildElement_Tri2DS:\n";
	}
	for (int i = 0; i < nTerach3D; i++)
	{
		SoildElement_Terach3D* pElement = new SoildElement_Terach3D;
		pElement->Input_Data(fin);
		pElement->Disp();
		m_Elements.insert({ pElement->m_id ,pElement });
	}

	//�����ӹ�ϵ��Ϣ
	int nDependant = 0;
	fin >> nDependant;
	if (nDependant>0)
	{
		std::cout << "Dependant:\n";
	}
	for (int i = 0; i < nDependant; ++i)
	{
		Dependant* pDependant = new Dependant;
		fin >> pDependant->m_id;
		fin >> pDependant->m_idNode;
		fin >> pDependant->m_idHost[0];
		fin >> pDependant->m_idHost[1];
		fin >> pDependant->m_idHost[2];
		fin >> pDependant->m_idHost[3];
		fin >> pDependant->m_idHost[4];
		fin >> pDependant->m_idHost[5];
		m_Dependant.insert({ pDependant->m_id ,pDependant });
	}
	std::cout << "\n";

	//������
	int nMaterial = 0;
	fin >> nMaterial;
	std::cout << "Material:\n";
	for (int i = 0; i < nMaterial; i++)
	{
		Material* pMaterial = new Material;
		fin >> pMaterial->m_id >> pMaterial->m_E >> pMaterial->m_v;
		pMaterial->Disp();
		m_Material.insert({ pMaterial->m_id,pMaterial });
	}
	std::cout << "\n";

	//����ܽ���
	int nSection_Truss = 0;
	fin >> nSection_Truss;
	if (nSection_Truss>0)
	{
		std::cout << "Section_Truss:\n";
	}
	for (int i = 0; i < nSection_Truss; ++i)
	{
		Section_Truss3D* pSection = new Section_Truss3D;
		fin >> pSection->m_id >> pSection->m_idMaterial >> pSection->m_Area;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";

	//��������
	int nSectio_Beam3D = 0;
	fin >> nSectio_Beam3D;
	if (nSectio_Beam3D > 0)
	{
		std::cout << "Sectio_Beam3D:\n";
	}
	for (int i = 0; i < nSectio_Beam3D; ++i)
	{
		Section_Beam3D* pSection = new Section_Beam3D;
		fin >> pSection->m_id >> pSection->m_idMaterial;
		fin >> pSection->m_Area >> pSection->m_Iz >> pSection->m_Iy;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";

	//�����ǽ���
	int nSectio_Tri = 0;
	fin >> nSectio_Tri;
	if (nSectio_Tri > 0)
	{
		std::cout << "Section_Triangle:\n";
	}
	for (int i = 0; i < nSectio_Tri; ++i)
	{
		SoildSection_Base* pSection = new SoildSection_Base;
		fin >> pSection->m_id >> pSection->m_idMaterial;
		fin >> pSection->m_t;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";


	//�����������
	int nSectio_Ter = 0;
	fin >> nSectio_Ter;
	if (nSectio_Ter > 0)
	{
		std::cout << "Sectio_Terach3D:\n";
	}
	for (int i = 0; i < nSectio_Ter; ++i)
	{
		SoildSection_Base* pSection = new SoildSection_Base;
		fin >> pSection->m_id >> pSection->m_idMaterial;
		pSection->Disp();
		m_Section.insert({ pSection->m_id ,pSection });
	}
	std::cout << "\n";

	//��ָ�ɵ�Ԫ������Ϣ
	int nElement = m_Elements.size();
	for (int i = 0; i < nElement; ++i)
	{
		int idElement, idSection;
		fin >> idElement >> idSection;
		Element_Base* pElement = Find_Element(idElement);
		pElement->m_idSection = idSection;
	}
	std::cout << "Assign_Sections:\n";
	for (auto& a : m_Elements)
	{
		a.second->Disp();
	}
	std::cout << "\n";

	//���߽�����
	int nBoundary = 0;
	fin >> nBoundary;
	std::cout << "Boundary:\n";
	for (int i = 0; i < nBoundary; ++i)
	{
		Boundary* pBoundary = new Boundary;
		fin >> pBoundary->m_id >> pBoundary->m_idNode;
		fin >> pBoundary->m_ixyz >> pBoundary->m_value;
		pBoundary->Disp();
		m_Boundary.insert({ pBoundary->m_id ,pBoundary });
		NodeFem* pNode = Find_Node(pBoundary->m_idNode);
		pNode->boundaryFlag = true;
	}
	std::cout << "\n";

	//������
	int nForce = 0;
	fin >> nForce;
	std::cout << "Force:\n";
	for (int i = 0; i < nForce; ++i)
	{
		ForceNode* pForce = new ForceNode;
		fin >> pForce->m_id >> pForce->m_idNode;
		fin >> pForce->m_ixyz >> pForce->m_value;
		pForce->Disp();
		m_ForceNode.insert({ pForce->m_id ,pForce });
	}
	std::cout << "\n";

	fin.close();

	std::cout << "read completed\n";
}


bool StructureFem::Input_datas(const QString& FileName)
{
	if (!QFile::exists(FileName))
	{//�ļ�������
		qDebug() << "Error: �ļ� " << FileName << " ������";
		exit(1);
		return false;
	}
	StructureFem* pStructure = this;

	QFile aFile(FileName);//�����ļ�
	aFile.open(QIODevice::ReadOnly);//���ļ����ж���
	QTextStream ssin(&aFile);//���ļ���
	QString strdata;

	while (ReadLine(ssin, strdata))
	{//��ȡ����Ч�����У������strdata��
		QStringList list_str = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
		if (list_str[0].compare("*Node", Qt::CaseInsensitive) == 0)
		{//������Ϊ�ڵ�ؼ�����
			Q_ASSERT(list_str.size() == 2);
			int nNode = list_str[1].toInt();//�õ��ڵ�������ɿ��ƺ���ѭ������
			qDebug() << "\n�ڵ���: " << nNode;
			for (int i = 0; i < nNode; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �ڵ����ݲ���";
					exit(1);
				}
				QStringList strlist_node = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_node.size() == 4);
				int idNode = strlist_node[0].toInt();//ȡ�ýڵ��
				double xi = strlist_node[1].toDouble();
				double yi = strlist_node[2].toDouble();
				double zi = strlist_node[3].toDouble();
				qDebug() << idNode << "  " << xi << " " << yi << " " << zi;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_Nodes.insert(make_pair(idNode, new NodeFem(idNode, xi, yi, zi)));
			}
		}

		else if (list_str[0].compare("*Element_Truss3D", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ���ܵ�Ԫ
			Q_ASSERT(list_str.size() == 2);
			int nEle = list_str[1].toInt();//�õ���Ԫ�������ɿ��ƺ���ѭ������

			qDebug() << "\n�ռ���ܵ�Ԫ��: " << nEle;
			for (int i = 0; i < nEle; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: ��Ԫ���ݲ���";
					exit(1);
				}
				QStringList strlist_ele = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_ele.size() == 3);
				int idEle = strlist_ele[0].toInt();//ȡ�õ�Ԫ��
				int idNode1 = strlist_ele[1].toInt();
				int idNode2 = strlist_ele[2].toInt();
				qDebug() << idEle << "  " << idNode1 << " " << idNode2;//������Ա���
				//���浽ģ�����ݿ�

				pStructure->m_Elements.insert(make_pair(idEle, new LinkElement_Truss3D(idEle, idNode1, idNode2)));
			}
		}

		else if (list_str[0].compare("*Element_Beam3D", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ�����Ԫ
			Q_ASSERT(list_str.size() == 2);
			int nEle = list_str[1].toInt();//�õ���Ԫ�������ɿ��ƺ���ѭ������

			qDebug() << "\n�ռ�����Ԫ��: " << nEle;
			for (int i = 0; i < nEle; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: ����Ԫ���ݲ���";
					exit(1);
				}
				QStringList strlist_ele = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_ele.size() == 3);
				int idEle = strlist_ele[0].toInt();//ȡ�õ�Ԫ��
				int idNode1 = strlist_ele[1].toInt();
				int idNode2 = strlist_ele[2].toInt();
				qDebug() << idEle << "  " << idNode1 << " " << idNode2;//������Ա���

				pStructure->m_Elements.insert(make_pair(idEle, new LinkElement_Beam3D(idEle, idNode1, idNode2)));
			}
		}

		else if (list_str[0].compare("*Tetrahedral_Element", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ������嵥Ԫ
			Q_ASSERT(list_str.size() == 2);
			int nEle = list_str[1].toInt();//�õ���Ԫ�������ɿ��ƺ���ѭ������

			qDebug() << "\n�ռ������嵥Ԫ��: " << nEle;
			for (int i = 0; i < nEle; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �ռ������嵥Ԫ���ݲ���";
					exit(1);
				}
				QStringList strlist_ele = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_ele.size() == 5);
				int idEle = strlist_ele[0].toInt();//ȡ�õ�Ԫ��
				int idNode1 = strlist_ele[1].toInt();
				int idNode2 = strlist_ele[2].toInt();
				int idNode3 = strlist_ele[3].toInt();
				int idNode4 = strlist_ele[4].toInt();
				qDebug() << idEle << "  " << idNode1 << " " << idNode2 << " " << idNode3 << " " << idNode4;

				pStructure->m_Elements.insert(make_pair(idEle, new SoildElement_Terach3D(idEle, idNode1, idNode2, idNode3, idNode4)));
			}
		}

		else if (list_str[0].compare("*Brick_Element", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ�˽ڵ������嵥Ԫ
			Q_ASSERT(list_str.size() == 2);
			int nEle = list_str[1].toInt();//�õ���Ԫ�������ɿ��ƺ���ѭ������

			qDebug() << "\n�ռ������嵥Ԫ��: " << nEle;
			for (int i = 0; i < nEle; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �ռ������嵥Ԫ���ݲ���";
					exit(1);
				}
				QStringList strlist_ele = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_ele.size() == 9);
				int idEle = strlist_ele[0].toInt();//ȡ�õ�Ԫ��
				int idNode1 = strlist_ele[1].toInt();
				int idNode2 = strlist_ele[2].toInt();
				int idNode3 = strlist_ele[3].toInt();
				int idNode4 = strlist_ele[4].toInt();
				int idNode5 = strlist_ele[5].toInt();
				int idNode6 = strlist_ele[6].toInt();
				int idNode7 = strlist_ele[7].toInt();
				int idNode8 = strlist_ele[8].toInt();
				std::vector<int> idNodes;
				for (int j = 0; j < 8; j++)
				{
					idNodes.push_back(strlist_ele[j + 1].toInt());
				}
				qDebug() << idEle << "  " << idNode1 << " " << idNode2 << " " << idNode3 << " " << idNode4
					<<" " << idNode5<< " " << idNode6 << " " << idNode7 << " " << idNode8;
				pStructure->m_Elements.insert(make_pair(idEle, new SoildElement_Brick3D(idEle, idNodes)));
			}
		}

		else if (list_str[0].compare("*Material", Qt::CaseInsensitive) == 0)
		{//��ȡ������
			Q_ASSERT(list_str.size() == 2);
			int nMat = list_str[1].toInt();//�õ����ϸ������ɿ��ƺ���ѭ������
			qDebug() << "\n������: " << nMat;
			for (int i = 0; i < nMat; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �������ݲ���";
					exit(1);
				}
				QStringList strlist_mat = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_mat.size() == 4);
				int idMat = strlist_mat[0].toInt();
				double elasticity = strlist_mat[1].toDouble();
				double poisson = strlist_mat[2].toDouble();
				double density = strlist_mat[3].toDouble();
				qDebug() << idMat << "  " << elasticity << " " << poisson << " " << density;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_Material.insert(make_pair(idMat, new Material(idMat, elasticity, poisson, density)));
			}
		}
		else if (list_str[0].compare("*Section_Truss3D", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ�˽���
			Q_ASSERT(list_str.size() == 2);
			int nSecT = list_str[1].toInt();//�õ��˽������������ɿ��ƺ���ѭ������
			qDebug() << "\n�˽�����: " << nSecT;
			for (int i = 0; i < nSecT; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �˽������ݲ���";
					exit(1);
				}
				QStringList strlist_secT = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_secT.size() == 3);
				int idSecT = strlist_secT[0].toInt();
				int idMat = strlist_secT[1].toInt();
				double area = strlist_secT[2].toDouble();
				qDebug() << idSecT << "  " << idMat << " " << area;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_Section.insert(make_pair(idSecT, new Section_Truss3D(idSecT, idMat, area)));
			}
		}

		else if (list_str[0].compare("*Section_Beam3D", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ�������
			Q_ASSERT(list_str.size() == 2);
			int nSecB = list_str[1].toInt();//�õ����������������ɿ��ƺ���ѭ������
			qDebug() << "\n��������: " << nSecB;
			for (int i = 0; i < nSecB; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: ���������ݲ���";
					exit(1);
				}
				QStringList strlist_secB = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_secB.size() == 5);
				int idSecT = strlist_secB[0].toInt();
				int idMat = strlist_secB[1].toInt();
				double area = strlist_secB[2].toDouble();
				double Iy = strlist_secB[3].toDouble();
				double Iz = strlist_secB[4].toDouble();
				qDebug() << idSecT << "  " << idMat << " " << area << Iy << " " << Iz;//������Ա���
				//���浽ģ�����ݿ�
				Section_Beam3D* beam = new Section_Beam3D(idSecT, idMat, area, Iy, Iz);
				pStructure->m_Section.insert(make_pair(idSecT, beam));
			}
		}

		else if (list_str[0].compare("*Section_SoildElement3D", Qt::CaseInsensitive) == 0)
		{//��ȡ���ռ�������
			Q_ASSERT(list_str.size() == 2);
			int nSecB = list_str[1].toInt();//�õ����������������ɿ��ƺ���ѭ������
			qDebug() << "\n�ռ������������: " << nSecB;
			for (int i = 0; i < nSecB; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: �ռ����������ݲ���";
					exit(1);
				}
				QStringList strlist_secB = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_secB.size() == 2);
				int idSecT = strlist_secB[0].toInt();
				int idMat = strlist_secB[1].toInt();
				qDebug() << idSecT << "  " << idMat;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_Section.insert(make_pair(idSecT, new SoildSection_Base(idSecT, idMat)));
			}
			}

		else if (list_str[0].compare("*Section_Assign", Qt::CaseInsensitive) == 0)
		{//��ȡ������ָ��
			Q_ASSERT(list_str.size() == 2);
			int nSec = list_str[1].toInt();//�õ�����ָ�ɸ������ɿ��ƺ���ѭ������
			qDebug() << "\n����ָ����: " << nSec;
			for (int i = 0; i < nSec; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error: ���������ݲ���";
					exit(1);
				}
				QStringList strlist_sec = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_sec.size() == 2);
				int idElement = strlist_sec[0].toInt();
				int	idSection = strlist_sec[1].toInt();
				Element_Base* pElement = Find_Element(idElement);
				pElement->m_idSection = idSection;
			}
		}

		else if (list_str[0].compare("*Constraint", Qt::CaseInsensitive) == 0)
		{//��ȡ��Լ��
			Q_ASSERT(list_str.size() == 2);
			int nConstraint = list_str[1].toInt();//�õ�Լ���������ɿ��ƺ���ѭ������
			qDebug() << "\nԼ������: " << nConstraint;
			for (int i = 0; i < nConstraint; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error:Լ�����ݲ���";
					exit(1);
				}
				QStringList strlist_Constraint = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_Constraint.size() == 4);
				int id = strlist_Constraint[0].toInt();
				int idNode = strlist_Constraint[1].toInt();
				int direction = strlist_Constraint[2].toInt();
				double P = strlist_Constraint[3].toDouble();
				qDebug() << id << "  " << idNode << "  " << "  " << direction << "  " << P;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_Boundary.insert(make_pair(id, new Boundary(id, idNode, direction, P)));
				NodeFem* pNode = Find_Node(idNode);
				pNode->boundaryFlag = true;
			}
		}

		else if (list_str[0].compare("*Force_Node_Function", Qt::CaseInsensitive) == 0)
		{//��ȡ���ڵ����
			Q_ASSERT(list_str.size() == 2);
			int nForce_Node = list_str[1].toInt();//�õ������ظ������ɿ��ƺ���ѭ������
			qDebug() << "\n��������: " << nForce_Node;
			for (int i = 0; i < nForce_Node; ++i)
			{
				//������һ����Ч����
				if (!ReadLine(ssin, strdata))
				{//û�ж�ȡ����Ч���ݣ��˳�
					qDebug() << "Error:�ڵ�������ݲ���";
					exit(1);
				}
				QStringList strlist_Force_Node = strdata.split(QRegularExpression("[\t, ]"), Qt::SkipEmptyParts);//���ÿո�,�ֽ��ַ���
				Q_ASSERT(strlist_Force_Node.size() == 4);
				int idForce = strlist_Force_Node[0].toInt();
				int idNode = strlist_Force_Node[1].toInt();
				int direction = strlist_Force_Node[2].toInt();
				double expression = strlist_Force_Node[3].toDouble();
				qDebug() << idForce << "  " << idNode << "  " << direction << "  " << expression;//������Ա���
				//���浽ģ�����ݿ�
				pStructure->m_ForceNode.insert(make_pair(idForce, new ForceNode(idForce, idNode, direction, expression)));
			}
		}
	}
}

bool StructureFem::ReadLine(QTextStream& ssin, QString& str)
{
	while (!ssin.atEnd())
	{
		str = ssin.readLine();
		if (str.left(2).compare("**") == 0) continue;//ע���У�������ȡ��һ��

		str = str.trimmed();//ȥ����ͷ��ĩβ�Ŀո�
		if (str.isEmpty()) continue;//ע���У�������ȡ��һ��

		return true;//��ȡ����Ч���ݣ����һ�ж�ȡ
	}

	return false;//�ļ����꣬û�еõ���Ч��������
}

void StructureFem::Init_DOFs()
{//����ڵ����ɶ�
	for (auto& a : m_Elements)
	{//����ÿ���ڵ�����ɶȸ���
		Element_Base* pElement = a.second;
		pElement->Set_NodeDOF();
	}
	for (auto& a : m_Nodes)
	{//�Խڵ�ѭ��
		NodeFem* pNode = a.second;
		for (auto& dof : pNode->m_DOF)
		{//��ÿ�����ɶ�ѭ��
			dof = -1; //��ʼ��ÿ���ڵ�����ɶȱ�ž�Ϊ-1
		}
	}

	auto iStart = 0;
	for (auto& a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		NodeFem* pNode = Find_Node(pBoundary->m_idNode);
		int it = pBoundary->m_ixyz;    //Լ�������ɶȵ���ţ�0��x��1��y
		pNode->m_DOF[it] = iStart;
		++iStart;
	}

	m_nFixed = iStart;//Լ�����ɶȸ���
	std::cout << "\nm_nFixed=" << m_nFixed << "\n";

	for (auto& a : m_Dependant)
	{//����������Ϣ
		Dependant* pDependant = a.second;
		pDependant->Set_DOF(iStart);
	}

	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		for (auto& dof : pNode->m_DOF)
		{
			if (dof == -1) dof = iStart++;
		}
	}
	m_nTotal = iStart;//�����ɶȸ���

	std::cout << "\nnode DOF:\n";
	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		std::cout << pNode->m_id << " ";
		for (auto& dof : pNode->m_DOF)
		{
			std::cout << dof << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

}

void StructureFem::Assemble_K(SpMat& K11, SpMat& K21, SpMat& K22)
{
	int nFree = m_nTotal - m_nFixed;

	K11.resize(m_nFixed, m_nFixed);
	K21.resize(nFree, m_nFixed);
	K22.resize(nFree, nFree);

	std::list<Tri> list_K11, list_K21, list_K22;

	for (auto& a : m_Elements)
	{
		Element_Base* pElement = a.second;
		pElement->calculate_all();
		pElement->Assemble_ke(list_K11, list_K21, list_K22);	
	}

	/*std::cout << "\nlist_K21:\n";
	for (auto& a : list_K21)
	{
		std::cout << a.row() << "," << a.col() << "," << a.value() << "\n";
	}*/

	K11.setFromTriplets(list_K11.begin(), list_K11.end());
	K21.setFromTriplets(list_K21.begin(), list_K21.end());
	K22.setFromTriplets(list_K22.begin(), list_K22.end());
	
	/*MatrixXd M11 = K11; MatrixXd M21 = K21; MatrixXd M22 = K22;

	std::cout << "M11��" << "\n" << M11 << "\n";
	std::cout << "M21��" << "\n" << M21 << "\n";
	std::cout << "M22��" << "\n" << M22 << "\n";*/
}

void StructureFem::Assemble_xF(VectorXd& x1, VectorXd& F1, VectorXd& F2)
{
	x1.resize(m_nFixed);
	// ���̶�Լ��������������
	for (auto& a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		pBoundary->Set_x1(x1);
	}

	F1.resize(m_nFixed);
	int nFree = m_nTotal - m_nFixed;
	F2.resize(nFree);
	F1.setZero();
	F2.setZero();
	for (auto& a : m_ForceNode)
	{
		ForceNode* pForce = a.second;
		pForce->Assemble_F1_F2(F1, F2);
	}
	
}

void StructureFem::Analyse()
{
	Init_DOFs();//����ڵ����ɶ�

	SpMat K11, K21, K22;
	Assemble_K(K11, K21, K22);//��װ�ܸپ���
	MatrixXd M11 = K11; MatrixXd M21 = K21; MatrixXd M22 = K22;

	//std::cout << "M11(K11)��" << "\n" << M11 << "\n";
	//std::cout << "M21(K21)��" << "\n" << M21 << "\n";
	//std::cout << "M22(K22)��" << "\n" << M22 << "\n";

	VectorXd x1, F1, F2;
	Assemble_xF(x1, F1, F2);

	std::cout << "\nF1=\n" << F1 << "\n";
	std::cout << "\nF2=\n" << F2 << "\n";
	
	
	SimplicialLDLT<SparseMatrix<double>> solver;
	solver.analyzePattern(K22); //��������Ԫ�ؽṹ
	solver.factorize(K22);      //�ֽ�
	
	VectorXd x2 = solver.solve(F2 - K21 * x1);
	VectorXd R1 = K11 * x1 + K21.transpose() * x2 - F1;


	VectorXd R2 = K11 * x1 + K21.transpose() * x2;

	// ��֧��������ֵ����Ӧ�ĵ�Լ����
	for (auto &a : m_Boundary)
	{
		Boundary* pBoundary = a.second;
		NodeFem* NodeFem = Find_Node(pBoundary->m_idNode);
		NodeFem->m_ReactionForce[pBoundary->m_ixyz] = R1(pBoundary->m_id - 1);
	}

	for (auto& a : m_Nodes)
	{
		if (a.second->boundaryFlag)
		{
			m_ReationForce.push_back(a.second);
		}
	}

	m_x1 = x1;
	m_x2 = x2;

	std::cout << "\nx1=\n" << m_x1 << "\n";
	std::cout << "\nx2=\n" << m_x2 << "\n";
	std::cout << "\nR1=\n" << R1 << "\n"<<"\n";

	for (auto& a : m_Nodes)
	{
		NodeFem* pNode = a.second;
		double dis = 0;
		int i = 0;
		std::cout << "\n";
		std::cout << "node" << pNode->m_id  << ":\n";
		for (auto& dof : pNode->m_DOF)
		{
			if (dof < m_nFixed)
			{
				dis = x1[dof];
				std::cout << x1[dof] << " ";
			}
			else
			{
				dis = x2[dof - m_nFixed];
				std::cout << x2[dof - m_nFixed] << " ";
			}
			pNode->m_Displacement[i] = dis;
			i++;
			std::cout << "\n";
		}
	}

	for (auto& a : m_Elements)
	{
		Element_Base* element = a.second;
		Eigen::VectorXd combinedDisp;
		int dofPerNode = element->Get_DOF_Node(); // Assuming this returns the DOF per node
		int dofNum = element->m_idNode.size() * dofPerNode;
		combinedDisp.resize(dofNum);

		SoildElement_Base* soildElement = dynamic_cast<SoildElement_Base*>(element);
		LinkElement_Beam3D* beamElement = dynamic_cast<LinkElement_Beam3D*>(element);
		LinkElement_Truss3D* trussElement = dynamic_cast<LinkElement_Truss3D*>(element);

		if (soildElement) 
		{
			soildElement->calculate_Stress(combinedDisp);
		}
		else if (beamElement)
		{
			for (int i = 0; i < a.second->m_idNode.size(); i++)
			{
				NodeFem* node = Find_Node(a.second->m_idNode[i]);
				combinedDisp.segment(i * dofPerNode, dofPerNode) = node->m_Displacement;
			}
			beamElement->calculate_internal_force(combinedDisp);
		}
		else if (trussElement)
		{
			for (int i = 0; i < a.second->m_idNode.size(); i++)
			{
				NodeFem* node = Find_Node(a.second->m_idNode[i]);
				node->m_Displacement.resize(3);
				combinedDisp.segment(i * dofPerNode, dofPerNode) = node->m_Displacement;
			}
			trussElement->calculate_internal_force(combinedDisp);
		}
			
	}
}

StructureFem::~StructureFem()
{
	std::cout << "\nClean memory\n";
	for (auto& a : m_Nodes)
	{
		delete a.second;
	}
	for (auto& a : m_Elements)
	{
		delete a.second;
	}
	for (auto& a : m_Boundary)
	{
		delete a.second;
	}
	for (auto& a : m_ForceNode)
	{
		delete a.second;
	}

}

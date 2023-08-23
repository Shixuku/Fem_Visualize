#include "SoildElement_Tri2DS.h"
#include "StructureFem.h"
#include "Material.h"
#include "SoildSection_Base.h"

void SoildElement_Tri2DS::calculate_D()
{
	StructureFem* pst = Get_Structure();
	Section_Base* pSection = pst->Find_Section(m_idSection);

	double E, nu;
	pSection->Get_Ev(E, nu);

	m_D.resize(3, 3);
	double a = E / (1 - nu * nu);
	m_D << 1,    nu,    0.,
		   nu,   1.0,   0.,
		   0.,   0.,   (1 - nu) / 2.0;
	m_D = a * m_D;
}

void SoildElement_Tri2DS::calculate_B()
{
	NodeFem* node0 = Get_Structure()->Find_Node(m_idNode[0]);
	NodeFem* node1 = Get_Structure()->Find_Node(m_idNode[1]);
	NodeFem* node2 = Get_Structure()->Find_Node(m_idNode[2]);

	double belta0 = node1->m_y - node2->m_y;
	double belta1 = node2->m_y - node0->m_y;
	double belta2 = node0->m_y - node1->m_y;

	double gama0 = node1->m_x - node2->m_x;
	double gama1 = node2->m_x - node0->m_x;
	double gama2 = node0->m_x - node1->m_x;

	m_B.resize(3, 6);

	m_B << belta0, 0,       belta1, 0,       belta2, 0,
		   0,      gama0,   0,      gama1,   0,      gama2,
		   gama0,  belta0,  gama1,  belta1,  gama2,  belta2;
	m_B = 1.0 / (2 * m_volume) * m_B;
}

void SoildElement_Tri2DS::calculate_volume()
{

	NodeFem* node0 = Get_Structure()->Find_Node(m_idNode[0]);
	NodeFem* node1 = Get_Structure()->Find_Node(m_idNode[1]);
	NodeFem* node2 = Get_Structure()->Find_Node(m_idNode[2]);

	double x0 = node0->m_x;
	double y0 = node0->m_y;
	double x1 = node1->m_x;
	double y1 = node1->m_y;
	double x2 = node2->m_x;
	double y2 = node2->m_y;

	double A = std::abs(x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1)) / 2.0;
	m_volume = A;
}

void SoildElement_Tri2DS::calculate_all()
{
	calculate_volume();
	calculate_B();
	calculate_D();
	calculate_ke();
}

void SoildElement_Tri2DS::calculate_ke()
{
	StructureFem* pst = Get_Structure();

	Section_Base* pSection = pst->Find_Section(m_idSection);
	SoildSection_Base* pSectionTir = dynamic_cast<SoildSection_Base*>(pSection);

	m_ke = pSectionTir->m_t * m_volume * m_B.transpose() * m_D * m_B;
}

void SoildElement_Tri2DS::Disp()
{
	std::cout << m_id << " " << m_idSection << " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}

	std::cout << "\n";
}

void SoildElement_Tri2DS::Input_Data(std::ifstream& fin)
{

	fin >> m_id;
	fin >> m_idSection;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
}

int SoildElement_Tri2DS::Get_DOF_Node()
{
	m_dof = 2;
	return 2;
}

SoildElement_Tri2DS::SoildElement_Tri2DS()
{
	m_idNode.resize(3);
}

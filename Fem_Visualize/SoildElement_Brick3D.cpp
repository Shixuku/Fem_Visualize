#include "SoildElement_Brick3D.h"
#include "Section_Base.h"
#include "StructureFem.h"
#include "NodeFem.h"

void SoildElement_Brick3D::calculate_D()
{
	StructureFem* pst = Get_Structure();
	Section_Base* pSection = pst->Find_Section(m_idSection);

	double E, nu;
	//pSection->Get_Ev(E, nu);
	E = 2.1e8;
	nu = 0.3;

	double a = E / ((1 + nu) * (1 - 2 * nu));

	Eigen::MatrixXd D(6, 6);
	D << a * (1 - nu), a* nu, a* nu, 0, 0, 0,
		a* nu, a* (1 - nu), a* nu, 0, 0, 0,
		a* nu, a* nu, a* (1 - nu), 0, 0, 0,
		0, 0, 0, a* (1 - nu) / 2, 0, 0,
		0, 0, 0, 0, a* (1 - nu) / 2, 0,
		0, 0, 0, 0, 0, a* (1 - nu) / 2;

	m_D = D;
}

void SoildElement_Brick3D::calculate_B()
{

}

std::tuple <Eigen::MatrixXd, double> SoildElement_Brick3D::calculate_B(Eigen::VectorXd& x)
{
	StructureFem* pst = Get_Structure();
	double s = x[0];
	double t = x[1];
	double u = x[2];

	std::vector<double> xs, ys, zs;

	for (const auto& nd : pst->m_Nodes) {
		NodeFem* node = nd.second;
		xs.push_back(node->m_x);
		ys.push_back(node->m_y);
		zs.push_back(node->m_z);
	}

	// Ns, Nt, Nu calculations
	std::vector<double> Ns, Nt, Nu;
	Ns.push_back((t - 1) * (1 + u) / 8.);
	Ns.push_back((t - 1) * (1 - u) / 8.);
	Ns.push_back((t + 1) * (u - 1) / 8.);
	Ns.push_back((1 + t) * (-u - 1) / 8.);
	Ns.push_back((1 - t) * (1 + u) / 8.);
	Ns.push_back((1 - t) * (1 - u) / 8.);
	Ns.push_back((1 + t) * (1 - u) / 8.);
	Ns.push_back((1 + t) * (1 + u) / 8.);

	Nt.push_back((s - 1) * (1 + u) / 8.);
	Nt.push_back((s - 1) * (1 - u) / 8.);
	Nt.push_back((1 - s) * (1 - u) / 8.);
	Nt.push_back((1 - s) * (1 + u) / 8.);
	Nt.push_back((-s - 1) * (1 + u) / 8.);
	Nt.push_back((-1 - s) * (1 - u) / 8.);
	Nt.push_back((1 + s) * (1 - u) / 8.);
	Nt.push_back((1 + s) * (1 + u) / 8.);

	Nu.push_back((1 - s) * (1 - t) / 8.);
	Nu.push_back((s - 1) * (1 - t) / 8.);
	Nu.push_back((s - 1) * (1 + t) / 8.);
	Nu.push_back((1 - s) * (1 + t) / 8.);
	Nu.push_back((1 + s) * (1 - t) / 8.);
	Nu.push_back((1 + s) * (t - 1) / 8.);
	Nu.push_back((-1 - s) * (t + 1) / 8.);
	Nu.push_back((1 + s) * (t + 1) / 8.);

	// xs, xt, xu calculations
	double xs_val = 0.0, xt_val = 0.0, xu_val = 0.0;
	for (int i = 0; i < 8; ++i) {
		xs_val += Ns[i] * xs[i];
		xt_val += Nt[i] * xs[i];
		xu_val += Nu[i] * xs[i];
	}

	// ys, yt, yu calculations
	double ys_val = 0.0, yt_val = 0.0, yu_val = 0.0;
	for (int i = 0; i < 8; ++i) {
		ys_val += Ns[i] * ys[i];
		yt_val += Nt[i] * ys[i];
		yu_val += Nu[i] * ys[i];
	}

	// zs, zt, zu calculations
	double zs_val = 0.0, zt_val = 0.0, zu_val = 0.0;
	for (int i = 0; i < 8; ++i) {
		zs_val += Ns[i] * zs[i];
		zt_val += Nt[i] * zs[i];
		zu_val += Nu[i] * zs[i];
	}

	// MJ calculation
	Eigen::MatrixXd MJ(3, 3);
	MJ << xs_val, ys_val, zs_val,
		xt_val, yt_val, zt_val,
		xu_val, yu_val, zu_val;

	// J and J_v calculations
	double J = MJ.determinant();
	Eigen::MatrixXd J_v = MJ.inverse();

	// Nx, Ny, Nz calculations
	std::vector<double> Nx, Ny, Nz;
	for (int i = 0; i < 8; ++i) {
		Nx.push_back(J_v(0, 0) * Ns[i] + J_v(0, 1) * Nt[i] + J_v(0, 2) * Nu[i]);
		Ny.push_back(J_v(1, 0) * Ns[i] + J_v(1, 1) * Nt[i] + J_v(1, 2) * Nu[i]);
		Nz.push_back(J_v(2, 0) * Ns[i] + J_v(2, 1) * Nt[i] + J_v(2, 2) * Nu[i]);
	}

	// B matrix construction
	Eigen::MatrixXd B(6, 24);
	B << Nx[0], 0, 0, Nx[1], 0, 0, Nx[2], 0, 0, Nx[3], 0, 0, Nx[4], 0, 0, Nx[5], 0, 0, Nx[6], 0, 0, Nx[7], 0, 0,
		0, Ny[0], 0, 0, Ny[1], 0, 0, Ny[2], 0, 0, Ny[3], 0, 0, Ny[4], 0, 0, Ny[5], 0, 0, Ny[6], 0, 0, Ny[7], 0,
		0, 0, Nz[0], 0, 0, Nz[1], 0, 0, Nz[2], 0, 0, Nz[3], 0, 0, Nz[4], 0, 0, Nz[5], 0, 0, Nz[6], 0, 0, Nz[7],
		Ny[0], Nx[0], 0, Ny[1], Nx[1], 0, Ny[2], Nx[2], 0, Ny[3], Nx[3], 0, Ny[4], Nx[4], 0, Ny[5], Nx[5], 0, Ny[6], Nx[6], 0, Ny[7], Nx[7], 0,
		0, Nz[0], Ny[0], 0, Nz[1], Ny[1], 0, Nz[2], Ny[2], 0, Nz[3], Ny[3], 0, Nz[4], Ny[4], 0, Nz[5], Ny[5], 0, Nz[6], Ny[6], 0, Nz[7], Ny[7],
		Nz[0], 0, Nx[0], Nz[1], 0, Nx[1], Nz[2], 0, Nx[2], Nz[3], 0, Nx[3], Nz[4], 0, Nx[4], Nz[5], 0, Nx[5], Nz[6], 0, Nx[6], Nz[7], 0, Nx[7];
	return std::make_tuple(B,J);
}

void SoildElement_Brick3D::calculate_volume()
{

}

void SoildElement_Brick3D::calculate_all()
{
	calculate_D();
	Eigen::VectorXd node(3);
	node << 0, 0, 0;
	auto result = calculate_B(node);
	m_B = std::get<0>(result);
	calculate_ke();
}

void SoildElement_Brick3D::calculate_ke()
{
	Eigen::VectorXd loc(3);
	Eigen::VectorXd w(3);
	loc << -0.77459667, 0., 0.77459667;
	w << 0.55555556, 0.88888889, 0.55555556;

	double result = 0.0;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				double v1 = loc[i];
				double v2 = loc[j];
				double v3 = loc[k];

				Eigen::VectorXd node(3);
				node << v1, v2, v3;

				std::tuple <Eigen::MatrixXd, double> result;
				result = calculate_B(node);
				MatrixXd B = std::get<0>(result);
				double J = std::get<1>(result);
				m_ke += w(i) * w(j) * w(k) * B.transpose() * m_D * B * J;
			}
		}
	}

}

void SoildElement_Brick3D::Disp()
{
	std::cout << m_id << " " << m_idSection << " ";
	int nNode = m_idNode.size();
	for (int i = 0; i < nNode; i++)
	{
		std::cout << m_idNode[i] << " ";
	}

	std::cout << "\n";
}

void SoildElement_Brick3D::Input_Data(std::ifstream& fin)
{
	fin >> m_id;
	fin >> m_idSection;
	for (auto& a : m_idNode)
	{
		fin >> a;
	}
}

int SoildElement_Brick3D::Get_DOF_Node()
{
	m_dof = 3;
	return 3;
}

SoildElement_Brick3D::SoildElement_Brick3D()
{
	m_type = "Brick";
	m_idNode.resize(8);
	m_ke.resize(24, 24);
	m_ke.setZero();
}

SoildElement_Brick3D::SoildElement_Brick3D(int id, std::vector<int> idNodes)
{
	*this = SoildElement_Brick3D();
	m_id = id;
	m_type = "Brick";
	for (int i = 0; i < idNodes.size(); i++)
	{
		m_idNode[i] = idNodes[i];
	}
}

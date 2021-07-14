#include "schwefel_2_6.h"
namespace OFEC {
	
	Schwefel_2_6::Schwefel_2_6(const ParamMap &v) :
		Schwefel_2_6((v.at("problem name")), (v.at("number of variables"))) {}

	Schwefel_2_6::Schwefel_2_6(const std::string &name, size_t num_vars) :
		Problem(name),
		Function(name, num_vars),
		m_a(num_vars, std::vector<int>(num_vars)),
		m_b(num_vars) {}

	void Schwefel_2_6::initialize() {    // To be continued ..
		m_variable_monitor = true;
		setDomain(-100, 100);
		setInitialDomain(-100, 100);
		m_initialized = true;
	}

	void Schwefel_2_6::load_data() {
		std::string sa;
		std::stringstream ss;
		ss << m_num_vars << "Dim.txt";
		sa = ss.str();
		sa.insert(0, m_name + "_a_");
		sa.insert(0, "/instance/problem/continuous/global/classical/data/");
		//sa.insert(0, g_working_dir);// data path

		std::ifstream in_a;
		in_a.open(sa.data());
		if (in_a.fail()) {
			for (int i = 0; i < m_num_vars; ++i) {
				for (int j = 0; j < m_num_vars; ++j) {
					m_a[i][j] = int(-500.0 + global::ms_global->m_uniform[caller::Problem]->next() * 1000);
				}
			}
			std::ofstream out(sa.c_str());
			for (int i = 0; i < m_num_vars; ++i) {
				for (int j = 0; j < m_num_vars; j++) {
					out << m_a[i][j] << " ";
				}
			}
			out.close();
		}
		else {
			for (int i = 0; i < m_num_vars; ++i) {
				for (int j = 0; j < m_num_vars; j++) {
					in_a >> m_a[i][j];
				}
			}
		}
		in_a.close();

		m_translation.resize(m_num_vars);
		std::string so;
		so = ss.str();
		so.insert(0, m_name + "_Shift_");

		so.insert(0, "/instance/problem/continuous/global/classical/data/");
		//so.insert(0, g_working_dir);// data path

		std::ifstream in;
		in.open(so.data());
		if (in.fail()) {
			setTranslation(m_original_optima.variable(0).data());
			for (int i = 0; i < m_num_vars; ++i) {
				if (i < m_num_vars / 4) m_translation[i] = -100;
				else if (i >= m_num_vars * 3 / 4 - 1) m_translation[i] = 100;
			}

			std::ofstream out(so.c_str());
			for (int j = 0; j < m_num_vars; j++)        out << m_translation[j] << " ";
			out.close();
		}
		else {
			for (int j = 0; j < m_num_vars; j++) {
				in >> m_translation[j];
			}
		}
		in.close();

		for (int i = 0; i < m_num_vars; ++i) {
			m_b[i] = 0;
			for (int j = 0; j < m_num_vars; ++j) {
				m_b[i] += m_a[i][j] * m_translation[j];
			}
		}
	}

	EvalTag Schwefel_2_6::evaluateObjective(Real *x, std::vector<Real> &obj) {
		// To be continued ..
		return EvalTag::Normal;
	}
	
}
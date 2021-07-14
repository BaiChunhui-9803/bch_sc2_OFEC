#include "GLT4.h"

namespace OFEC {
	GLT4::GLT4(param_map & v) : GLT4(v.at("problem name"), v.at("number of variables")) { //param_numDim = 30 is suggested

	}
	GLT4::GLT4(const std::string & name, size_t size_var) : problem(name, size_var, 2), GLT(name, size_var, 2) {

	}

	void GLT4::initialize() {
		std::vector<std::pair<Real, Real>> r;
		r.push_back(std::make_pair(0., 1.));
		r.push_back(std::make_pair(0., 1.));
		for (int i = 2; i < m_num_vars; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		setInitialDomain(r);
		setDomain(r);
		load_PF();
		m_optima.set_flag_objective(true);
		m_initialized = true;
	}

	EvalTag GLT4::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real yj, g = 0;
		for (size_t j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			g += yj * yj;
		}

		obj[0] = (1 + g)*x[0];
		obj[1] = (1 + g)*(2 - 2 * std::sqrt(x[0]) * cos(2 * std::sqrt(x[0])*OFEC_PI)*cos(2 * sqrt(x[0])*OFEC_PI));
		return EvalTag::Normal;
	}
}
#include "GLT3.h"

namespace OFEC {
	GLT3::GLT3(param_map & v) : GLT3(v.at("problem name"), v.at("number of variables")) { //param_numDim = 30 is suggested

	}
	GLT3::GLT3(const std::string & name, size_t size_var) : problem(name, size_var, 2), GLT(name, size_var, 2) {

	}

	EvalTag GLT3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real yj, g = 0;
		for (size_t j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(2 * OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			g += yj * yj;
		}

		obj[0] = (1 + g)*x[0];
		if (obj[0] <= 0.05)
			obj[1] = (1 + g)*(1 - 19 * x[0]);
		else
			obj[1] = (1 + g)*(1.0 / 19 - x[0] / 19);
		return EvalTag::Normal;
	}
}
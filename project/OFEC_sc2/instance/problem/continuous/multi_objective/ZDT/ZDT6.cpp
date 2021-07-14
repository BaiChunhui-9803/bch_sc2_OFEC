#include "ZDT6.h"

namespace OFEC {
	ZDT6::ZDT6(param_map & v) :  ZDT6(v.at("problem name"), v.at("number of variables")) { //param_numDim = 10 is suggested
		
	}
	ZDT6::ZDT6(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		
	}

	EvalTag ZDT6::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t n = 1; n<m_num_vars; n++)
			g = g + x[n];
		g = pow(g / (m_num_vars - 1), 0.25);
		g = 1 + 9 * g;
		obj[0] = 1 - exp(-4 * x[0])*pow(sin(6 * OFEC_PI*x[0]), 6);
		obj[1] = g*(1 - pow(obj[0] / g, 2));
		return EvalTag::Normal;
	}
}

#include "ZDT1.h"

namespace OFEC {
	ZDT1::ZDT1(param_map & v) : ZDT1(v.at("problem name"), v.at("number of variables")) { //param_numDim = 30 is suggested
		
	}
	ZDT1::ZDT1(const std::string & name, size_t size_var) : problem(name, size_var, 2), ZDT(name, size_var) {
		
	}

	EvalTag ZDT1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t n = 1; n < m_num_vars; n++)
			g = g + x[n];
		g = 1 + 9 * g / (m_num_vars - 1);
		obj[0] = x[0];
		obj[1] = g * (1 - pow(obj[0] / g, 0.5));
		return EvalTag::Normal;
	}
}
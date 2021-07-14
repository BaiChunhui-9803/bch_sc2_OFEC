#include "DTLZ1.h"

namespace OFEC {
	DTLZ1::DTLZ1(param_map & v) :
		DTLZ1(v.at("problem name"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) { // param_interTest1 = 5 is recommended
	}

	DTLZ1::DTLZ1(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	EvalTag DTLZ1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t i = m_num_objs - 1; i < m_num_vars; i += 1)
		{
			g += (x[i] - 0.5)*(x[i] - 0.5) - cos(20 * OFEC_PI*(x[i] - 0.5));
		}
		g = (m_num_vars + 1 - m_num_objs + g) * 100;
		for (size_t m = 0; m < m_num_objs; m += 1)
		{
			Real product = 0.5*(1 + g);
			size_t i = 0;
			for (; m_num_objs >= 2 + m && i <= m_num_objs - 2 - m; i += 1)
				product *= x[i];
			if (m > 0)
				product *= (1 - x[i]);
			obj[m] = product;
		}
		return EvalTag::Normal;
	}
}
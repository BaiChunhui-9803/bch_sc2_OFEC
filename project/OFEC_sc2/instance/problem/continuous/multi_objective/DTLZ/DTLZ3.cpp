#include "DTLZ3.h"

namespace OFEC {
	DTLZ3::DTLZ3(param_map & v) : 
		DTLZ3(v.at("problem name"), (v.at("numObj") + v.at("interTest1") - 1), v.at("numObj")) { // param_interTest1 = 10 is recommended
	}

	DTLZ3::DTLZ3(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		DTLZ(name, size_var, size_obj) {
	}

	EvalTag DTLZ3::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real g = 0;
		for (size_t i = m_num_objs - 1; i < m_num_vars; i += 1)
			g += (pow((x[i] - 0.5), 2) - cos(20 * OFEC_PI*(x[i] - 0.5)));
		g = (10 + g) * 100;
		for (size_t m = 0; m < m_num_objs; m += 1)
		{
			Real product = (1 + g);
			size_t i = 0;
			for (; i + m <= m_num_objs - 2; i += 1)
				product *= cos(x[i] * OFEC_PI / 2);
			if (m > 0)
				product *= sin(x[i] * OFEC_PI / 2);
			obj[m] = product;
		}
		return EvalTag::Normal;
	}
}
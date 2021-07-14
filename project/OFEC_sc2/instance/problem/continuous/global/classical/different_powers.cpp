#include "different_powers.h"

namespace OFEC {

	different_powers::different_powers(const ParamMap &v) :
		different_powers((v.at("problem name")), (v.at("number of variables")), 1) {

	}
	different_powers::different_powers(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void different_powers::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);

		setOriginalGlobalOpt();

		setGlobalOpt();
		m_initialized = true;
	}
	EvalTag different_powers::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		size_t i;
		obj[0] = 0.0;
		for (i = 0; i<m_num_vars; i++)
		{
			obj[0] += pow(fabs(x[i]), 2.0 + 4.0*i / (m_num_vars - 1.0));
		}
		obj[0] = pow(obj[0], 0.5);
		return EvalTag::Normal;
	}

}
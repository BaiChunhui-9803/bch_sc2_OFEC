#include "expanded_himmelblau.h"

namespace OFEC {

	expanded_himmelblau::expanded_himmelblau(const ParamMap &v) :
		expanded_himmelblau((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	expanded_himmelblau::expanded_himmelblau(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_himmelblau::initialize() {
		setDomain(-40., 10.);
		setInitialDomain(-40., 10.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_obj_minmax_monitored = true;
		// 4^(Dim/2) gopt 
		size_t num = (int)pow(4, m_num_vars/2);
		
		std::vector<std::vector<Real>> obj_data(num, std::vector<Real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag expanded_himmelblau::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
		
		for (i = 0; i<m_num_vars - 1; i += 2)
		{
			x[i] += 3.0;
			x[i + 1] += 2.0;
			obj[0] += pow((x[i] * x[i] + x[i + 1] - 11.0), 2.0) + pow((x[i] + x[i + 1] * x[i + 1] - 7.0), 2.0);
		}
		return EvalTag::Normal;
	}

}
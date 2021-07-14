#include "expanded_decreasing_minima.h"

namespace OFEC {

	expanded_decreasing_minima::expanded_decreasing_minima(const ParamMap &v) :
		expanded_decreasing_minima((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	expanded_decreasing_minima::expanded_decreasing_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_decreasing_minima::initialize() {
		setDomain(-40, 40);
		setInitialDomain(-40., 40.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_variable_monitor = true;
		// 1 gopt 
		std::vector<std::vector<Real>> var_data(1, std::vector<Real>(m_num_vars, 0));

		for (auto &i : var_data) {
			setOriginalGlobalOpt(i.data());
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag expanded_decreasing_minima::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
		
		for (i = 0; i<m_num_vars; ++i)
		{
			x[i] += 0.1;  
			if ((x[i] <= 1.0)&(x[i] >= 0.0))
			{
				obj[0] += -exp(-2.0*log(2.0)*pow((x[i] - 0.1) / 0.8, 2.0))*pow(sin(5.0*OFEC_PI*x[i]), 6.0);
			}
			else
			{
				obj[0] += pow(x[i], 2.0);
			}
		}
		obj[0] += 1.0*m_num_vars;
		return EvalTag::Normal;
	}

}
#include "expanded_uneven_minima.h"

namespace OFEC {

	expanded_uneven_minima::expanded_uneven_minima(const ParamMap &v) :
		expanded_uneven_minima((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	expanded_uneven_minima::expanded_uneven_minima(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_uneven_minima::initialize() {
		setDomain(-40., 40.);
		setInitialDomain(-40., 40.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_obj_minmax_monitored = true;
		// 5^Dim gopt 
		size_t num = (int)pow(5, m_num_vars);
		
		std::vector<std::vector<Real>> obj_data(num, std::vector<Real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag expanded_uneven_minima::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
			x[i] += 0.079699392688696; //pow(0.15,4.0/3.0);//shift to orgin
			if ((x[i] <= 1.0)&(x[i] >= 0.0))
			{
				obj[0] -= pow(sin(5.0*OFEC_PI*(pow(x[i], 0.75) - 0.05)), 6.0);
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
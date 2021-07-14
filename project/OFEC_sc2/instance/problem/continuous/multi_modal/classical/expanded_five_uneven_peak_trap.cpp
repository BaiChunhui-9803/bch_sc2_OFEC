#include "expanded_five_uneven_peak_trap.h"

namespace OFEC {

	expanded_five_uneven_peak_trap::expanded_five_uneven_peak_trap(const ParamMap &v) :
		expanded_five_uneven_peak_trap((v.at("problem name")), (v.at("number of variables")), 1) {

	
	}
	expanded_five_uneven_peak_trap::expanded_five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void expanded_five_uneven_peak_trap::initialize() {
		setDomain(-40, 40);
		setInitialDomain(-40., 40.);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_obj_minmax_monitored = true;
		// 2^Dim gopt 
		size_t num = (int)pow(2, m_num_vars);
		
		std::vector<std::vector<Real>> obj_data(num, std::vector<Real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag expanded_five_uneven_peak_trap::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
			if (x[i]<0)
			{
				obj[0] += -200.0 + pow(x[i], 2.0);
			}
			else if (x[i]<2.5)
			{
				obj[0] += -80.0*(2.5 - x[i]);
			}
			else if (x[i]<5.0)
			{
				obj[0] += -64.0*(x[i] - 2.5);
			}
			else if (x[i]<7.5)
			{
				obj[0] += -160.0 + pow(x[i], 2.0);
			}
			else if (x[i]<12.5)
			{
				obj[0] += -28.0*(x[i] - 7.5);
			}
			else if (x[i]<17.5)
			{
				obj[0] += -28.0*(17.5 - x[i]);
			}
			else if (x[i]<22.5)
			{
				obj[0] += -32.0*(x[i] - 17.5);
			}
			else if (x[i]<27.5)
			{
				obj[0] += -32.0*(27.5 - x[i]);
			}
			else if (x[i] <= 30.0)
			{
				obj[0] += -80.0*(x[i] - 27.5);
			}
			else
			{
				obj[0] += -200.0 + pow(x[i] - 30.0, 2.0);
			}
		}
		obj[0] += 200.0*m_num_vars;
		return EvalTag::Normal;
	}

}
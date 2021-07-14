#include "expanded_two_peak_trap.h"

namespace OFEC {
	
		expanded_two_peak_trap::expanded_two_peak_trap(const ParamMap &v) :
			expanded_two_peak_trap((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		expanded_two_peak_trap::expanded_two_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void expanded_two_peak_trap::initialize() {
			setDomain(-40., 40.);
			setInitialDomain(-40., 40.);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			m_variable_monitor = true;
			// 1 gopt 
			std::vector<std::vector<Real>> var_data(1, std::vector<Real>(m_num_vars,0));
			
			for (auto &i : var_data) {
				setOriginalGlobalOpt(i.data());
			}
			m_optima = m_original_optima;
			m_initialized = true;
		}
		EvalTag expanded_two_peak_trap::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
				x[i] += 20.0;
				if ((x[i]<15.0)&(x[i] >= 0.0))
				{
					obj[0] += -(160.0 / 15.0)*(15.0 - x[i]);
				}
				else if ((x[i] <= 20.0)&(x[i] >= 15.0))
				{
					obj[0] += -40.0*(x[i] - 15.0);
				}
				else if (x[i]<0.0)
				{
					obj[0] += -160.0 + pow(x[i], 2.0);
				}
				else
				{
					obj[0] += -200.0 + pow(x[i] - 20.0, 2.0);
				}
			}
			obj[0] += 200.0*m_num_vars;
			return EvalTag::Normal;
		}
	
}
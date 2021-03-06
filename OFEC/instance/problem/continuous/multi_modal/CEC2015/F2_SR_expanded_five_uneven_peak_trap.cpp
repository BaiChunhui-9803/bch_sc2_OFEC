#include "F2_SR_expanded_five_uneven_peak_trap.h"

namespace ofec {
	namespace CEC2015 {
		F2_SR_expanded_five_uneven_peak_trap::F2_SR_expanded_five_uneven_peak_trap(const ParamMap &v) :
			F2_SR_expanded_five_uneven_peak_trap((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		F2_SR_expanded_five_uneven_peak_trap::F2_SR_expanded_five_uneven_peak_trap(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			CEC2015_function(name, size_var, size_obj) {
			
		}

		void F2_SR_expanded_five_uneven_peak_trap::initialize() {
			setDomain(-100, 100);
			setInitialDomain(-100, 100);
			
			m_variable_accuracy = 0.01;
			m_objective_accuracy = 1.e-4;
			setConditionNumber(1.0);
			m_variable_monitor = true;
			setBias(200.);

			load_optima("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadTranslation("/instance/problem/continuous/multi_modal/CEC2015/data/");
			loadRotation("/instance/problem/continuous/multi_modal/CEC2015/data/");

			// 2^Dim gopt and 5^Dim - 2^Dim lopt 
			evaluate_optima();
			m_initialized = true;
		}
		int F2_SR_expanded_five_uneven_peak_trap::evaluateObjective(Real *x, std::vector<Real> &obj) {

			size_t i;

			for (i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			rotate(x);

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
			obj[0] += m_bias;
			return kNormalEval;
		}
	}
}
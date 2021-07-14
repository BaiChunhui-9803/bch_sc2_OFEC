#include "shubert.h"

namespace OFEC {
	
	shubert::shubert(const ParamMap &v) :
		shubert((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	shubert::shubert(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void shubert::initialize() {
		setDomain(-10, 10);
		setInitialDomain(-10, 10);
		m_variable_accuracy = 0.5;
		m_obj_minmax_monitored = true;
		size_t num = m_num_vars*(size_t)pow(3, m_num_vars);
		
		if (m_num_vars == 2) {
			m_objective_accuracy = 1.e-4;

			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-186.7309);
			}
		}
		else if (m_num_vars == 3) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-2709.09);
			}
		}
		else if (m_num_vars == 4) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-39303.6);
			}
		}
		else {

		}
		
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag shubert::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 1;

		for (int j = 0; j < m_num_vars; ++j) {
			Real a = 0;
			for (int i = 1; i <= 5; i++) {
				a += i*cos((i + 1)*x[j] + i);
			}
			s *= a;
		}
		obj[0] = s + m_bias;
		return EvalTag::Normal;
	}
	
}
#include "michalewicz.h"
namespace OFEC {
	
	michalewicz::michalewicz(const ParamMap &v) :
		michalewicz((v.at("problem name")), 2, 1) {
		
	}
	michalewicz::michalewicz(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_m(20) {

	
	}

	void michalewicz::initialize() {

		setDomain(0, OFEC_PI);
		setInitialDomain(0, OFEC_PI);
		m_variable_accuracy = 1.e-3;
		m_objective_accuracy = 0.2;
		m_variable_monitor = true;
		m_opt_mode[0] = optimization_mode::Maximization;

		 //1 gopt + 1 lopt
		std::vector<std::vector<Real>> var_data = { { 2.20291f, 1.5708f },{ 2.20291f, 2.71157f } };
		for (auto &i : var_data) {
			setOriginalGlobalOpt(i.data());
		}

		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag michalewicz::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;

		for (int i = 0; i < m_num_vars; ++i) {
			s += sin(x[i])*pow(sin((i + 1)*x[i] * x[i] / OFEC_PI), m_m);
		}
		obj[0] = s + m_bias;
		return EvalTag::Normal;
	}
	
}
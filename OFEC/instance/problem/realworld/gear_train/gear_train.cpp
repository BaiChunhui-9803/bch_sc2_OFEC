
#include "gear_train.h"
namespace OFEC {
	gear_train::gear_train(const ParamMap &v) :
		gear_train((v.at("problem name")), 4, 1) {
		

	}

	gear_train::gear_train(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 4, 1), \
		function(name, 4, 1) {
		
	}

	void gear_train::initialize() {
		m_variable_monitor = true;
		setDomain(12, 60);
		setInitialDomain(12, 60);
		std::vector<Real> v(m_num_vars, 0);
		v[0] = 15; v[1] = 20; v[2] = 57; v[3] = 59;

		setOriginalGlobalOpt(v.data());
		m_optima = m_original_optima;
		m_initialized = true;
	}


	EvalTag gear_train::evaluateObjective(Real *x, std::vector<Real> &obj) {

		int x1, x2, x3, x4;
		Real s;

		x1 = (int)x[0]; x2 = (int)x[1]; x3 = (int)x[2]; x4 = (int)x[3];
		s = 1. / 6.931 - x1*x2 / (Real)(x3*x4);

		obj[0] = s*s;
		return EvalTag::Normal;
	}
}
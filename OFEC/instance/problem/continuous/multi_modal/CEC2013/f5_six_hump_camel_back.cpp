#include "f5_six_hump_camel_back.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC::CEC2013 {
	void SixHumpCamelBack::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(2);
		std::vector<std::pair<Real, Real>> ranges({ {-1.9,1.9},{-1.1,1.1} });
		setDomain(ranges);
		m_opt_mode[0] = OptMode::Maximize;
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
		m_variable_niche_radius = 0.5;

		for (size_t i = 0; i < 2; i++)
			m_original_optima.appendObj(1.031628453);
		m_optima = m_original_optima;
		m_optima.setObjectiveGiven(true);
	}

	void SixHumpCamelBack::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = -((4 - 2.1*x[0] * x[0] + pow(x[0], 4.0) / 3.0)*x[0] * x[0] \
			+ x[0] * x[1] + (4 * x[1] * x[1] - 4)*x[1] * x[1]);
		obj[0] = s + m_bias;
	}
}
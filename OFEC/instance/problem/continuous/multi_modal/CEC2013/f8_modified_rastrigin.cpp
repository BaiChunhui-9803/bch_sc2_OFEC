#include "f8_modified_rastrigin.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC::CEC2013 {
	void ModifiedRastrigin::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(0, 1);
		m_opt_mode[0] = OptMode::Maximize;
		m_k.resize(2);
		m_k[0] = 3;
		m_k[1] = 4;
		m_variable_niche_radius = 0.01;
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;

		size_t num_global_optima = 1;
		for (size_t i = 0; i < 2; i++)
			num_global_optima *= m_k[i];
		for (size_t i = 0; i < num_global_optima; ++i)
			m_original_optima.appendObj(-2.);
		m_optima = m_original_optima;
		m_optima.setObjectiveGiven(true);
	}

	void ModifiedRastrigin::evaluateObjective(Real* x, std::vector<Real>& obj) {
		Real s = 0;
		for (int i = 0; i < m_num_vars; ++i) {
			s += 10 + 9 * cos(2 * OFEC_PI * m_k[i] * x[i]);
		}
		obj[0] = -s;
	}
}
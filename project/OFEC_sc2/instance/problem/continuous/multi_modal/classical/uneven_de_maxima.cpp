#include "uneven_de_maxima.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void UnevenDeMaxima::initialize_() { 
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(1);
		setDomain(0, 1.); // note
		m_opt_mode[0] = OptMode::Maximize;
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
		m_variable_niche_radius = 0.01;
		
		std::vector<std::vector<Real>> obj_data(1, std::vector<Real>(m_num_objs, 1.));
		for (auto &i : obj_data)
			m_original_optima.appendObj(i);
		m_optima.setObjectiveGiven(true);
		m_optima = m_original_optima;
	}

	void UnevenDeMaxima::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real tmp1 = -2 * log(2)*((x[0] - 0.08) / 0.854)*((x[0] - 0.08) / 0.854);
		Real tmp2 = sin(5 * OFEC_PI*(pow(x[0], 3.0 / 4.0) - 0.05));
		Real s = exp(tmp1) * pow(tmp2, 6);
		obj[0] = s + m_bias;  // note
	}
}
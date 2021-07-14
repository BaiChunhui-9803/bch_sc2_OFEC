#include "equal_maxima.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
		void EqualMaxima::initialize_() {
			Function::initialize_();
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(1);
			setDomain(0, 1.);
			m_opt_mode[0] = OptMode::Maximize;
			m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
			m_variable_niche_radius = 1.e-2;
			
			std::vector<std::vector<Real>> obj_data(5, std::vector<Real>(1, 1.));
			for (auto &i : obj_data) {
				m_original_optima.appendObj(i[0]);
			}
			m_optima = m_original_optima;
			m_optima.setObjectiveGiven(true);
		}

		void EqualMaxima::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Real s;
			s = pow(sin(5 * OFEC_PI*x[0]), 6);
			obj[0] = s + m_bias;  // note
		}
}
#include "five_uneven_peak_trap.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
		void FiveUnevenPeakTrap::initialize_() {
			Function::initialize_();
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(1);
			setDomain(0, 30);
			m_opt_mode[0] = OptMode::Maximize;
			m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
			m_variable_niche_radius = 0.01;
	
			std::vector<std::vector<Real>> obj_data(2, std::vector<Real>(1, 200));
			for (auto &i : obj_data) {
				m_original_optima.appendObj(i[0]);
			}
			m_optima.setObjectiveGiven(true);
			m_optima = m_original_optima;
		}

		void FiveUnevenPeakTrap::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Real s = -1.0;
			if (x[0] >= 0 && x[0] < 2.5) {
				s = 80 * (2.5 - x[0]);
			}
			else if (x[0] >= 2.5 && x[0] < 5.0) {
				s = 64 * (x[0] - 2.5);
			}
			else if (x[0] >= 5.0 && x[0] < 7.5) {
				s = 64 * (7.5 - x[0]);
			}
			else if (x[0] >= 7.5 && x[0] < 12.5) {
				s = 28 * (x[0] - 7.5);
			}
			else if (x[0] >= 12.5 && x[0] < 17.5) {
				s = 28 * (17.5 - x[0]);
			}
			else if (x[0] >= 17.5 && x[0] < 22.5) {
				s = 32 * (x[0] - 17.5);
			}
			else if (x[0] >= 22.5 && x[0] < 27.5) {
				s = 32 * (27.5 - x[0]);
			}
			else if (x[0] >= 27.5 && x[0] <= 30) {
				s = 80 * (x[0] - 27.5);
			}

			obj[0] = s + m_bias;
		}
}
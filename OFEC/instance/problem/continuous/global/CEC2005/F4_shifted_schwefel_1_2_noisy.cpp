#include "F4_shifted_schwefel_1_2_noisy.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedSchwefel_1_2_Noisy::ShiftedSchwefel_1_2_Noisy(const ParamMap &v) :
			ShiftedSchwefel_1_2_Noisy((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedSchwefel_1_2_Noisy::ShiftedSchwefel_1_2_Noisy(const std::string &name, size_t num_vars) :
			Problem(name), Schwefel_1_2(name, num_vars) {}

		void ShiftedSchwefel_1_2_Noisy::initialize() {
			setDomain(-100, 100);
			setOriginalGlobalOpt();
			setBias(-450);
			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			
			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}

		EvalTag ShiftedSchwefel_1_2_Noisy::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Schwefel_1_2::evaluateObjective(x, obj);
			obj[0] = (obj[0] - m_bias)*fabs(GET_RND(m_id_rnd).normal.next()) + m_bias;
			return EvalTag::Normal;
		}
	}
}
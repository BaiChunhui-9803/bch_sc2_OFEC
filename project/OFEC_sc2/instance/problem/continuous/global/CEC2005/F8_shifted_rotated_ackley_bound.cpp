#include "F8_shifted_rotated_ackley_bound.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedAckleyBound::ShiftedRotatedAckleyBound(const ParamMap &v) :
			ShiftedRotatedAckleyBound((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRotatedAckleyBound::ShiftedRotatedAckleyBound(const std::string &name, size_t num_vars) :
			Problem(name), Ackley(name, num_vars) {}

		void ShiftedRotatedAckleyBound::initialize() {
			setDomain(-32, 32);

			setOriginalGlobalOpt();
			setBias(-140);
			setConditionNumber(100);
			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			for (size_t j = 1; j < m_num_vars / 2 + 1; ++j) {
				m_translation[(2 * j - 1) - 1] = -32;
				m_translation[2 * j - 1] = GET_RND(m_id_rnd).uniform.nextNonStd<Real>(-32, 32);
			}
			loadRotation("/instance/problem/continuous/global/CEC2005/data/");
			
			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
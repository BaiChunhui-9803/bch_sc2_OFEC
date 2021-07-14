#include "F14_shifted_rotated_expanded_scaffer_F6.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedExpandedScafferF6::ShiftedRotatedExpandedScafferF6(const ParamMap &v) :
			ShiftedRotatedExpandedScafferF6((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRotatedExpandedScafferF6::ShiftedRotatedExpandedScafferF6(const std::string &name, size_t num_vars) :
			Problem(name), ScafferF6(name, num_vars) {}

		void ShiftedRotatedExpandedScafferF6::initialize() {
			setDomain(-100, 100);

			setOriginalGlobalOpt();				
			setConditionNumber(3);
			setBias(-300);			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/CEC2005/data/");
			
			setGlobalOpt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
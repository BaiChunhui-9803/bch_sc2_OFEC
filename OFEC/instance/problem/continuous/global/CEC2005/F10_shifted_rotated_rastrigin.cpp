#include "F10_shifted_rotated_rastrigin.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedRastrigin::ShiftedRotatedRastrigin(const ParamMap &v) :
			ShiftedRotatedRastrigin((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRotatedRastrigin::ShiftedRotatedRastrigin(const std::string &name, size_t num_vars) : 
			Problem(name), Rastrigin(name, num_vars) {}

		void ShiftedRotatedRastrigin::initialize() {
			setDomain(-5, 5);
			setOriginalGlobalOpt();
			setBias(-330);
			setConditionNumber(2);

			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			loadRotation("/instance/problem/continuous/global/CEC2005/data/");
			
			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
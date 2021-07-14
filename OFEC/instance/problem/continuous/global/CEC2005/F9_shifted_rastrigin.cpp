#include "F9_shifted_rastrigin.h"

namespace OFEC {
	namespace CEC2005 {
		ShiftedRastrigin::ShiftedRastrigin(const ParamMap &v) :
			ShiftedRastrigin((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRastrigin::ShiftedRastrigin(const std::string &name, size_t num_vars) :
			Problem(name), Rastrigin(name, num_vars) {}

		void ShiftedRastrigin::initialize() {
			setDomain(-5, 5);
			setOriginalGlobalOpt();
			setBias(-330);

			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path

			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
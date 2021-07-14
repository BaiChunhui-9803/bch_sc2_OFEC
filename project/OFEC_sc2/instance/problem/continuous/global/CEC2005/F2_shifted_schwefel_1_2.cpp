#include "F2_shifted_schwefel_1_2.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedSchwefel_1_2::ShiftedSchwefel_1_2(const ParamMap &v) :
			ShiftedSchwefel_1_2((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedSchwefel_1_2::ShiftedSchwefel_1_2(const std::string &name, size_t num_vars) :
			Problem(name), Schwefel_1_2(name, num_vars) {}

		void ShiftedSchwefel_1_2::initialize() {
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
	}
}
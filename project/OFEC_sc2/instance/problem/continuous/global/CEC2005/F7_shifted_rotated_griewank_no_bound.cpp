#include "F7_shifted_rotated_griewank_no_bound.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedGriewankNoBound::ShiftedRotatedGriewankNoBound(const ParamMap &v) :
			ShiftedRotatedGriewankNoBound((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRotatedGriewankNoBound::ShiftedRotatedGriewankNoBound(const std::string &name, size_t num_vars) :
			Problem(name), Griewank(name, num_vars) {}

		void ShiftedRotatedGriewankNoBound::initialize() {
#ifdef OFEC_DEMO // only for demonstration
			setDomain(-1000, 1000);
#endif
			setInitialDomain(0, 600.);
			setBias(-180);
			setConditionNumber(3);
			setOriginalGlobalOpt();
			
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
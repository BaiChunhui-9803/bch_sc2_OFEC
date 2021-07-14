#include "F11_shifted_rotated_weierstrass.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedWeierstrass::ShiftedRotatedWeierstrass(const ParamMap &v) :
			ShiftedRotatedWeierstrass((v.at("problem name")), (v.at("number of variables"))) {
			
		}
		ShiftedRotatedWeierstrass::ShiftedRotatedWeierstrass(const std::string &name, size_t num_vars) : Problem(name), Weierstrass(name, num_vars) {}

		void ShiftedRotatedWeierstrass::initialize() {
			setDomain(-0.5, 0.5);

			setOriginalGlobalOpt();
			setConditionNumber(5);
			setBias(90);
			
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
#include "F3_shifted_rotated_high_cond_elliptic.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRotatedHighCondElliptic::ShiftedRotatedHighCondElliptic(const ParamMap &v) :
			ShiftedRotatedHighCondElliptic((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedRotatedHighCondElliptic::ShiftedRotatedHighCondElliptic(const std::string &name, size_t num_vars) :
			Problem(name), Elliptic(name, num_vars) {}

		void ShiftedRotatedHighCondElliptic::initialize() {
			setDomain(-100., 100.);

			setOriginalGlobalOpt();
			setBias(-450);
			
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
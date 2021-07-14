#include "F6_shifted_rosenbrock.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedRosenbrock::ShiftedRosenbrock(const ParamMap &v) :
			ShiftedRosenbrock((v.at("problem name")), (v.at("number of variables"))) {}
		
		ShiftedRosenbrock::ShiftedRosenbrock(const std::string &name, size_t num_vars) :
			Problem(name), Rosenbrock(name, num_vars) {}

		void ShiftedRosenbrock::initialize() {
			setDomain(-100, 100);
			std::vector<Real> v(m_num_vars, 1);
			setOriginalGlobalOpt(v.data());
			setBias(390);
			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			
			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
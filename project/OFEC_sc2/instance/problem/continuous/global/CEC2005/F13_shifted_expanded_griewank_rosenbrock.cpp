#include "F13_shifted_expanded_griewank_rosenbrock.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		ShiftedExpandedGriewankRosenbrock::ShiftedExpandedGriewankRosenbrock(const ParamMap &v) :
			ShiftedExpandedGriewankRosenbrock((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedExpandedGriewankRosenbrock::ShiftedExpandedGriewankRosenbrock(const std::string &name, size_t num_vars) :
			Problem(name), Function(name, num_vars) {}

		void ShiftedExpandedGriewankRosenbrock::initialize() {
			setDomain(-3, 1);
			setOriginalGlobalOpt();
			setBias(-130);
			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path

			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}

		EvalTag ShiftedExpandedGriewankRosenbrock::evaluateObjective(Real *x, std::vector<Real> &obj) {
			if (m_translated) {
				translate(x);
				translateOrigin(x);
			}
			Real result = 0;
			for (size_t i = 0; i < m_num_vars; ++i) {
				Real result_f2 = 0;
				Real result_f8 = 0;
				Real x_front = x[i] + 1;
				Real x_back = x[(i + 1) % m_num_vars] + 1;
				result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
				result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((Real)(i + 1))) + 1;
				result += result_f8;
			}
			result += m_bias;
			obj[0] = result;
			return EvalTag::Normal;
		}
	}
}
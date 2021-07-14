
#include "F1_R_elliptic.h"
namespace OFEC {
	namespace CEC2015 {
		F1_R_elliptic::F1_R_elliptic(const ParamMap &v) :
			F1_R_elliptic((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F1_R_elliptic::F1_R_elliptic(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			elliptic(name, size_var, size_obj) {

			
		}

		void F1_R_elliptic::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);

			setOriginalGlobalOpt();
			setConditionNumber(1.);
			setBias(100.);
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}



		EvalTag F1_R_elliptic::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return elliptic::evaluateObjective(x,obj);
		}
	}
}
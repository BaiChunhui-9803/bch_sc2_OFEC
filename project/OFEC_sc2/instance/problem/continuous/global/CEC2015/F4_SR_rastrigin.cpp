
#include "F4_SR_rastrigin.h"
namespace OFEC {
	namespace CEC2015 {
		F4_SR_rastrigin::F4_SR_rastrigin(const ParamMap &v) :
			F4_SR_rastrigin((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F4_SR_rastrigin::F4_SR_rastrigin(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			rastrigin(name, size_var, size_obj) {

			
		}

		void F4_SR_rastrigin::initialize() {
			m_variable_monitor = true;
			setDomain(-5.12, 5.12);
			setInitialDomain(-5.12, 5.12);
			setOriginalGlobalOpt();
			setConditionNumber(1.);
			setBias(400.);
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			setScale(100. / 5.12);
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}



		EvalTag F4_SR_rastrigin::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return rastrigin::evaluateObjective(x, obj);
		}
	}
}
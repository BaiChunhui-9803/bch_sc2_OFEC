
#include "F3_SR_ackley.h"
namespace OFEC {
	namespace CEC2015 {
		F3_SR_ackley::F3_SR_ackley(const ParamMap &v) :
			F3_SR_ackley((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F3_SR_ackley::F3_SR_ackley(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			ackley(name, size_var, size_obj) {

			
		}

		void F3_SR_ackley::initialize() {
			m_variable_monitor = true;
			setDomain(-32.768, 32.768);
			setInitialDomain(-32.768, 32.768);

			setOriginalGlobalOpt();
			setConditionNumber(1.);
			setBias(300.);
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}



		EvalTag F3_SR_ackley::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return ackley::evaluateObjective(x, obj);
		}
	}
}
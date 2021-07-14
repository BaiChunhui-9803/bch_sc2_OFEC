
#include "F5_SR_schwefel.h"
namespace OFEC {
	namespace CEC2015 {
		F5_SR_schwefel::F5_SR_schwefel(const ParamMap &v) :
			F5_SR_schwefel((v.at("problem name")), (v.at("number of variables")), 1) {

		
		}
		F5_SR_schwefel::F5_SR_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			schwefel(name, size_var, size_obj) {

			
		}

		void F5_SR_schwefel::initialize() {
			m_variable_monitor = true;
			setDomain(-500, 500);
			setInitialDomain(-500, 500);
			setOriginalGlobalOpt();
			setConditionNumber(1.);
			setBias(500.);
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			setScale(1. / 10.);
			
			setGlobalOpt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
			m_initialized = true;
		}



		EvalTag F5_SR_schwefel::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return schwefel::evaluateObjective(x, obj);
		}
	}
}
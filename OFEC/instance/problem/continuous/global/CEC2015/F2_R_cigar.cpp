
#include "F2_R_cigar.h"
namespace OFEC {
	namespace CEC2015 {
		F2_R_cigar::F2_R_cigar(const ParamMap &v) :
			F2_R_cigar((v.at("problem name")), (v.at("number of variables")), 1) {

			
		}
		F2_R_cigar::F2_R_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			bent_cigar(name, size_var, size_obj) {

			
		}

		void F2_R_cigar::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);

			setOriginalGlobalOpt();
			setConditionNumber(1.);
			setBias(200.);
			loadTranslation("/instance/problem/continuous/global/CEC2015/data/");
			loadRotation("/instance/problem/continuous/global/CEC2015/data/");
			
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}



		EvalTag F2_R_cigar::evaluateObjective(Real *x, std::vector<Real> &obj) {
			return bent_cigar::evaluateObjective(x, obj);
		}
	}
}
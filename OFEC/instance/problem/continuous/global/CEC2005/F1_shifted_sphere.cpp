#include "F1_shifted_sphere.h"

namespace OFEC {
	namespace CEC2005 {
		ShiftedSphere::ShiftedSphere(const ParamMap &v) : 
			ShiftedSphere((v.at("problem name")), (v.at("number of variables"))) {}

		ShiftedSphere::ShiftedSphere(const std::string &name, size_t num_vars) : 
			Problem(name), Sphere(name, num_vars) {}

		void ShiftedSphere::initialize() {
			setDomain(-100., 100.);
			setOriginalGlobalOpt();

			setBias(-450);
			
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			
			setGlobalOpt(m_translation.data());
			m_optima.setVariableGiven(true);
			m_obj_minmax_monitored = true;
			m_objective_accuracy = 1.0e-8;

			m_initialized = true;
		}
	}
}
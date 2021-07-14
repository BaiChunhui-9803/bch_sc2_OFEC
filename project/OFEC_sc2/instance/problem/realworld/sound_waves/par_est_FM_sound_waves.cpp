

#include "par_est_FM_sound_waves.h"
namespace OFEC {
	par_est_FM_sound_waves::par_est_FM_sound_waves(const ParamMap &v) :
		par_est_FM_sound_waves((v.at("problem name")), 6, 1) {
		

	}
	par_est_FM_sound_waves::par_est_FM_sound_waves(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 6, 1), \
		function(name, 6, 1) {
		
	}


	void par_est_FM_sound_waves::initialize() {
		m_variable_monitor = true;
		setDomain(-6.4, 6.35);
		setInitialDomain(-6.4, 6.35);
		std::vector<Real> v(m_num_vars);
		v[0] = 1.0; v[1] = 5.0; v[2] = 1.5; v[3] = 4.8; v[4] = 2.0; v[5] = 4.9;

		setOriginalGlobalOpt(v.data());
		m_optima = m_original_optima;
		m_initialized = true;
	}

	EvalTag par_est_FM_sound_waves::evaluateObjective(Real *x, std::vector<Real> &obj) {

		Real theta = 2 * OFEC_PI / 100.;
		Real s = 0, t;

		for (int i = 0; i < 100; i++) {
			t = x[0] * sin(x[1] * i*theta + x[2] * sin(x[3] * i*theta + x[4] * sin(x[5] * i*theta))) - sin(5.*i*theta + 1.5*sin(4.8*i*theta + 2.0*sin(4.9*i*theta)));
			s += t*t;
		}
		obj[0] = s;
		return EvalTag::Normal;
	}
}
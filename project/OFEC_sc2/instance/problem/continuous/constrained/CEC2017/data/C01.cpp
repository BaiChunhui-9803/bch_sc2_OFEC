#include "C01.h"

namespace OFEC {
	namespace CEC2017 {
		C01::C01(const ParamMap &v) :
			C01((v.at("problem name")), (v.at("number of variables")), 1) {
		}
		C01::C01(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
		}

		void C01::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_constraint_type.resize(1);
			m_constraint_type[0] = constraint_type::Inequality; 

			//loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path 
			//setOriginalGlobalOpt(m_translation.data());
			//m_optima = m_original_optima;

			setOriginalGlobalOpt();
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path 
			setGlobalOpt(m_translation.data());
			m_initialized = true;
		}
		void C01::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];

			size_t i, j;
			obj[0] = 0.0;
			for (i = 1; i<m_num_vars+1; ++i)
			{
				Real t = 0.0;
				for (j = 0; j<i; j++)
				{
					t += x[j];
				}
				obj[0] += t*t;
			}
			obj[0] += m_bias;
			
			// evaluate constraint value

			con[0] = 0;
			for (i = 0; i < m_num_vars; ++i)
			{
				con[0] += (x[i] * x[i] - 5000 * cos(0.1*OFEC_PI*x[i]) - 4000);
			}
			if (con[0] <= 0) con[0] = 0;
		
		}
	}
}
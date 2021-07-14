#include "C07.h"

namespace OFEC {
	namespace CEC2017 {
		C07::C07(const ParamMap &v) :
			C07((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C07::C07(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C07::initialize() {
			m_variable_monitor = true;
			setDomain(-50., 50.);
			setInitialDomain(-50., 50.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Equality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C07::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_num_vars - 1; ++i)
			{
				obj[0] += x[i] * sin(x[i]);
			}
			obj[0] += m_bias;

			
			// evaluate constraint value
			
			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_num_vars; ++i)
			{
				con[0] += (x[i] - 100 * cos(0.5*x[i]) + 100);       
			}
			
			con[1] *= -1 * con[0];

			for (auto &i : con)
				if (fabs(i) - 1e-4 <= 0) i = 0;
		}
	}
}
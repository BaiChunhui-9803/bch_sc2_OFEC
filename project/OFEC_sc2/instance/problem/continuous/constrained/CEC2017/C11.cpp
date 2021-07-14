#include "C11.h"

namespace OFEC {
	namespace CEC2017 {
		C11::C11(const ParamMap &v) :
			C11((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C11::C11(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C11::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Equality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C11::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i < m_num_vars ; ++i)
			{
				obj[0] += x[i];
			}

			obj[0] += m_bias;

			
			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_num_vars; ++i)
			{
				con[0] *= x[i];
			}
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i < m_num_vars - 1; ++i)
			{
				con[1] += (x[i] - x[i + 1])*(x[i] - x[i + 1]);
			}
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
	

		}
	}
}
#include "C23.h"

namespace OFEC {
	namespace CEC2017 {
		C23::C23(const ParamMap &v) :
			C23((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C23::C23(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C23::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Equality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			loadRotation("/instance/problem/continuous/constrained/CEC2017/data/");
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C23::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;

			Real num1 = 0., num2 = 0.;
			obj[0] = 0.;
			for (i = 0; i<m_num_vars; ++i)
			{
				num1 += x[i] * x[i];
			}
			num1 = sqrt(num1 / m_num_vars);
			for (i = 0; i<m_num_vars; ++i)
			{
				num2 += cos(2 * OFEC_PI*x[i]);
			}
			num2 /= m_num_vars;
			obj[0] = -20 * exp(-0.2*num1) + 20 - exp(num2) + exp(1.0);
			obj[0] += m_bias;

			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 1; i < m_num_vars; ++i)
			{
				con[0] += pow(x[i], 2);
			}
			con[0] += 1 - fabs(x[0]);
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i < m_num_vars; ++i)
			{
				con[1] += x[i] * x[i];
			}
			con[1] -= 4;
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
			
		}
	}
}
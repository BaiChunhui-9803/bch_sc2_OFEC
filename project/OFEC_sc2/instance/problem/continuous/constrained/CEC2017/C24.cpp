#include "C24.h"

namespace OFEC {
	namespace CEC2017 {
		C24::C24(const ParamMap &v) :problem((v.at("problem name")), (v.at("number of variables")), 1), \
			function((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C24::C24(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C24::initialize() {
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
		void C24::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			rotate(x);

			size_t i;

			obj[0] = fabs(x[0]);
			for (i = 1; i<m_num_vars; ++i)
			{
				if (fabs(x[i]) > obj[0]) obj[0] = fabs(x[i]);
			}
			obj[0] += m_bias;

			
			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i < m_num_vars; ++i)
			{
				con[0] += pow(x[i], 2);
			}
			con[0] -= 100 * m_num_vars;
			if (con[0] <= 0) con[0] = 0;

			std::vector<Real> x_temp(x, x + m_num_vars);
			for (size_t i = 0; i < m_num_vars; ++i)
				x_temp[i] -= m_translation[i];
			rotate(x_temp.data());

			Real result = fabs(x_temp[0]);
			for (i = 1; i<m_num_vars; ++i)
			{
				if (fabs(x_temp[i]) > result) result = fabs(x_temp[i]);
			}
			con[1] = cos(result) + sin(result);
			if (fabs(con[1]) - 1e-4 <= 0) con[1] = 0;
			
		}
	}
}
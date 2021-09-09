#include "C27.h"

namespace OFEC {
	namespace CEC2017 {
		C27::C27(const ParamMap &v) :
			C27((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C27::C27(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C27::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_constraint_type.resize(3);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Inequality;
			m_constraint_type[2] = constraint_type::Equality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			loadRotation("/instance/problem/continuous/constrained/CEC2017/data/");
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C27::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];
			std::vector<Real> x_ori(x, x + m_num_vars);
			rotate(x);

			std::vector<Real> x_(x, x + m_num_vars);
			for (size_t i = 0; i < m_num_vars; ++i) {
				if (fabs(x[i] >= 0.5)) x_[i] = 0.5*round(2 * x[i]);
			}
			size_t i;
			obj[0] = 0.;

			for (i = 0; i<m_num_vars; ++i)
			{
				obj[0] += x_[i] * x_[i] - 10 * cos(2 * OFEC_PI*x_[i]) + 10;
			}
			obj[0] += m_bias;

			// evaluate constraint value

			for (auto &i : con)
				i = 0.;

			for (i = 0; i<m_num_vars; ++i)
			{
				con[0] += fabs(x_ori[i]);
			}
			con[0] = 1 - con[0];
			if (con[0] <= 0) con[0] = 0;

			for (i = 0; i<m_num_vars; ++i)
			{
				con[1] += pow(x_ori[i], 2);
			}
			con[1] -= 100 * m_num_vars;
			if (con[1] <= 0) con[1] = 0;

			Real num1 = 0., num2 = 1.;
			for (i = 0; i<m_num_vars - 1; ++i)
			{
				num1 += 100 * pow((pow(x_ori[i], 2) - x_ori[i + 1]), 2);
				num2 *= pow(sin(x_ori[i] - 1), 2)*OFEC_PI;
			}
			con[2] = num1 + num2;
			if (fabs(con[2]) - 1e-4 <= 0) con[2] = 0;

			
		}
	}
}
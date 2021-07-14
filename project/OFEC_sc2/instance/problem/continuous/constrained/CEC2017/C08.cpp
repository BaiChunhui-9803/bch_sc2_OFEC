#include "C08.h"

namespace OFEC {
	namespace CEC2017 {
		C08::C08(const ParamMap &v) :
			C08((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C08::C08(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj) {
			
		}

		void C08::initialize() {
			m_variable_monitor = true;
			setDomain(-100., 100.);
			setInitialDomain(-100., 100.);
			m_constraint_type.resize(2);
			for (auto &i : m_constraint_type)
				i = constraint_type::Equality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C08::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];

			size_t i,j;
			obj[0] = *std::max_element(x,x+m_num_vars);
			
			obj[0] += m_bias;

			
			// evaluate constraint value
			
			std::vector<Real> x_1(x, x + m_num_vars);
			std::vector<Real> x_2(x, x + m_num_vars);

			for (auto &i : con)
				i = 0.;
			
			for (i = 1; i <= m_num_vars / 2; ++i) {
				x_1[i - 1] = x[2 * i - 2];
			}
			for (i = 0; i < m_num_vars / 2; ++i)
			{
				Real m = 0.;
				for (j = 0; j<i + 1; j++)    //h1
				{
					m += x_1[j];
				}
				con[0] += m*m;
			}

			for (i = 1; i <= m_num_vars / 2; ++i) {
				x_2[i - 1] = x[2 * i - 1];
			}
			for (i = 0; i < m_num_vars / 2; ++i)
			{
				Real m = 0.;
				for (j = 0; j<i + 1; j++)    //h2
				{
					m += x_2[j];
				}
				con[1] += m*m;
			}

			for (auto &i : con)
				if (fabs(i) - 1e-4 <= 0) i = 0;
			
		}
	}
}
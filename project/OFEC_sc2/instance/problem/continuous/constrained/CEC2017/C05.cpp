#include "C05.h"

namespace OFEC {
	namespace CEC2017 {
		C05::C05(const ParamMap &v) :
			C05((v.at("problem name")), (v.at("number of variables")), 1) {
			
		}
		C05::C05(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj), m_mat1(size_var), m_mat2(size_var) {
			
		}

		void C05::initialize() {
			m_variable_monitor = true;
			setDomain(-10., 10.);
			setInitialDomain(-10., 10.);
			m_constraint_type.resize(2);
			m_constraint_type[0] = constraint_type::Inequality;
			m_constraint_type[1] = constraint_type::Inequality;
			 
			
			loadTranslation("/instance/problem/continuous/constrained/CEC2017/data/");  //data path
			
			load_rotation_C05("/instance/problem/continuous/constrained/CEC2017/data/");
			setOriginalGlobalOpt(m_translation.data());
			m_optima = m_original_optima;
			m_initialized = true;
		}
		void C05::evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) {
			for (size_t i = 0; i < m_num_vars; ++i)
				x[i] -= m_translation[i];

			size_t i;
			obj[0] = 0.;
			for (i = 0; i<m_num_vars - 1; ++i)
			{
				obj[0] += (100 * (x[i] * x[i] - x[i + 1])*(x[i] * x[i] - x[i + 1]) + (x[i] - 1)*(x[i] - 1));
			}
			obj[0] += m_bias;

			
			// evaluate constraint value

			std::vector<Real> x_1(x, x + m_num_vars);
			std::vector<Real> x_2(x, x + m_num_vars);

			con[0] = 0.;
			rotate(x_1.data(), 1);   // rotate x_1 using rotation matrix 1
			for (i = 0; i < m_num_vars; ++i)
			{
				con[0] += (x_1[i] * x_1[i] - 50.0*cos(2.0*OFEC_PI*x_1[i]) - 40.0);
			}
			if (con[0] <= 0) con[0] = 0;

			con[1] = 0.;
			rotate(x_2.data(), 2);  // rotate x_2 using rotation matrix 2
			for (i = 0; i < m_num_vars; ++i)
			{
				con[1] += (x_2[i] * x_2[i] - 50.0*cos(2.0*OFEC_PI*x_2[i]) - 40.0);
			}
			if (con[1] <= 0) con[1] = 0;

		}
		bool C05::load_rotation_C05(const std::string &path) {
			std::string s;
			std::stringstream ss;
			ss << m_num_vars << "Dim.txt";
			s = ss.str();
			s.insert(0, m_name + "_RotM_");

			s.insert(0, path);// data path
			s.insert(0, g_working_dir);

			load_rotation_C05_(s);

			return true;
		}

		void C05::load_rotation_C05_(const std::string &path) {
			std::ifstream in;
			in.open(path);
			if (in.fail()) {
				set_rotation_C05();
				std::ofstream out(path);
				m_mat1.print(out);
				m_mat2.print(out);
				out.close();
			}
			else {
				m_mat1.load(in);
				m_mat2.load(in);
			}
			in.close();
		}

		void C05::set_rotation_C05() {
			m_mat1.generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), m_condition_number);
			m_mat2.generate_rotation_classical(global::ms_global->m_normal[caller::Problem].get(), m_condition_number);
		}
		void C05::rotate(Real *x, size_t num) {
			Real *x_ = new Real[m_num_vars];
			std::copy(x, x + m_num_vars, x_);

			for (size_t i = 0; i<m_num_vars; ++i) {
				x[i] = 0;
				
				for (size_t j = 0; j < m_num_vars; ++j) {
					if(num == 1) x[i] += m_mat1[j][i] * x_[j];
					if(num == 2) x[i] += m_mat2[j][i] * x_[j];
				}
			}

			delete[] x_;
			x_ = 0;
		}
	}
}
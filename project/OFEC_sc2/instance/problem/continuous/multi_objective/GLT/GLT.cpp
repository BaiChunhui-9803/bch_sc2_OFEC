#include "GLT.h"
#include <fstream>

namespace OFEC {
	GLT::GLT(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj) {

	}
	void GLT::initialize() {		
		std::vector<std::pair<Real, Real>> r;
		r.push_back(std::make_pair(0., 1.));
		for (int i = 1; i < m_num_vars; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		setInitialDomain(r);
		setDomain(r);		
		load_PF();
		m_optima.set_flag_objective(true);
		m_initialized = true;
	}

	void GLT::load_PF(){
		std::ifstream infile;
		std::stringstream os;
		os << g_working_dir << "/instance/problem/continuous/multi_objective/GLT/data/" << m_name << ".dat";
		infile.open(os.str());
		if (!infile) {
			//std::cout << "open file is failed" << std::endl;
			throw myexcept("open PF file of GLT problem is fail");
			return;
		}
		std::string str;
		size_t line = 0;
		while (getline(infile, str))
			++line;
		m_optima.resize_objective_set(line);
		infile.close();
		infile.clear();
		infile.open(os.str());
		for (size_t i = 0; i < line; i++) {
			std::vector<Real> temp_obj(m_num_objs);
			for (size_t j = 0; j < m_num_objs; j++)
				infile >> temp_obj[j];
			m_optima.set_objective(temp_obj, i);
		}
		infile.close();
	}	
	
}
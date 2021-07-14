/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************/

#include "modified_rastrigin.h"
namespace OFEC {
	
	modified_rastrigin::modified_rastrigin(const ParamMap &v) :
		modified_rastrigin((v.at("problem name")), 2, 1){
		
	}
	modified_rastrigin::modified_rastrigin(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_k(2) {
		
	}

	void modified_rastrigin::initialize() {
		setDomain(0.0, 1.); // note
		setInitialDomain(0.0, 1.); // note
		m_k[0] = 3; m_k[1] = 4;
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;
		m_variable_monitor = true;
		std::ifstream in;
		std::stringstream ss;
		std::vector<std::vector<Real>> var_data(12, std::vector<Real>(m_num_vars));
		ss << g_working_dir << "/instance/problem/continuous/multi_modal/classical/data/" << m_name << "_Opt_" << m_num_vars << "Dim.txt";
		in.open(ss.str());
		if (in.fail()) {
			throw myexcept("cannot open data file@modified_rastrigin::initialize()");
		}
		for (int i = 0; i < 12; ++i) {
			Real x0, x1;
			in >> x0 >> x1;
			var_data[i][0] = x0;
			var_data[i][1] = x1;
		}
		in.close();
		for (auto &i : var_data) {
			setOriginalGlobalOpt(i.data());
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag modified_rastrigin::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;

		for (int i = 0; i < m_num_vars; ++i) {
			s += 10 + 9 * cos(2 * OFEC_PI*m_k[i] * x[i]);
		}
		obj[0] = s + m_bias;  // note
		return EvalTag::Normal;
	}
	
}
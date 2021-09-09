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
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "waves.h"
namespace OFEC {
	
	waves::waves(const ParamMap &v) : 
		waves((v.at("problem name")), 2, 1) {
		
		
	}
	waves::waves(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void waves::initialize() {
		std::vector<std::pair<Real, Real>> data;
		data.push_back(std::make_pair(-0.9, 1.2));
		data.push_back(std::make_pair(-1.2, 1.2));
		setDomain(data);
		setInitialDomain(std::move(data));
		m_opt_mode[0] = optimization_mode::Maximization;
		m_objective_accuracy = 0.15;
		m_variable_accuracy = 1.e-3;
		m_variable_monitor = true;
		//one global optimum+9 local optimum

		std::ifstream in;
		std::stringstream ss;
		std::vector<std::vector<Real>> var_data(10, std::vector<Real>(m_num_vars));
		ss << g_working_dir << "/instance/problem/continuous/global/classical/data/" << m_name << "_Opt_" << m_num_vars << "Dim.txt";
		in.open(ss.str().c_str());
		if (!in)		throw myexcept("cannot open data file@waves::initialize()");

		for (int i = 0; i < 10; ++i) {
			Real x0, x1, obj;
			in >> x0 >> x1 >> obj;
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
	EvalTag waves::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s, t;

		t = x[1] * x[1];
		s = pow(0.3*x[0], 3) + 3.5*x[0] * pow(x[1], 3) - 4.7*cos(3 * x[0] - (2 + x[0])*t)*sin(2.5*x[0] * OFEC_PI);
		obj[0] = s + m_bias;
		return EvalTag::Normal;
	}
	
}
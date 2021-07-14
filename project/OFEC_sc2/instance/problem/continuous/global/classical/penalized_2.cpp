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
*************************************************************************/

#include "penalized_2.h"
namespace OFEC {
	
	penalized_2::penalized_2(const ParamMap &v) : 
		penalized_2((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	penalized_2::penalized_2(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void penalized_2::initialize() {
		m_variable_monitor = true;
		setDomain(-50, 50);
		setInitialDomain(-50., 50.);
		std::vector<Real> v(m_num_vars, 1.0);
		setOriginalGlobalOpt(v.data());
		m_optima = m_original_optima;
		m_initialized = true;
	}


	EvalTag penalized_2::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real s = 0;

		for (int i = 0; i < m_num_vars - 1; i++)
			s += (x[i] - 1)*(x[i] - 1)*(1 + sin(3 * OFEC_PI*x[i + 1])*sin(3 * OFEC_PI*x[i + 1]));
		s += (x[m_num_vars - 1] - 1)*(x[m_num_vars - 1] - 1)*(1 + sin(2 * OFEC_PI*x[m_num_vars - 1])*sin(2 * OFEC_PI*x[m_num_vars - 1])) + sin(3 * OFEC_PI*x[0])*sin(3 * OFEC_PI*x[0]);
		s = s*0.1;
		for (int i = 0; i < m_num_vars; i++)
			s += u(x[i], 5, 100, 4);

		obj[0] = s + m_bias;
		return EvalTag::Normal;

	}
	Real penalized_2::u(Real x, Real a, Real k, Real m)const {
		if (x > a) return k*pow(x - a, m);
		else if (x < -a) return k*pow(-x - a, m);
		else return 0;
	}
	
}

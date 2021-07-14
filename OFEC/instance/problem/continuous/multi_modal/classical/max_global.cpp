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

#include "max_global.h"

namespace OFEC {
	
	max_global::max_global(const ParamMap &v) :
		max_global((v.at("problem name")), 1, 1) {
		
		
	}
	max_global::max_global(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void max_global::initialize() {
		setDomain(0, 1);
		setInitialDomain(0, 1);
		m_opt_mode[0] = optimization_mode::Maximization;
		m_variable_monitor = true;
		 //5 gopt

		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;
		std::vector<std::vector<Real>> var_data = { { 0.5f },{ 0.1f },{ 0.3f },{ 0.7f },{0.9f} };
		for (auto &i : var_data) {
			setOriginalGlobalOpt(i.data());
		}

		m_optima = m_original_optima;
		m_initialized = true;
	}

	EvalTag max_global::evaluateObjective(Real *x, std::vector<Real> &obj) {

		obj[0] = pow(sin(5 * OFEC_PI*x[0]), 6.) + m_bias;
		return EvalTag::Normal;
	}
	
}
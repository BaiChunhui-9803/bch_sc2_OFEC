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

#include "quartic_noisy.h"
namespace OFEC {
	
	quartic_noisy::quartic_noisy(const ParamMap &v) :
		quartic_noisy((v.at("problem name")), (v.at("number of variables")), 1) {


	}
	quartic_noisy::quartic_noisy(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	
	void quartic_noisy::initialize() {
		m_variable_monitor = true;
		setDomain(-1.28, 1.28);
		setInitialDomain(-1.28, 1.28);
		setOriginalGlobalOpt();

		setGlobalOpt();
		m_variable_accuracy = 1.0e-2;
		m_initialized = true;
	}

	EvalTag quartic_noisy::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real fitness = 0;

		for (int i = 0; i < m_num_vars; i++) {
			fitness += (i + 1)*pow(x[i], 4);
		}
		fitness += global::ms_global->m_uniform[caller::Problem]->next();

		obj[0] = fitness + m_bias;
		return EvalTag::Normal;
	}
	
}
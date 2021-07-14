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

#include "shaffer.h"
namespace OFEC {

	shaffer::shaffer(const ParamMap &v) :
		shaffer((v.at("problem name")), 2, 1) {
		
		
	}
	shaffer::shaffer(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void shaffer::initialize() {
		setDomain(-15, 15);
		setInitialDomain(-15, 15);
		m_variable_accuracy = 0.1;
		m_objective_accuracy = 1.e-6;
		m_variable_monitor = true;
		m_opt_mode[0] = optimization_mode::Maximization;

		std::vector<Real> var_data = { 0.0, 0.0 };
		setOriginalGlobalOpt(var_data.data());

		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag shaffer::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real s, t = x[0] * x[0] + x[1] * x[1];
		s = 0.5 + (0.5 - pow(sin(sqrt(0.0001 + t)), 2)) / pow(1 + 0.001*t*t, 2);
		obj[0] = s + m_bias;
		return EvalTag::Normal;
	}
	
}
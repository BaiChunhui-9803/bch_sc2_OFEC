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

#include "center_peak.h"
namespace OFEC {
	
	center_peak::center_peak(const ParamMap &v) :
		center_peak((v.at("problem name")), 2, 1) {
		
	}
	center_peak::center_peak(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void center_peak::initialize() {
		setDomain(-2, 2);
		setInitialDomain(-2, 2);
		m_opt_mode[0] = optimization_mode::Maximization;

		 //1 gopt + 4 lopt
		m_variable_monitor = true;
		m_objective_accuracy = 1.e-5;
		m_variable_accuracy = 0.2;

		std::vector<std::vector<Real>> var_data = { {-6.02513e-012f, -2.41155e-012f}, {2, -2}, {-2, 2}, {-2, -2}, {2, 2} };

		for (auto &i : var_data) {
			setOriginalGlobalOpt(i.data());
		}

		m_optima = m_original_optima;
		m_initialized = true;
		
	}
	EvalTag center_peak::evaluateObjective(Real *x, std::vector<Real> &obj) {

		Real s;
		s = 3 * sin(0.5*OFEC_PI*x[0] + 0.5*OFEC_PI)*(2 - sqrt(x[0] * x[0] + x[1] * x[1])) / 4;
		obj[0] = s + m_bias;
		return EvalTag::Normal;
	}
	
}
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

#include "scaffer_F6.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void ScafferF6::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		m_opt_mode[0] = OptMode::Minimize;
		setOriginalGlobalOpt();
		setGlobalOpt();
		m_variable_accuracy = 1.0e-2;
	}

	void ScafferF6::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real fitness = 0;
		for (size_t i = 0; i < m_num_vars-1; ++i) {
			fitness += 0.5 + (sin(sqrt(x[i] * x[i] + x[i+1] * x[i+1]))*sin(sqrt(x[i] * x[i] + x[i+1] * x[i+1])) - 0.5) / ((1 + 0.001*(x[i] * x[i] + x[i+1] * x[i+1]))*(1 + 0.001*(x[i] * x[i] + x[i+1] * x[i+1])));
		}
		obj[0] = fitness + m_bias;
	}
	
}
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

#include "griewank.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void Griewank::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_opt_mode[0] = OptMode::Minimize;
		setDomain(-600, 600);
		setOriginalGlobalOpt();
		setGlobalOpt();
		m_variable_accuracy = 1.0e-2;
	}

	void Griewank::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		Real result = 0;

		Real s1 = 0, s2 = 1;
		for (int i = 0; i < m_num_vars; i++) {
			s1 += x[i] * x[i] / 4000.;
			s2 *= cos(x[i] / sqrt((Real)(i + 1)));
		}
		result = s1 - s2 + 1. + m_bias;

		obj[0] = result;
	}
	
}
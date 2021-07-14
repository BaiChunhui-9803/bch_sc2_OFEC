
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

#include "ackley.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void Ackley::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-32.768, 32.768);
		m_opt_mode[0] = OptMode::Minimize;		
		setOriginalGlobalOpt();
		setGlobalOpt();
	}

	void Ackley::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		Real s1 = 0, s2 = 0;

		for (int i = 0; i < m_num_vars; i++) {
			s1 += x[i] * x[i];
			s2 += cos(2 * OFEC_PI*x[i]);
		}

		obj[0] = -20 * exp(-0.2*sqrt(s1 / m_num_vars)) - exp(s2 / m_num_vars) + 20 + OFEC_E + m_bias;
	}
	
}
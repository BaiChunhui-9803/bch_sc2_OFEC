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

#include "sphere_noisy.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void SphereNoisy::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-100., 100.);
		m_opt_mode[0] = OptMode::Minimize;
		setOriginalGlobalOpt();
		setGlobalOpt();
	}

	void SphereNoisy::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		Real fit = 0;

		Real noise;
		for (int i = 0; i < m_num_vars; i++) {
			noise = 0.01 * GET_RND(m_id_rnd).uniform.next();
			fit += (x[i] + noise)*(x[i] + noise);
		}
		obj[0] = fit + m_bias;
	}
	
}
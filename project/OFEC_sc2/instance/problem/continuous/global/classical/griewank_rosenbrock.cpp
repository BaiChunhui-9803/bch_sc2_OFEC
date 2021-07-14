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

#include "griewank_rosenbrock.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void GriewankRosenbrock::initialize_() {
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		m_opt_mode[0] = OptMode::Minimize;
		setDomain(-5, 5);
		setOriginalGlobalOpt();
		setGlobalOpt();
	}

	void GriewankRosenbrock::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		Real result = 0;
		for (size_t i = 0; i < m_num_vars; ++i) {
			Real result_f2 = 0;
			Real result_f8 = 0;
			Real x_front = x[i] + 1;
			Real x_back = x[(i + 1) % m_num_vars] + 1;

			result_f2 += 100 * pow((x_back - x_front * x_front), 2.0) + (x_front - 1) * (x_front - 1);
			result_f8 += result_f2 * result_f2 / 4000.0 - cos(result_f2 / sqrt((Real)(i + 1))) + 1;
			result += result_f8;
		}

		result += m_bias;
		obj[0] = result;
	}
	
}
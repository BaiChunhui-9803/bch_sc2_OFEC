/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "HGBat.h"
namespace OFEC {

	HGBat::HGBat(const ParamMap &v) :
		HGBat((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	HGBat::HGBat(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		
	}

	void HGBat::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);
		setOriginalGlobalOpt();

		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag HGBat::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		size_t i;

		Real alpha, r2, sum_x;
		alpha = 1.0 / 4.0;

		r2 = 0.0;
		sum_x = 0.0;
		for (i = 0; i<m_num_vars; ++i)
		{
			x[i] = x[i] - 1.0;//shift to orgin
			r2 += x[i] * x[i];
			sum_x += x[i];
		}
		obj[0] = pow(fabs(pow(r2, 2.0) - pow(sum_x, 2.0)), 2 * alpha) + (0.5*r2 + sum_x) / m_num_vars + 0.5;

		obj[0] += m_bias;
		return EvalTag::Normal;
	}

}
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

#include "rosenbrock.h"

namespace OFEC {
	
	Rosenbrock::Rosenbrock(const ParamMap &v) :
		Rosenbrock((v.at("problem name")), (v.at("number of variables"))) {}

	Rosenbrock::Rosenbrock(const std::string &name, size_t num_vars) :
		Problem(name), Function(name, num_vars) {}

	void Rosenbrock::initialize() {
		setDomain(-2.048, 2.048);
		std::vector<Real> v(m_num_vars, 1);
		setOriginalGlobalOpt(v.data());
		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag Rosenbrock::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real fitness = 0;

		for (int i = 0; i < m_num_vars - 1; i++) {
			fitness += 100 * (x[i + 1] - x[i] * x[i])*(x[i + 1] - x[i] * x[i]) + (x[i] - 1)*(x[i] - 1);
		}

		obj[0] = fitness + m_bias;
		return EvalTag::Normal;
	}
	
}
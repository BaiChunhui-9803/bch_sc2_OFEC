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

#include "schwefel_1_2.h"
namespace OFEC {
	
	Schwefel_1_2::Schwefel_1_2(const ParamMap &v) :
		Schwefel_1_2((v.at("problem name")), (v.at("number of variables"))) {}

	Schwefel_1_2::Schwefel_1_2(const std::string &name, size_t num_vars) :
		Problem(name), Function(name, num_vars) {}

	void Schwefel_1_2::initialize() {
		setDomain(-100, 100);
		setOriginalGlobalOpt();
		setGlobalOpt();
		m_initialized = true;
	}


	EvalTag Schwefel_1_2::evaluateObjective(Real *x, std::vector<Real> &obj) {
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
			for (int j = 0; j <= i; j++)
				s1 += x[j];
			s2 += s1*s1;
			s1 = 0;
		}

		obj[0] = s2 + m_bias;
		return EvalTag::Normal;
	}
	
}
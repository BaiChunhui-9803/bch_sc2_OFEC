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

#include "schwefel.h"

namespace OFEC {
	
	schwefel::schwefel(const ParamMap &v) :
		schwefel((v.at("problem name")), (v.at("number of variables")), 1)
	{

	}
	schwefel::schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		
	}

	void schwefel::initialize()
	{
		m_variable_monitor = true;
		setDomain(-500, 500);
		setInitialDomain(-500, 500);
		std::vector<Real> v(m_num_vars, 420.9687);
		setOriginalGlobalOpt(v.data());
		
		setGlobalOpt();
		m_variable_accuracy = 1.0e-2;
		m_initialized = true;
	}

	EvalTag schwefel::evaluateObjective(Real *x, std::vector<Real> &obj)
	{
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		Real fitness = 0;

		for (int i = 0; i < m_num_vars; i++)
		{
			fitness += -x[i] * sin(sqrt(fabs(x[i])));
		}

		obj[0] = fitness + m_bias;
		return EvalTag::Normal;
	}
	
}
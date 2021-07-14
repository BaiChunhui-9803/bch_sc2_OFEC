
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

#include "bent_cigar.h"
namespace OFEC {

	bent_cigar::bent_cigar(const ParamMap &v) :
		bent_cigar((v.at("problem name")), (v.at("number of variables")), 1) {
		
	}
	bent_cigar::bent_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		
	}

	void bent_cigar::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);

		setOriginalGlobalOpt();

		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag bent_cigar::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		size_t i;

		obj[0] = x[0] * x[0];
		for (i = 1; i< m_num_vars; ++i)
		{
			obj[0] += pow(10.0, 6.0) * x[i] * x[i];
		}

		obj[0] += m_bias;
		return EvalTag::Normal;
	}

}

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

#include "rotated_bent_cigar.h"
namespace OFEC {

	rotated_bent_cigar::rotated_bent_cigar(const ParamMap &v) :
		rotated_bent_cigar((v.at("problem name")), (v.at("number of variables")), 1) {
		
	}
	rotated_bent_cigar::rotated_bent_cigar(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		bent_cigar(name, size_var, 1) {
		
	}

	void rotated_bent_cigar::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);

		setOriginalGlobalOpt();

		loadRotation("/instance/problem/continuous/global/CEC2005/data/");
		
		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag rotated_bent_cigar::evaluateObjective(Real *x, std::vector<Real> &obj) {
		return bent_cigar::evaluateObjective(x, obj);

	}

}
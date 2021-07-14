
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

#include "rotated_discus.h"
namespace OFEC {

	rotated_discus::rotated_discus(const ParamMap &v) :
		rotated_discus((v.at("problem name")), (v.at("number of variables")), 1) {
		
	}
	rotated_discus::rotated_discus(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		discus(name, size_var, 1) {
		
	}

	void rotated_discus::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);

		setOriginalGlobalOpt();

		loadRotation("/instance/problem/continuous/global/CEC2005/data/");

		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag rotated_discus::evaluateObjective(Real *x, std::vector<Real> &obj) {
		return discus::evaluateObjective(x, obj);

	}

}
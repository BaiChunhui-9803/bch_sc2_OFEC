
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

#include "rotated_rosenbrock.h"
namespace OFEC {

	rotated_rosenbrock::rotated_rosenbrock(const ParamMap &v) :
		rotated_rosenbrock((v.at("problem name")), (v.at("number of variables")), 1) {
		
	}
	rotated_rosenbrock::rotated_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		rosenbrock(name, size_var, 1) {
		
	}

	void rotated_rosenbrock::initialize() {
		m_variable_monitor = true;
		setDomain(-2.048, 2.048);
		setInitialDomain(-2.048, 2.048);
		std::vector<Real> v(m_num_vars, 1);
		setOriginalGlobalOpt(v.data());

		setConditionNumber(1);
		loadRotation("/instance/problem/continuous/global/CEC2005/data/");
	
		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag rotated_rosenbrock::evaluateObjective(Real *x, std::vector<Real> &obj) {
		return rosenbrock::evaluateObjective(x, obj);

	}

}
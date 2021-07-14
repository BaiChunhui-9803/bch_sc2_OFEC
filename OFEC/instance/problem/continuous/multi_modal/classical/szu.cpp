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
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#include "szu.h"
//* 
//* F(vec3{X})=\sum_{i=1}^{D}{-x_i^2}
//*
namespace OFEC {
	
	szu::szu(const ParamMap &v) :
		szu((v.at("problem name")), (v.at("number of variables")), 1) {
		
	}
	szu::szu(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void szu::initialize() {
		setDomain(-5.0, 5.0);
		setInitialDomain(-5.0, 5.0);
		
		m_obj_minmax_monitored = true;
		
		std::vector<std::vector<Real>> gobj;

		if (m_num_vars == 2) {
			gobj.push_back(std::vector<Real>(1, -156.66));
		}
		else if (m_num_vars == 3) {
			gobj.push_back(std::vector<Real>(1, -235.0));
		}
		else if (m_num_vars == 4) {
			gobj.push_back(std::vector<Real>(1, -313.33));
		}
		else if (m_num_vars == 5) {
			gobj.push_back(std::vector<Real>(1, -391.66));
		}
		else if (m_num_vars == 6) {
			gobj.push_back(std::vector<Real>(1, -469.99));
		}
		else if (m_num_vars == 7) {
			gobj.push_back(std::vector<Real>(1, -548.33));
		}
		else if (m_num_vars == 8) {
			gobj.push_back(std::vector<Real>(1, -626.66));
		}
		else if (m_num_vars == 9) {
			gobj.push_back(std::vector<Real>(1, -704.99));
		}

		if (m_num_vars >= 2 && m_num_vars <= 9) {
			m_original_optima.append(gobj[0]);
		}
		else {
		}
		m_optima = m_original_optima;
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-2;
		//setObjSet();
		m_initialized = true;
	}
	EvalTag szu::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;
		size_t i;

		for (i = 0; i < m_num_vars; ++i) {
			s += pow(x[i], 4) - 16 * x[i] * x[i] + 5 * x[i];
		}
		obj[0] = s;
		return EvalTag::Normal;
	}
	
}
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
*  Paper; A sequential niching memetic algorithm for Continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for Continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************
*  LaTex:F(x)=\frac{1}{D}\sum^D_{i=1}{sin(10\log(x_i))}
*******************************************************************************************/

#include "vincent.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	void Vincent::initialize_() { // note
		Function::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(0.25, 10.); // note
		m_opt_mode[0] = OptMode::Maximize;
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
		m_variable_niche_radius = 0.2;

		size_t num_solution = pow(6, m_num_vars);
		
		std::vector<std::vector<Real>> obj_data((int)num_solution, std::vector<Real>(m_num_objs, 1));
		for (auto &i : obj_data)
			m_original_optima.appendObj(i);
		m_optima = m_original_optima;
		m_optima.setObjectiveGiven(true);
	}

	void Vincent::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Real s = 0;

		for (int i = 0; i < m_num_vars; ++i) {
			s += sin(10 * log(x[i]));
		}
		s /= m_num_vars;
		obj[0] = s + m_bias;  // note
	}
}
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

#include "modified_schwefel.h"
namespace OFEC {

	modified_schwefel::modified_schwefel(const ParamMap &v) :
		modified_schwefel((v.at("problem name")), (v.at("number of variables")), 1) {

	}
	modified_schwefel::modified_schwefel(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, 1) {
		
	}

	void modified_schwefel::initialize() {
		m_variable_monitor = true;
		setDomain(-100., 100.);
		setInitialDomain(-100., 100.);
		setOriginalGlobalOpt();

		setGlobalOpt();
		m_initialized = true;
	}

	EvalTag modified_schwefel::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);

		size_t i;

		Real tmp;
		obj[0] = 0.0;

		for (i = 0; i<m_num_vars; i++)
		{
			x[i] += 4.209687462275036e+002;
			if (x[i]>500)
			{
				obj[0] -= (500.0 - fmod(x[i], 500))*sin(pow(500.0 - fmod(x[i], 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_num_vars;
			}
			else if (x[i]<-500)
			{
				obj[0] -= (-500.0 + fmod(fabs(x[i]), 500))*sin(pow(500.0 - fmod(fabs(x[i]), 500), 0.5));
				tmp = (x[i] + 500.0) / 100;
				obj[0] += tmp*tmp / m_num_vars;
			}
			else
				obj[0] -= x[i] * sin(pow(fabs(x[i]), 0.5));
		}
		obj[0] += 4.189828872724338e+002*m_num_vars;
		obj[0] += m_bias;
		return EvalTag::Normal;
	}

}
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


#include "DynDE_individual.h"
#include <cmath>

namespace OFEC {
	EvalTag DynDE_individual::brownian(const solution_type &best, Real sigma) {
		for (size_t i = 0; i < m_var.size(); ++i) {
			m_var[i] = (best.variable()[i]) + global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0, sigma);
		}

		check_boundary_violation();

		return evaluate();
	}

	EvalTag DynDE_individual::quantum(const solution_type &best, Real rcloud) {
		const auto dim = best.variable().size();
		std::vector<Real> x(dim, 0);
		Real dis = 0;
		for (size_t i = 0; i < dim; ++i) {
			x[i] = global::ms_global->m_normal[caller::Algorithm]->next();
			dis += x[i] * x[i];
		}
		dis = sqrt(dis);
		const auto r = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(0, rcloud);
		for (size_t i = 0; i < dim; ++i) {
			m_var[i] = (best.variable()[i]) + r * x[i] / dis;
		}
		x.clear();
		check_boundary_violation();
		return evaluate();
	}
	EvalTag DynDE_individual::entropy(Real sigma) {
		for (auto& i : m_var)
			i = i + global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0, sigma);

		check_boundary_violation();
		return evaluate();
	}
}
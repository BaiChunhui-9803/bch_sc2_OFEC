//Register Schwefel_1_2 "Classic_schwefel_1_2" GOP,ConOP,SOP

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
// Created: 21 July 2011
// Last modified:
#ifndef OFEC_SCHWEFEL_1_2_H
#define OFEC_SCHWEFEL_1_2_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	class Schwefel_1_2 : public Function {
	public:
		Schwefel_1_2(const ParamMap &v);
		Schwefel_1_2(const std::string &name, size_t num_vars);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	};
	
}
#endif // !OFEC_SCHWEFEL_1_2_H

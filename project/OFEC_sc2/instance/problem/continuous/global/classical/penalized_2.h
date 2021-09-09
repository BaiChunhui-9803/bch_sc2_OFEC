//Register penalized_2 "Classic_penalized_2" GOP,ConOP,SOP

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

#ifndef OFEC_PENALIZED_2_H
#define OFEC_PENALIZED_2_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class penalized_2 : public function
	{
	public:
		penalized_2(const ParamMap &v);
		penalized_2(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		Real u(Real x, Real a, Real k, Real m)const;
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	private:
	};
	
}
#endif // !OFEC_PENALIZED_2_H
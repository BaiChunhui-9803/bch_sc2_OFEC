//Register expanded_six_hump_camel_back "Classic_six_hump_camel_back_expanded" MMOP,ConOP,SOP

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
******************************************************************************************
*  Paper: Problem Definitions and Evaluation Criteria for the CEC 2015
*  Competition on Single Objective Multi-Niche Optimization.
*******************************************************************************************/


#ifndef OFEC_EXPANDED_SIX_HUMP_CAMEL_BACK_H
#define OFEC_EXPANDED_SIX_HUMP_CAMEL_BACK_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class expanded_six_hump_camel_back : public function
	{
	public:
		expanded_six_hump_camel_back(const ParamMap &v);
		expanded_six_hump_camel_back(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	private:
	};

}
#endif // !OFEC_EXPANDED_SIX_HUMP_CAMEL_BACK_H




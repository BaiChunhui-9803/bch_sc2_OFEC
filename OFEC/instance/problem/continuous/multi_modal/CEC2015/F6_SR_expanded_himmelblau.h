//Register CEC2015_MMOP_F06 "MMOP_CEC2015_F06" MMOP,ConOP,SOP

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


#ifndef OFEC_F6_SR_EXPANDED_HIMMELBLAU_H
#define OFEC_F6_SR_EXPANDED_HIMMELBLAU_H

#include "CEC2015_function.h"

namespace ofec {
	namespace CEC2015 {
		class F6_SR_expanded_himmelblau final : public CEC2015_function
		{
		public:
			F6_SR_expanded_himmelblau(const ParamMap &v);
			F6_SR_expanded_himmelblau(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
		private:
		};
	}
	using CEC2015_MMOP_F06 = CEC2015::F6_SR_expanded_himmelblau;
}
#endif // !OFEC_F6_SR_EXPANDED_HIMMELBLAU_H




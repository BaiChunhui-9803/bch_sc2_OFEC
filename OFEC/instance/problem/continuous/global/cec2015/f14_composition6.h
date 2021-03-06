//Register CEC2015_GOP_F14 "GOP_CEC2015_F14" GOP,ConOP,SOP

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
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015
*  Competition on Learning-based Real-Parameter Single Objective
*  Optimization.
************************************************************************/

#ifndef OFEC_F14_GLOBAL_COMPOSITION6_H
#define OFEC_F14_GLOBAL_COMPOSITION6_H

#include "composition.h"

namespace ofec {
	namespace CEC2015 {
		class F14_global_composition6 final : public composition_2015
		{
		public:
			F14_global_composition6(const ParamMap &v);
			F14_global_composition6(const std::string &name, size_t size_var, size_t size_obj);

			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction();

		protected:

		};
	}
	using CEC2015_GOP_F14 = CEC2015::F14_global_composition6;
}
#endif // !OFEC_F14_GLOBAL_COMPOSITION6_H





//Register CEC2015_GOP_F11 "GOP_CEC2015_F11" GOP,ConOP,SOP

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

#ifndef OFEC_F11_GLOBAL_COMPOSITION3_H
#define OFEC_F11_GLOBAL_COMPOSITION3_H

#include "composition.h"

namespace ofec {
	namespace CEC2015 {
		class F11_global_composition3 final : public composition_2015
		{
		public:
			F11_global_composition3(const ParamMap &v);
			F11_global_composition3(const std::string &name, size_t size_var, size_t size_obj);

			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction();
			
		protected:
			
		};
	}
	using CEC2015_GOP_F11 = CEC2015::F11_global_composition3;
}
#endif // !OFEC_F11_GLOBAL_COMPOSITION3_H




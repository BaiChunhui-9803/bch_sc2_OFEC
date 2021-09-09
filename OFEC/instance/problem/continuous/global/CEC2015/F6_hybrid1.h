//Register CEC2015_GOP_F06 "GOP_CEC2015_F06" GOP,ConOP,SOP

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
#ifndef OFEC_F6_HYBRID1_H
#define OFEC_F6_HYBRID1_H

#include "../../expensive/CEC2015/hybrid.h"
namespace OFEC {
	namespace CEC2015 {
		class F6_hybrid1 final : public hybrid
		{
		public:
			F6_hybrid1(const ParamMap &v);
			F6_hybrid1(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction();
		private:
		};
	}
	using CEC2015_GOP_F06 = CEC2015::F6_hybrid1;
}

#endif // !OFEC_F6_HYBRID1_H






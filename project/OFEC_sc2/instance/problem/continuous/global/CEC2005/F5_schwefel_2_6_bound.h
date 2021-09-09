//Register CEC2005_GOP_F05 "GOP_CEC2005_F05" GOP,ConOP,SOP

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


#ifndef OFEC_F5_SCHWEFEL_2_6_BOUND_H
#define OFEC_F5_SCHWEFEL_2_6_BOUND_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2005 {
		class Schwefel_2_6_Bound final : public Function {
		public:
			Schwefel_2_6_Bound(const ParamMap &v);
			Schwefel_2_6_Bound(const std::string &name, size_t num_vars);
			void initialize();
		protected:
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void load_data(const std::string & path);

			std::vector<Real> m_b;
			std::vector<std::vector<Real>> m_a;
		};
	}
	using CEC2005_GOP_F05 = CEC2005::Schwefel_2_6_Bound;
}
#endif // ! OFEC_F5_SCHWEFEL_2_6_BOUND_H
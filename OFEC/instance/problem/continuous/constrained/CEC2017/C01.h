//Register CEC2017_COP_F01 "COP_CEC2017_F01" COP,ConOP,SOP

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


#ifndef OFEC_C01_H
#define OFEC_C01_H

#include "../../../../../core/problem/continuous/function.h"
#include <functional>

namespace OFEC {
	namespace CEC2017 {
		class C01 final: public function
		{
		public:
			C01(const ParamMap &v);
			C01(const std::string &name, size_t size_var, size_t size_obj);
			void initialize() override;
		protected:
			void evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) override;
		private:
			
		};
	}
	using CEC2017_COP_F01 = CEC2017::C01;
}
#endif // ! OFEC_C01_H
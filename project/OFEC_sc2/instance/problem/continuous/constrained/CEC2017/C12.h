//Register CEC2017_COP_F12 "COP_CEC2017_F12" COP,ConOP,SOP

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


#ifndef OFEC_C12_H
#define OFEC_C12_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2017 {
		class C12 final: public function
		{
		public:
			C12(const ParamMap &v);
			C12(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:
			void evaluate_obj_nd_con(Real *x, std::vector<Real>& obj, std::vector<Real> &con) override;
		private:

		};
	}
	using CEC2017_COP_F12 = CEC2017::C12;
}
#endif // ! OFEC_C12_H








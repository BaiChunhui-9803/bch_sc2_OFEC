//Register CEC2005_GOP_F14 "GOP_CEC2005_F14" GOP,ConOP,SOP

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


#ifndef OFEC_F14_SHIFTED_ROTATED_EXPANDED_SCAFFER_F6_H
#define OFEC_F14_SHIFTED_ROTATED_EXPANDED_SCAFFER_F6_H

#include "../classical/scaffer_F6.h"

namespace OFEC {
	namespace CEC2005 {
		class ShiftedRotatedExpandedScafferF6 final : public ScafferF6 {
		public:
			ShiftedRotatedExpandedScafferF6(const ParamMap &v);
			ShiftedRotatedExpandedScafferF6(const std::string &name, size_t num_vars);
			void initialize();
		};
	}
	using CEC2005_GOP_F14 = CEC2005::ShiftedRotatedExpandedScafferF6;
}
#endif // ! OFEC_F14_SHIFTED_ROTATED_EXPANDED_SCAFFER_F6_H



//Register HGBat "Classic_HGBat" GOP,ConOP,SOP

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

#ifndef OFEC_HGBAT_H
#define OFEC_HGBAT_H

#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {

	class HGBat : public function
	{
	public:
		HGBat(const ParamMap &v);
		HGBat(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	private:
	};

}
#endif // !OFEC_HGBAT_H


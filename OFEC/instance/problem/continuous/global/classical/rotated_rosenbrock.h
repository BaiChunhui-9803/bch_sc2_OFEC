//Register rotated_rosenbrock "Classic_rosenbrock_rotated" GOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_ROTATED_ROSENBROCK_H
#define OFEC_ROTATED_ROSENBROCK_H

#include "../../../../../instance/problem/continuous/global/classical/rosenbrock.h"
namespace OFEC {

	class rotated_rosenbrock : public rosenbrock
	{
	public:
		rotated_rosenbrock(const ParamMap &v);
		rotated_rosenbrock(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	private:
	};

}
#endif // !OFEC_ROTATED_ROSENBROCK_H

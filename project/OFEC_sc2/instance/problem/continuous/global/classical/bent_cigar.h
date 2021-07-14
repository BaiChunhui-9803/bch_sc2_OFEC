//Register bent_cigar "Classic_bent_cigar" GOP,ConOP,SOP

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

#ifndef OFEC_BENT_CIGAR_H
#define OFEC_BENT_CIGAR_H

#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {

	class bent_cigar : public function
	{
	public:
		bent_cigar(const ParamMap &v);
		bent_cigar(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	private:
	};

}
#endif // !OFEC_BENT_CIGAR_H

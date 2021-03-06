//Register ModifiedVincent "Classic_vincent_modified" MMOP,ConOP,SOP

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
******************************************************************************************
*  Paper: Problem Definitions and Evaluation Criteria for the CEC 2015
*  Competition on Single Objective Multi-Niche Optimization.
*******************************************************************************************/

#ifndef OFEC_MODIFIED_VINCENT_H
#define OFEC_MODIFIED_VINCENT_H

#include "../metrics_mmop.h"

namespace ofec {
	class ModifiedVincent : public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}
#endif // !OFEC_MODIFIED_VINCENT_H






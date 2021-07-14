//Register ANDE "ANDE" ConOP,SOP,GOP,MMOP

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// Created on 20th May, 2020 by Junchen Wang
/*
Z. Wang et al., "Automatic Niching Differential Evolution With Contour Prediction Approach for Multimodal Optimization Problems," 
in IEEE Transactions on Evolutionary Computation, vol. 24, no. 1, pp. 114-128, Feb. 2020, doi: 10.1109/TEVC.2019.2910721.
*/

#ifndef OFEC_ANDE_H
#define OFEC_ANDE_H

#include "../../../../core/algorithm/algorithm.h"
#include "../population.h"
#include "../../../../utility/clustering/apc.h"

namespace OFEC {
	class ANDE : public algorithm {
		DE::population<> m_pop;
		APC<DE::individual> m_apc;
		size_t m_MaxFEs = 1000000;
	public:
		ANDE(param_map& v);
		void initialize() override;
		void run_() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif
	private:
		void CPA(const std::vector<size_t> &cluster);
		void TLLS(const std::vector<std::vector<size_t>>& clusters);
	};
}


#endif // !OFEC_ANDE_H


//Register NEA2 "NEA2" GOP,MMOP,ConOP,SOP

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
*-------------------------------------------------------------------------------
* NBC (Nearest-Better Clustering)
* Preuss, Mike.
* "Niching the CMA-ES via nearest-better clustering."
* Proceedings of the 12th annual conference companion on Genetic and evolutionary computation. 2010.
*-------------------------------------------------------------------------------
* Implemented by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/9/23
*********************************************************************************/

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../../../utility/clustering/nbc.h"
#include "../CMA_ES/CMA_ES.h"

namespace OFEC {
	class NEA2 : public algorithm {
		using pop_type = cmaes_pop;
		using ind_type = individual<>;
	public:
		NEA2(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif
	protected:
		void run_() override;
	private:
		void add_subpops();
		bool stopTolFun(pop_type& subpop);
		multi_population<pop_type> m_subpops;
		NBC<ind_type> m_nbc;
		size_t m_sizepop;
	};
}


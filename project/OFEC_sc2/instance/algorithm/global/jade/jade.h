//Register JADE "JADE" GOP,ConOP,SOP

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
// updated Mar 28, 2018 by Li Zhou
// Updated on 15th Aug, 2019 by Junchen Wang

/*
Paper: J. Zhang and C. Arthur, ��JADE: Adaptive differential evolution with
optional external archive,�� IEEE Trans. Evol. Comput., vol. 13, no. 5,
pp. 945�C958, Oct. 2009.
*/
#ifndef JADE_H
#define JADE_H

#include "../population.h"
#include "../individual.h"
#include "../../../../core/algorithm/algorithm.h"

namespace OFEC {
	class JADE_individual :public DE::individual {
	public:
		using  individual::select;
		JADE_individual(size_t num_obj, size_t num_con, size_t size_var) : individual(num_obj, num_con, size_var) {}
		EvalTag select(std::vector<solution_type>& archive);
	};

	class JADE_pop :public DE::population<JADE_individual>
	{
	public:
		//using JADE_individual::solution_type;
		JADE_pop(size_t size_pop);
		EvalTag evolve() override;
	protected:
		void select_trial(size_t base_);
		void update_F();
		void update_CR();
	protected:
		///////////////////////algorithm parameters/////////////////////////////
		Real m_p;
		Real m_c;
		int m_archive_flag;
		std::vector<Solution<VarVec<Real>, Real>> m_archive;
		std::vector<Solution<VarVec<Real>, Real>*> m_candi;
		std::vector<Real> m_pcent_best;
		std::vector<int> m_pcent_best_index;
		std::vector<Real> mv_F, mv_CR;
		///////////////////////////////////////////////////////////////////////////
	};

	class JADE : public algorithm {
	public:
		JADE(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		void run_() override;
	protected:
		JADE_pop m_pop;
	};
}
#endif // JADE_H

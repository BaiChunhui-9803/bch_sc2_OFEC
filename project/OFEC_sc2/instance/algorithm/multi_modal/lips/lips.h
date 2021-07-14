//Register LIPS "LIPS" ConOP,GOP,MMOP,SOP

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
// Created at 21 January 2017
// Modifed by WangJunChen at 2 Dec 2019 

/* ---------------------------------------------------------------------------------------
LIPS: B. Y. Qu, P. N. Suganthan and S. Das, 
"A Distance-Based Locally Informed Particle Swarm Model for Multimodal Optimization," 
in IEEE Transactions on Evolutionary Computation, vol. 17, no. 3, pp. 387-402, June 2013.
doi: 10.1109/TEVC.2012.2203138
-----------------------------------------------------------------------------------------*/

#ifndef OFEC_LIPS_H
#define OFEC_LIPS_H

#include "../particle.h"
#include "../swarm.h"
#include <list>

namespace OFEC {
	class LI_swarm;
	class LI_particle : public particle {
		friend LI_swarm;
	private:
		std::vector<Real> m_pos;           // the mean position of neighborhood
		std::vector<Real> m_rdm;
		std::vector<int> m_nbr;
	public:
		LI_particle() = default;
		LI_particle(size_t num_obj, size_t num_con, size_t size_var);
		LI_particle(const Solution<>& rhs);
		void next_velocity(const Solution<>* lbest = nullptr, Real w = 0, Real c1 = 0, Real c2 = 0) override;
	};

	class LI_swarm : public swarm<LI_particle> {
	private:
		int m_M;            // the num of neighbors
		int m_max_evals = 0;    // the maximum num of evaluations
		std::vector<std::list<std::pair<Real, size_t>>> m_dis;
		void set_best_pos(int idx_ind);
		void sort_distance(int idx_ind);
	public:
		LI_swarm(size_t size_pop);
		void set_max_evals(int max_evals);
		EvalTag evolve() override;
	};

	class LIPS : public algorithm {
	private:
		LI_swarm m_pop;
	public:
		LIPS(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		void run_() override;
	};
}

#endif // !OFEC_LIPS_H


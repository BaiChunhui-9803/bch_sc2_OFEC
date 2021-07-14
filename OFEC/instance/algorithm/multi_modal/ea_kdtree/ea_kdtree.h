//Register EA_KDTree "EA-KDTree" ConOP,SOP,GOP,MMOP

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
* Nguyen, Trung Thanh and Jenkinson, Ian and Yang, Zaili,
* "An Experimental Study of Combining Evolutionary Algorithms with KD-Tree to Solving Dynamic Optimisation Problems."
* Applications of Evolutionary Computation, 2015, pp. 857-868
*-------------------------------------------------------------------------------
* Implemented by Junchen Wang (wangjunchen.chris@gmail.com) at 2020/10/14
*********************************************************************************/

#ifndef OFEC_EA_KDTREE_H
#define OFEC_EA_KDTREE_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../utility/kd-tree/kdtree_space.h"
#include "../../GA/SBX/SBX.h"
#include <set>

namespace OFEC {
	class EA_KDTree : public algorithm {
	public:
		struct attractor {
			attractor(const Solution<>& sol, const std::vector<std::pair<Real, Real>>& basin) :
				range(basin), taboo(basin), optimum(sol) {
				for (size_t j = 0; j < basin.size(); j++) {
					taboo[j].first += (sol.variable()[j] - basin[j].first) * Real(0.2);
					taboo[j].second -= (basin[j].second - sol.variable()[j]) * Real(0.2);
				}
			}
			std::vector<std::pair<Real, Real>> range;
			std::vector<std::pair<Real, Real>> taboo;
			Solution<> optimum;
		};
		using pop_type = SBX_pop<>;
	public:
		EA_KDTree(param_map& v);
		EA_KDTree(const std::string& alg_name, size_t size_pop);
		void initialize() override;
		void record() override;
		const std::map<size_t, std::unique_ptr<attractor>>& get_attractors() const { return m_attractors; }
#ifdef OFEC_DEMO
		void updateBuffer() override;
	private:
		bool m_update_buffer = false;
#endif
	protected:
		void run_() override;
	private:
		EvalTag evolve_pop();
		void basin_estimation(const Solution<>& opt, std::vector<std::pair<Real, Real>>& basin);
		void find_min_ratio_division(const attractor& atr, int dim_split, size_t idx_reg, Real& split_pivot, Real& ratio);
		void tree_construction(const attractor& atr);
		bool is_converged();
		bool is_revisiting(const std::vector<Real>& x);
		void init_exploring_ind(Solution<>& ind);
		Real dis_range(const std::pair<Real, Real>& range1, const std::pair<Real, Real>& range2);
		Real split_pivot_of_range(const std::pair<Real, Real>& range1, const std::pair<Real, Real>& range2);
		Real filled_ratio(size_t idx_reg);

		pop_type m_pop;
		size_t m_num_segs;
		KDTreeSpace::PartitioningKDTree<Real> m_kdtree;
		std::set<size_t> m_explored;
		std::vector<size_t> m_unexplored;
		std::map<size_t, std::unique_ptr<attractor>> m_attractors;
		Real m_ratio_explored;
	};
}

#endif // !OFEC_EA_KDTREE_H


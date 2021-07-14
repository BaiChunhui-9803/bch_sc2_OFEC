//Register R2PSO "R2PSO" ConOP,GOP,MMOP,SOP
//Register R3PSO "R3PSO" ConOP,GOP,MMOP,SOP
//Register R2PSO_LHC "R2PSO-LHC" ConOP,GOP,MMOP,SOP
//Register R3PSO_LHC "R3PSO-LHC" ConOP,GOP,MMOP,SOP

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
// Created: 21 September 2011
// Modified: 21 Nov 2014
// Modified: 2 Dec 2019 by WangJunChen

/* X. Li, 
"Niching Without Niching Parameters: Particle Swarm Optimization Using a Ring Topology," 
in IEEE Transactions on Evolutionary Computation, vol. 14, no. 1, pp. 150-169, Feb. 2010.
doi: 10.1109/TEVC.2009.2026270 */

#ifndef OFEC_RINGPSO_H
#define OFEC_RINGPSO_H

#include "../particle.h"
#include "../swarm.h"
#include "../../../../core/algorithm/algorithm.h"

#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	class ring_particle : public particle {
	public:
		ring_particle() = default;
		ring_particle(size_t num_obj, size_t num_con, size_t size_var) : particle(num_obj, num_con, size_var) {}
		ring_particle(const Solution<> & rhs) : particle(rhs) {}
		void next_velocity(const Solution<>* lbest = nullptr, Real w = 0, Real c1 = 0, Real c2 = 0) override;
	};

	class ring_swarm : public swarm<ring_particle> {
	public:
		ring_swarm(size_t size_pop);
	protected:
		bool m_is_neighbor_set = false;
	};

	class R2_swarm : public ring_swarm {
	public:
		R2_swarm(size_t size_pop);
		void set_neighborhood() override;
	};

	class R3_swarm : public ring_swarm {
	public:
		R3_swarm(size_t size_pop);
		void set_neighborhood() override;
	};

	class LHC_R2_swarm : public ring_swarm {
	public:
		LHC_R2_swarm(size_t size_pop);
		void set_neighborhood() override;
	};

	class LHC_R3_swarm : public ring_swarm {
	public:
		LHC_R3_swarm(size_t size_pop);
		void set_neighborhood() override;
	};

	template<typename RingSwarm>
	class RingPSO : public algorithm {
	private:
		RingSwarm m_pop;
	public:
		RingPSO(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		void run_() override;
	};

	template<typename RingSwarm>
	RingPSO<RingSwarm>::RingPSO(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	template<typename RingSwarm>
	void RingPSO<RingSwarm>::initialize() {
		m_pop.set_parameters(0.7298, 2.05, 2.05);
		m_pop.initialize();
		m_pop.evaluate();
		m_pop.initialize_pbest();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			std::cout << evals << "\t" << err << std::endl;
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			std::cout << evals << "\t" << num_opt_found << "/" << num_opt_known << std::endl;
		}
#endif
	}

	template<typename RingSwarm>
	void RingPSO<RingSwarm>::record() {
		//if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
		//	// ******* Multi-Modal Optimization *******
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
		//	size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
		//	Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
		//	Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
		//	measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		//}
		//else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
		//	// ******* Global Optimization ************
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
		//	measure::get_measure()->record(global::ms_global.get(), evals, err);
		//}
		size_t num_opt = CONTINUOUS_CAST->get_optima().number_variable();
		std::vector<Real> vals(num_opt + 1);
		vals[0] = CONTINUOUS_CAST->evaluations();
		Real dis;
		for (size_t i = 0; i < num_opt; i++) {
			dis = CONTINUOUS_CAST->get_optima().variable_min_dis(i);
			if (dis == 0)
				vals[i + 1] = -17;
			else
				vals[i + 1] = log10(dis);
		}
		measure::get_measure()->record(global::ms_global.get(), vals);
	}

	template<typename RingSwarm>
	void RingPSO<RingSwarm>::run_() {
		while (!this->terminating()) {
			m_pop.evolve();
#ifdef OFEC_DEMO
			vector<vector<Solution<>*>> pops(1);
			for (size_t i = 0; i < m_pop.size(); ++i)
				pops[0].emplace_back(&m_pop[i].solut());
			dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
			if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
				// ******* Global Optimization ************
				size_t evals = CONTINUOUS_CAST->evaluations();
				Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
				std::cout << evals << "\t" << err << std::endl;
			}
			else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
				// ******* Multi-Modal Optimization *******
				size_t evals = CONTINUOUS_CAST->evaluations();
				size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
				size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
				std::cout << evals << "\t" << num_opt_found << "/" << num_opt_known << std::endl;
			}
#endif
		}
	}

	using R2PSO = RingPSO<R2_swarm>;
	using R3PSO = RingPSO<R3_swarm>;
	using R2PSO_LHC = RingPSO<LHC_R2_swarm>;
	using R3PSO_LHC = RingPSO<LHC_R3_swarm>;
}
#endif // !OFEC_RINGPSO_H

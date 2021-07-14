#include "JADE.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	EvalTag JADE_individual::select(std::vector<solution_type>& archive) {
		m_improved = false;
		EvalTag tag = m_pu.evaluate();
		if (m_pu.dominate(*this)) {
			m_var = m_pu.variable();
			m_obj = m_pu.objective();
			m_constraint_value = m_pu.constraint_value();
			m_improved = true;
		}
		else {
			archive.push_back(*this);
		}
		return tag;
	}

	JADE_pop::JADE_pop(size_t size_pop) : DE::population<JADE_individual>(size_pop), m_candi(3, nullptr), m_pcent_best(size_pop), \
		m_pcent_best_index(size_pop), mv_F(size_pop), mv_CR(size_pop) {
		m_archive_flag = 1;
		m_p = 0.2;
		m_c = 0.1;
	}

	void JADE_pop::select_trial(size_t base_) {
		std::vector<int> candidate;
		for (size_t i = 0; i < size(); i++) {
			if (base_ != i) candidate.push_back(i);
		}
		size_t idx;
		do {
			idx = m_pcent_best_index[global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, static_cast<size_t>(size() * m_p))];
		} while (idx == base_);

		m_candi[0] = m_inds[idx].get();

		const auto it = std::find(candidate.begin(), candidate.end(), idx);
		candidate.erase(it);

		idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size());
		m_candi[1] = m_inds[candidate[idx]].get();
		candidate.erase(candidate.begin() + idx);

		idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)(candidate.size() + m_archive.size()));

		if (idx >= candidate.size()) {
			m_candi[2] = &m_archive[idx - candidate.size()];
		}
		else {
			m_candi[2] = m_inds[candidate[idx]].get();
		}

	}

	EvalTag JADE_pop::evolve() {
		update_best();
		update_CR();
		update_F();
		EvalTag tag = EvalTag::Normal;
		for (size_t i = 0; i < size(); ++i) {
			m_pcent_best[i] = m_inds[i]->objective()[0];
		}
		bool minf = global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization ? true : false;
		merge_sort(m_pcent_best, (int)size(), m_pcent_best_index, minf);
		std::vector<int> candidate(3);
		for (size_t i = 0; i < size(); ++i) {
			select_trial(i);
			m_inds[i]->mutate(mv_F[i], m_inds[i].get(), m_candi[0], m_inds[i].get(), m_candi[1], m_candi[2]);
			m_inds[i]->recombine(mv_CR[i], m_recombine_strategy);
		}
		for (size_t i = 0; i < size(); i++) {
			tag = m_inds[i]->select(m_archive);
			while (m_archive.size() > size()) {
				int ridx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)m_archive.size());
				m_archive.erase(m_archive.begin() + ridx);
			}
			//update_archive(m_pop[i]);
			if (tag != EvalTag::Normal) break;
		}
		if (tag == EvalTag::Normal) {
			m_iter++;
		}
		return tag;
	}

	void JADE_pop::update_F() {
		Real mu = 0.5;
		if (m_iter > 0) {
			Real mean = 0, mean2 = 0;
			int cnt = 0;
			for (size_t i = 0; i < size(); i++) {
				if (m_inds[i]->is_improved()) {
					cnt++;
					mean += mv_F[i];
					mean2 += mv_F[i] * mv_F[i];
				}
			}
			if (cnt > 0) {
				mean = mean2 / mean;
			}

			mu = (1 - m_c)*mu + m_c * mean;
		}
		for (size_t i = 0; i < size(); i++) {
			do {
				mv_F[i] = static_cast<Real>(global::ms_global->m_cauchy[caller::Algorithm]->next_non_standard(static_cast<Real>(mu), 0.01));
			} while (mv_F[i] <= 0);

			if (mv_F[i] > 1) mv_F[i] = 1;
		}
	}

	void JADE_pop::update_CR() {
		Real mu = 0.5;
		if (m_iter > 0) {
			Real mean = 0;
			int cnt = 0;
			for (size_t i = 0; i < size(); i++) {
				if (m_inds[i]->is_improved()) {
					cnt++;
					mean += mv_CR[i];
				}
			}
			if (cnt > 0) {
				mean /= cnt;
			}

			mu = (1 - m_c)*mu + m_c * mean;
		}
		for (size_t i = 0; i < size(); i++) {
			mv_CR[i] = static_cast<Real>(global::ms_global->m_normal[caller::Algorithm]->next_non_standard(static_cast<Real>(mu), 0.01));
			if (mv_CR[i] < 0) mv_CR[i] = 0;
			else if (mv_CR[i] > 1) mv_CR[i] = 1;
		}
	}

	JADE::JADE(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void JADE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void JADE::run_() {
		while (!terminating()) {
			m_pop.evolve();
#ifdef OFEC_DEMO
			vector<vector<Solution<>*>> pops(1);
			for (size_t i = 0; i < m_pop.size(); ++i)
				pops[0].emplace_back(&m_pop[i].solut());
			dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
		}
	}

	void JADE::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
			Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
			measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			measure::get_measure()->record(global::ms_global.get(), evals, err);
		}
	}
}


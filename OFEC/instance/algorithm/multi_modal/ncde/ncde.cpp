#include "NCDE.h"

#ifdef OFEC_DEMO
#include <buffer/buffer_alg/buffer_alg_cont.h>
extern std::unique_ptr<Demo::scene> Demo::g_buffer_alg;
#endif

namespace OFEC {
	NCDE_pop::NCDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop), m_dis(size_pop) {
		m_r = 0.1;
		m_m = static_cast<size_t>(m_r * size());
		for (auto &i : m_inds) {
			i->set_improved_flag(true);
		}
	}

	void NCDE_pop::sort_distance(size_t a) {
		if (!m_inds[a]->is_improved()) {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				if (m_inds[j]->is_improved()) {
					std::pair<Real, int> dis = std::make_pair(m_inds[a]->variable_distance(*m_inds[j]), j);
					auto it = m_dis[a].begin();
					while (it != m_dis[a].end() && it->first < dis.first) {
						++it;
					}
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
			}
		}
		else {
			for (size_t j = 0; j < size(); ++j) {
				if (a == j) continue;
				std::pair<Real, int> dis = std::make_pair(m_inds[a]->variable_distance(*m_inds[j]), j);
				auto it = m_dis[a].begin();
				while (it != m_dis[a].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[a].size() >= m_m) {
					if (it != m_dis[a].end()) {
						m_dis[a].insert(it, dis);
						m_dis[a].pop_back();
					}
				}
				else {
					m_dis[a].insert(it, dis);
				}
			}
		}

	}

	EvalTag NCDE_pop::evolve() {
		update_best();
		EvalTag tag = EvalTag::Normal;
		size_t nearest_index = 0;
		Real neardis;
		for (size_t i = 0; i < size(); ++i) {
			sort_distance(i);
			std::vector<size_t> candidate;
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				candidate.push_back(it->second);
			}
			std::vector<size_t> result(3);
			select_in_neighborhood(3, candidate, result);
			m_inds[i]->mutate(m_F, m_inds[result[0]].get(), m_inds[result[1]].get(), m_inds[result[2]].get());
			m_inds[i]->recombine(m_CR, m_recombine_strategy);
			tag = m_inds[i]->select();
			if (m_inds[i]->is_improved()) {
				neardis = std::numeric_limits<Real>::max();
				for (size_t j = 0; j < size(); ++j) {
					if (i == j) continue;
					Real dis = m_inds[i]->variable_distance(*m_inds[j]);
					if (neardis > dis) {
						neardis = dis;
						nearest_index = j;
					}
				}
			}
			else {
				nearest_index = m_dis[i].begin()->second;
			}
			if (m_inds[i]->dominate(*m_inds[nearest_index])) {
				*m_inds[nearest_index] = *m_inds[i];
				m_inds[nearest_index]->set_improved_flag(true);
			}
			if (tag != EvalTag::Normal) break;
		}
		if (tag == EvalTag::Normal) {
			++m_iter;
		}
		return tag;
	}

	NCDE::NCDE(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	void NCDE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
		m_pop.set_parameter(0.1, 0.9);
		m_initialized = true;
	}

	void NCDE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

#ifdef OFEC_DEMO
	void NCDE::updateBuffer() {
		if (!m_initialized) return;
		std::vector<std::vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_alg_cont*>(Demo::g_buffer_alg.get())->updateBuffer(&pops);
		Real num_improved = 0;
		for (size_t i = 0; i < m_pop.size(); ++i)
			if (m_pop[i].is_improved())
				num_improved++;
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			std::cout << evals << "\t" << err << "\tImprove Ratio:" << num_improved / m_pop.size() * 100 << "\%" << std::endl;
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			std::cout << evals << "\t" << num_opt_found << "/" << num_opt_known << "\tImprove Ratio:" << num_improved / m_pop.size() * 100 << "\%" << std::endl;
		}
	}
#endif

	void NCDE::record() {
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
}




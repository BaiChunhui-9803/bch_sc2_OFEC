#include "NSDE.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	NSDE_pop::NSDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop), m_m(9), m_dis((size_pop / (m_m + 1)) + 1), m_seed((size_pop / (m_m + 1)) + 1) {}

	void NSDE_pop::select_subpopulation() {
		sort();
		for (auto& i : m_order)
			m_order_list.emplace_back(i.second);
		int idx_species = 0;
		while (m_order_list.size() > 0) {
			m_seed[idx_species] = m_order_list[0];
			m_dis[idx_species].emplace_back(std::make_pair(0, m_order_list[0]));
			for (size_t j = 1; j < m_order_list.size(); ++j) {
				std::pair<Real, int> dis = std::make_pair(m_inds[m_seed[idx_species]]->variable_distance(*m_inds[m_order_list[j]]), m_order_list[j]);
				auto it = m_dis[idx_species].begin();
				while (it != m_dis[idx_species].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[idx_species].size() >= m_m + 1) {
					if (it != m_dis[idx_species].end()) {
						m_dis[idx_species].insert(it, dis);
						m_dis[idx_species].pop_back();
					}
				}
				else {
					m_dis[idx_species].insert(it, dis);
				}
			}
			for (auto it = m_dis[idx_species].begin(); it != m_dis[idx_species].end(); it++) {
				auto it_ = std::find(m_order_list.begin(), m_order_list.end(), it->second);
				m_order_list.erase(it_);
			}
			++idx_species;
		}
	}

	EvalTag NSDE_pop::evolve() {
		EvalTag tag = EvalTag::Normal;
		Real u, l;
		for (size_t i = 0; i < m_dis.size(); ++i) {
			for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
				mutate(it->second);
				m_inds[it->second]->recombine(m_CR, m_recombine_strategy);
				tag = m_inds[it->second]->select();
				if (m_seed[i] != it->second) {
					if (m_inds[m_seed[i]]->equal(*m_inds[it->second])) {
						for (size_t k = 0; k < m_inds[it->second]->variable().size(); ++k) {
							l = CONTINUOUS_CAST->range(k).first;
							u = CONTINUOUS_CAST->range(k).second;

							m_inds[it->second]->variable()[k] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(l, u);
						}
						m_inds[it->second]->evaluate();
					}
					else if (m_inds[it->second]->dominate(*m_inds[m_seed[i]])) {
						*m_inds[m_seed[i]] = *m_inds[it->second];
					}
				}
				if (tag != EvalTag::Normal) break;
			}
			if (tag != EvalTag::Normal) break;
		}
		++m_iter;
		return tag;
	}

	NSDE::NSDE(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void NSDE::initialize() {
		m_pop.set_mutation_strategy(DE::mutation_strategy::rand_1);
		m_pop.set_parameter(0.7, 0.5);  // set CR=0.7, F=0.5
		m_pop.initialize();
		m_pop.evaluate();
		m_pop.select_subpopulation();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void NSDE::run_() {
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

	void NSDE::record() {
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



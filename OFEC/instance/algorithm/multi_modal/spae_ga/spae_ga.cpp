#include "SPAE_GA.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include <buffer/buffer_alg/buffer_alg_cont.h>
extern std::unique_ptr<Demo::scene> Demo::g_buffer_alg;
#endif

namespace OFEC {
	SPAE_GA::SPAE_GA(param_map& v) : SPAE_GA(v.at("algorithm name"), v.at("subpopulation size"), v.at("numPartition")) {}

	SPAE_GA::SPAE_GA(const std::string& algName, size_t subPopSize, size_t numPartition)
		: algorithm(algName),
		m_subpop_size(subPopSize),
		m_num_segs(numPartition),
		m_hlc(CONTINUOUS_CAST->variable_size()) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	void SPAE_GA::initialize() {
		/* Initialize HLC */
		Solution<> temp(CONTINUOUS_CAST->objective_size(), CONTINUOUS_CAST->num_constraints(), CONTINUOUS_CAST->variable_size());
		std::vector<std::pair<Real, Real>> boundary(CONTINUOUS_CAST->variable_size());
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			boundary[j] = CONTINUOUS_CAST->range(j);
		m_hlc.initialize(boundary, m_num_segs);
		for (size_t i = 0; i < m_hlc.get_num_regions(); ++i) {
			const auto& box = m_hlc.get_box(i);
			for (size_t k = 0; k < CONTINUOUS_CAST->variable_size(); ++k) {
				for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
					temp.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(box[j].first, box[j].second);
				temp.evaluate();
				m_hlc.input_sample(temp);
			}
		}
		m_hlc.clustering();

		/* Initialize population */
		for (const auto& cluster : m_hlc.get_clusters()) {
			auto pop = new pop_type(m_subpop_size);
			for (size_t i = 0; i < pop->size(); ++i)
				init_in_cluster(cluster, (*pop)[i]);
			pop->set_cr(0.9);
			pop->set_mr(1.0 / CONTINUOUS_CAST->variable_size());
			pop->set_eta(20, 20);
			m_subpops += pop;
		}

		m_initialized = true;
	}

	void SPAE_GA::record() {
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
	}

	void SPAE_GA::run_() {
		size_t iterations = 1;
		while (!terminating()) {
			if (!m_inds_explore.empty()) m_inds_explore.clear();
			for (auto iter = m_subpops.begin(); iter != m_subpops.end();) {
				if (is_converged(**iter)) {
					explore(**iter);
					iter = m_subpops.remove(iter);
				}
				else {
					exploit(**iter);
					iter++;
				}
			}
			if (iterations % 10 == 0) {
				m_hlc.clustering();
			}
			update_accu_pro_clu();
			remove_outside_inds();
			merge_overlapped_pops();
			reallocate_explore_inds();
			iterations++;
		}
	}

	void SPAE_GA::update_accu_pro_clu() {
		m_hlc.update_potential();
		m_accu_pro_clu.resize(m_hlc.get_clusters().size());
		m_accu_pro_clu[0] = 1. / m_hlc.get_potential(0);
		for (size_t i = 1; i < m_accu_pro_clu.size(); ++i)
			m_accu_pro_clu[i] = m_accu_pro_clu[i - 1] + 1. / m_hlc.get_potential(i);
	}

	bool SPAE_GA::is_converged(pop_type& subpop) {
		subpop.update_best();
		subpop.update_worst();
		if (subpop.best()[0]->objective_distance(*subpop.worst()[0]) > 1e-6)
			return false;
		m_hlc.converged(subpop.best()[0]->solut());
		return true;
	}

	void SPAE_GA::exploit(pop_type& subpop) {
		if (subpop.size() < 4) return;
		size_t pop_size = subpop.size();
		size_t offsprings_size = subpop.size() / 2 * 2;
		pop_type offspring(offsprings_size + subpop.size());
		auto tag = EvalTag::Normal;
		for (size_t i = 0; i < offsprings_size; i += 2) {
			std::vector<size_t> p(2);
			p[0] = subpop.tournament_selection();
			do { p[1] = subpop.tournament_selection(); } while (p[1] == p[0]);
			subpop.crossover(p[0], p[1], offspring[i], offspring[i + 1]);
			subpop.mutate(offspring[i]);
			subpop.mutate(offspring[i + 1]);
		}
		for (size_t i = 0; i < subpop.size(); ++i)
			offspring[i + offsprings_size] = subpop[i];
		for (size_t i = 0; i < offsprings_size; ++i) {
			tag = offspring[i].evaluate();
			m_hlc.input_sample(offspring[i].solut());
			if (tag != EvalTag::Normal) break;
		}
		offspring.sort();
		for (size_t i = 0; i < offspring.size(); i++) {
			if (offspring[i].rank() < subpop.size())
				subpop[offspring[i].rank()] = offspring[i];
		}
		subpop.update_flag_best() = false;
		subpop.update_flag_worst() = false;
	}

	void SPAE_GA::explore(pop_type& subpop) {
		Real p;
		int idx_clu;
		for (size_t i = 0; i < subpop.size(); ++i) {
			p = global::ms_global->m_uniform[caller::Algorithm]->next() * m_accu_pro_clu.back();
			idx_clu = lower_bound(m_accu_pro_clu.begin(), m_accu_pro_clu.end(), p) - m_accu_pro_clu.begin();
			init_in_cluster(m_hlc.get_clusters()[idx_clu], subpop[i]);
			m_inds_explore.emplace_back(subpop[i]);
		}
	}

	void SPAE_GA::init_in_cluster(const std::vector<size_t>& cluster, ind_type& ind) {
		size_t ridx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, cluster.size());
		size_t immigrant_region = cluster[ridx];
		auto& box = m_hlc.get_box(immigrant_region);
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			ind.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(box[j].first, box[j].second);
		ind.evaluate();
		m_hlc.input_sample(ind.solut());
	}

	void SPAE_GA::remove_outside_inds() {
		std::vector<int> subpop_in_cluster(m_hlc.get_clusters().size(), -1);
		int idx_clu;
		for (size_t k = 0; k < m_subpops.size(); k++) {
			idx_clu = m_hlc.get_idx_cluster(m_hlc.find_idx(m_subpops[k].best()[0]->variable().vect()));
			if (idx_clu == -1)
				idx_clu = m_hlc.get_idx_nearest_cluster(m_hlc.find_idx(m_subpops[k].best()[0]->variable().vect()));
			subpop_in_cluster[idx_clu] = k;
		}
		for (size_t k = 0; k < m_subpops.size(); k++) {
			for (size_t i = 0; i < m_subpops[k].size();) {
				idx_clu = m_hlc.get_idx_cluster(m_hlc.find_idx(m_subpops[k][i].variable().vect()));
				if (idx_clu == -1)
					idx_clu = m_hlc.get_idx_nearest_cluster(m_hlc.find_idx(m_subpops[k][i].variable().vect()));
				if (subpop_in_cluster[idx_clu] != k) {
					m_inds_explore.emplace_back(m_subpops[k][i]);
					m_subpops[k].remove(i);
				}
				else
					++i;
			}
		}
		for (auto iter = m_subpops.begin(); iter != m_subpops.end(); ) {
			if ((*iter)->size() == 0)
				iter = m_subpops.remove(iter);
			else
				iter++;
		}
	}

	void SPAE_GA::reallocate_explore_inds() {
		if (m_inds_explore.empty()) return;
		std::vector<int> subpop_in_cluster(m_hlc.get_clusters().size(), -1);
		int idx_clu;
		for (size_t k = 0; k < m_subpops.size(); k++) {
			idx_clu = m_hlc.get_idx_cluster(m_hlc.find_idx(m_subpops[k].best()[0]->variable().vect()));
			if (idx_clu == -1)
				idx_clu = m_hlc.get_idx_nearest_cluster(m_hlc.find_idx(m_subpops[k].best()[0]->variable().vect()));
			subpop_in_cluster[idx_clu] = k;
		}
		for (auto& ind : m_inds_explore) {
			idx_clu = m_hlc.get_idx_cluster(m_hlc.find_idx(ind.variable().vect()));
			if (idx_clu == -1)
				idx_clu = m_hlc.get_idx_nearest_cluster(m_hlc.find_idx(ind.variable().vect()));
			if (subpop_in_cluster[idx_clu] == -1) {
				auto pop = new pop_type(1);
				(*pop)[0] = ind;
				subpop_in_cluster[idx_clu] = m_subpops.size();
				pop->set_cr(0.9);
				pop->set_mr(1.0 / CONTINUOUS_CAST->variable_size());
				pop->set_eta(20, 20);
				m_subpops += pop;
			}
			else {
				m_subpops[subpop_in_cluster[idx_clu]].add(ind);
			}
		}
	}

	void SPAE_GA::merge_overlapped_pops() {
		std::map<int, iter_subpop> subpop_in_cluster;
		int idx_clu;
		for (auto iter = m_subpops.begin(); iter != m_subpops.end();) {
			idx_clu = m_hlc.get_idx_cluster(m_hlc.find_idx((*iter)->best()[0]->variable().vect()));
			if (idx_clu == -1)
				idx_clu = m_hlc.get_idx_nearest_cluster(m_hlc.find_idx((*iter)->best()[0]->variable().vect()));
			if (subpop_in_cluster.find(idx_clu) == subpop_in_cluster.end()) {
				subpop_in_cluster[idx_clu] = iter;
				iter++;
			}
			else {
				iter = m_subpops.merge(subpop_in_cluster[idx_clu], iter);
			}
		}
	}

#ifdef OFEC_DEMO
	void SPAE_GA::updateBuffer() {
		if (!m_initialized) return;
		std::vector<std::vector<Solution<>*>> pops;
		pops.resize(m_subpops.size());
		for (size_t k = 0; k < m_subpops.size(); k++) {
			for (size_t i = 0; i < m_subpops[k].size(); ++i)
				pops[k].emplace_back(&m_subpops[k][i].solut());
		}
		m_hlc.update_potential();
		dynamic_cast<Demo::buffer_alg_cont*>(Demo::g_buffer_alg.get())->updateBuffer(&pops);
		//dynamic_cast<Demo::buffer_HLC*>(Demo::msp_buffer.get())->updateBuffer_(&pops, m_hlc);
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
	}
#endif
}


#include "EA_KDTree.h"
#include <utility>

#ifdef OFEC_DEMO
#include <buffer/buffer_alg/buffer_alg_cont.h>
extern std::unique_ptr<Demo::scene> Demo::g_buffer_alg;
#endif

namespace OFEC {
	EA_KDTree::EA_KDTree(param_map& v) : EA_KDTree(v.at("algorithm name"), v.at("population size")) {}

	EA_KDTree::EA_KDTree(const std::string& alg_name, size_t size_pop) : 
		algorithm(alg_name), 
		m_pop(size_pop),
		m_num_segs(1),
		m_ratio_explored(0.9), 
		m_kdtree(CONTINUOUS_CAST->variable_size(), 2) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void EA_KDTree::initialize() {
		auto& range = CONTINUOUS_CAST->range();
		std::vector<std::pair<Real, Real>> boundary(CONTINUOUS_CAST->variable_size());
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			boundary[j] = CONTINUOUS_CAST->range(j);
		m_kdtree.setInitBox(boundary);
		m_kdtree.inputData(std::vector<Real>(m_num_segs, 1. / m_num_segs));
		m_kdtree.buildIndex();
		m_unexplored.resize(m_num_segs);
		std::iota(m_unexplored.begin(), m_unexplored.end(), 0);
		m_pop.set_cr(0.9);
		m_pop.set_mr(1.0 / CONTINUOUS_CAST->variable_size());
		m_pop.set_eta(20, 20);
		m_pop.initialize();
		m_pop.evaluate();
		m_initialized = true;
	}

	void EA_KDTree::record() {

	}

#ifdef OFEC_DEMO
	void EA_KDTree::updateBuffer() {
		if (!m_update_buffer) return;
		std::vector<std::vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_alg_cont*>(Demo::g_buffer_alg.get())->updateBuffer(&pops);
		//vector<vector<Solution<>*>> pops(1);
		//for (size_t i = 0; i < m_pop.size(); ++i)
		//	pops[0].emplace_back(&m_pop[i].solut());
		//dynamic_cast<Demo::buffer_EA_KDTree*>(Demo::msp_buffer.get())->updateBuffer_(&pops, m_kdtree, m_explored, m_attractors);
		m_update_buffer = false;
	}
#endif

	void EA_KDTree::run_() {
		while (!terminating()) {
			while (!is_converged()) {
				evolve_pop();
				if (terminating())
					break;
#ifdef OFEC_DEMO
				m_update_buffer = true;
#endif
			}
			std::vector<std::pair<Real, Real>> basin(CONTINUOUS_CAST->variable_size());
			basin_estimation(*m_pop.best()[0], basin);
			attractor atr(*m_pop.best()[0], basin);
			tree_construction(atr);
			if (m_unexplored.empty()) {
#ifdef OFEC_DEMO
				m_update_buffer = true;
				updateBuffer();
#endif
				std::cout << "all searched out" << std::endl;
				break;
			}
			for (size_t i = 0; i < m_pop.size(); ++i) {
				while (is_revisiting(m_pop[i].variable().vect())) {
					init_exploring_ind(m_pop[i]);
					if (!is_revisiting(m_pop[i].variable().vect())) {
						m_pop[i].evaluate();
						break;
					}
				}
			}
		}
	}

	EvalTag EA_KDTree::evolve_pop() {
		auto tag = EvalTag::Normal;
		pop_type offspring(2 * m_pop.size());
		for (size_t i = 0; i < m_pop.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = m_pop.tournament_selection();
			do { p[1] = m_pop.tournament_selection(); } while (p[1] == p[0]);
			m_pop.crossover(p[0], p[1], offspring[i], offspring[i + 1]);
			m_pop.mutate(offspring[i]);
			m_pop.mutate(offspring[i + 1]);
			while (is_revisiting(offspring[i].variable().vect())) {
				init_exploring_ind(offspring[i]);
				if (!is_revisiting(offspring[i].variable().vect())) {
					offspring[i].evaluate();
					break;
				}
			}
			while (is_revisiting(offspring[i + 1].variable().vect())) {
				init_exploring_ind(offspring[i + 1]);
				if (!is_revisiting(offspring[i + 1].variable().vect())) {
					offspring[i + 1].evaluate();
					break;
				}
			}
		}
		for (size_t i = 0; i < m_pop.size(); ++i)
			offspring[i + m_pop.size()] = m_pop[i];
		for (size_t i = 0; i < m_pop.size(); ++i) {
			tag = offspring[i].evaluate();
			if (tag != EvalTag::Normal) return tag;
		}
		offspring.sort();
		for (size_t i = 0; i < offspring.size(); i++) {
			if (offspring[i].rank() < m_pop.size())
				m_pop[offspring[i].rank()] = offspring[i];
		}
		return tag;
	}

	void EA_KDTree::basin_estimation(const Solution<>& opt, std::vector<std::pair<Real, Real>>& basin) {
		size_t idx_reg = m_kdtree.get_regionIdx(opt.variable().vect());
		if (idx_reg == -1) return;
		//auto& box = m_kdtree.get_box(idx_reg);
		Real sigma;
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			auto& range = CONTINUOUS_CAST->range(j);
			sigma = (range.second - range.first) / 50;
			basin[j].first = basin[j].second = opt.variable()[j];
			Solution<> tmp_sol_1(opt), tmp_sol_2(opt);
			// Identifying the upper range u of the basin
			do {
				if (basin[j].second + sigma > range.second) {
					basin[j].second = range.second;
					//if (basin[j].second + sigma > box[j].second) {
						//basin[j].second = box[j].second;
					break;
				}
				basin[j].second += sigma;
				tmp_sol_1 = tmp_sol_2;
				tmp_sol_2.variable()[j] = basin[j].second;
				tmp_sol_2.evaluate();
			} while (tmp_sol_1.dominate(tmp_sol_2));
			// Identifying the lower range l of the basin
			tmp_sol_2 = opt;
			do {
				//if (basin[j].first - sigma < box[j].first) {
				//	basin[j].first = box[j].first;
				if (basin[j].first - sigma < range.first) {
					basin[j].first = range.first;
					break;
				}
				basin[j].first -= sigma;
				tmp_sol_1 = tmp_sol_2;
				tmp_sol_2.variable()[j] = basin[j].first;
				tmp_sol_2.evaluate();
			} while (tmp_sol_1.dominate(tmp_sol_2));
		}
	}

	void EA_KDTree::find_min_ratio_division(const attractor& atr, int dim_split, size_t idx_reg, Real& split_pivot, Real& ratio) {
		auto& box = m_kdtree.get_box(idx_reg);
		if ((atr.range[dim_split].second - box[dim_split].first) < (box[dim_split].second - atr.range[dim_split].first)) {
			ratio = (atr.range[dim_split].second - box[dim_split].first) / (box[dim_split].second - box[dim_split].first);
			split_pivot = atr.range[dim_split].second;
		}
		else {
			ratio = (box[dim_split].second - atr.range[dim_split].first) / (box[dim_split].second - box[dim_split].first);
			split_pivot = atr.range[dim_split].first;
		}
	}

	void EA_KDTree::tree_construction(const attractor& atr) {
		size_t idx_reg = m_kdtree.get_regionIdx(atr.optimum.variable().vect());
		int dim_split = 0;
		Real split_pivot;
		if (m_attractors[idx_reg]) {        // there exsits another optimum
			Real dis, max_dis = dis_range(atr.range[0], m_attractors[idx_reg]->range[0]);
			for (int j = 1; j < CONTINUOUS_CAST->variable_size(); ++j) {
				dis = dis_range(atr.range[j], m_attractors[idx_reg]->range[j]);
				if (max_dis < dis) {
					max_dis = dis;
					dim_split = j;
				}
			}
			split_pivot = split_pivot_of_range(atr.range[dim_split], m_attractors[idx_reg]->range[dim_split]);
			size_t new_reg = m_kdtree.split_region(idx_reg, &dim_split, &split_pivot);
			m_attractors[new_reg].reset(new attractor(atr));
			if (m_kdtree.get_regionIdx(atr.optimum.variable().vect()) != new_reg)
				std::swap(m_attractors[idx_reg], m_attractors[new_reg]);
		}
		else {                              // there is no other optimum
			Real pivot;
			Real min_ratio, ratio;
			find_min_ratio_division(atr, 0, idx_reg, split_pivot, min_ratio);
			for (size_t j = 1; j < CONTINUOUS_CAST->variable_size(); ++j) {
				find_min_ratio_division(atr, j, idx_reg, pivot, ratio);
				if (ratio < min_ratio) {
					min_ratio = ratio;
					dim_split = j;
					split_pivot = pivot;
				}
			}
			size_t new_reg = m_kdtree.split_region(idx_reg, &dim_split, &split_pivot);
			m_attractors[new_reg].reset(new attractor(atr));
			if (m_kdtree.get_regionIdx(atr.optimum.variable().vect()) != new_reg) {
				std::swap(m_attractors[idx_reg], m_attractors[new_reg]);
			}
		}
		m_unexplored.clear();
		m_explored.clear();
		for (size_t i = 0; i < m_kdtree.size(); i++) {
			if (filled_ratio(i) > m_ratio_explored)
				m_explored.insert(i);
			else
				m_unexplored.push_back(i);
		}
	}

	bool EA_KDTree::is_converged() {
		m_pop.update_best();
		m_pop.update_worst();
		if (m_pop.best()[0]->objective_distance(*m_pop.worst()[0]) > 1e-6)
			return false;
		else
			return true;
	}

	bool EA_KDTree::is_revisiting(const std::vector<Real>& x) {
		for (auto& pair : m_attractors) {
			if (!pair.second) continue;
			bool in_range = true;
			for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); j++) {
				if (x[j] > pair.second->range[j].second || x[j] < pair.second->range[j].first) {
					in_range = false;
					break;
				}
			}
			if (in_range)
				return true;
		}
		return false;
	}

	void EA_KDTree::init_exploring_ind(Solution<>& ind) {
		size_t trg_reg = m_unexplored[global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, m_unexplored.size())];
		auto& box = m_kdtree.get_box(trg_reg);
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); j++) {
			ind.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(box[j].first, box[j].second);
		}
	}

	Real EA_KDTree::dis_range(const std::pair<Real, Real>& range1, const std::pair<Real, Real>& range2) {
		if (range1.first < range2.first)
			return range2.first - range1.second;
		else
			return range1.first - range2.second;
	}

	Real EA_KDTree::split_pivot_of_range(const std::pair<Real, Real>& range1, const std::pair<Real, Real>& range2) {
		Real dis = dis_range(range1, range2);
		if (dis < 0) {
			if (range1.first < range2.first)
				return (range2.first + range1.second) / 2;
			else
				return (range1.first + range2.second) / 2;
		}
		else {
			if (range1.first < range2.first)
				return range1.second;
			else
				return range2.second;
		}
	}

	Real EA_KDTree::filled_ratio(size_t idx_reg) {
		auto& box = m_kdtree.get_box(idx_reg);
		size_t num_sample = 1000, num_in_box = 0;
		std::vector<Real> tmp_x(CONTINUOUS_CAST->variable_size());
		for (size_t i = 0; i < num_sample; ++i) {
			for (size_t j = 0; j < tmp_x.size(); ++j) {
				tmp_x[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(box[j].first, box[j].second);
			}
			if (is_revisiting(tmp_x))
				num_in_box++;
		}
		return Real(num_in_box) / Real(num_sample);
	}

}

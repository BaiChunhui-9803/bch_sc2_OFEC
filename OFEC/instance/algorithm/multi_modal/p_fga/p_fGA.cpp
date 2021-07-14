#include "./p_fGA.h"

#ifdef OFEC_DEMO
#include <buffer/buffer_alg/buffer_alg_cont.h>
extern std::unique_ptr<Demo::scene> Demo::g_buffer_alg;
#endif

namespace OFEC {
	p_fGA::p_fGA(param_map& v) : p_fGA(v.at("algorithm name"), v.at("population size")) {}

	p_fGA::p_fGA(const std::string& algName, size_t popSize) :
		algorithm(algName),
		m_pop(popSize),
		m_S(CONTINUOUS_CAST->variable_size()),
		m_K_H(60),
		m_K_R(0.7),
		m_num_stagnate(0) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void p_fGA::initialize() {
		m_pop.set_cr(0.9);
		m_pop.set_mr(1.0 / CONTINUOUS_CAST->variable_size());
		m_pop.set_eta(20, 20);
		//m_pop.set_mutation_strategy(DE::mutation_strategy::rand_1);
		//m_pop.set_parameter(0.5, 0.6);
		m_pop.initialize();
		m_pop.evaluate();
		update_S();
		m_pop.update_best();
		m_previous_best = m_pop.best()[0]->objective(0);
#ifdef OFEC_DEMO
		m_update_buffer = true;
#endif
	}

	void p_fGA::record() {

	}

#ifdef OFEC_DEMO
	void p_fGA::updateBuffer() {
		if (!m_update_buffer) return;
		std::vector<std::vector<Solution<>*>> pops(1 + m_child_pops.size());
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		for (size_t k = 0; k < m_child_pops.size(); k++) {
			for (size_t i = 0; i < m_child_pops[k].size(); i++) {
				pops[k + 1].emplace_back(&m_child_pops[k][i].solut());
			}
		}
		dynamic_cast<Demo::buffer_alg_cont*>(Demo::g_buffer_alg.get())->updateBuffer(&pops);
		//dynamic_cast<Demo::buffer_p_fGA*>(Demo::msp_buffer.get())->updateBuffer_(&pops, m_attractors, m_S);
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
		m_update_buffer = false;
	}
#endif

	void p_fGA::run_() {
		auto tag = EvalTag::Normal;
		std::vector<size_t> inds_to_fork;
		while (!terminating()) {
			tag = evolve_pop(m_pop, true);
			if (forking_condition(inds_to_fork)) {
				m_attractors.push_back(*m_pop.best()[0]);
				fork_pop(inds_to_fork);
			}
			for (size_t k = 0; k < m_child_pops.size(); k) {
				tag = evolve_pop(m_child_pops[k], false);
				if (overlapped(m_child_pops[k]))
					m_child_pops.remove(k);
				else
					k++;
			}
#ifdef OFEC_DEMO
			m_update_buffer = true;
#endif
		}
	}

	EvalTag p_fGA::evolve_pop(type_pop& pop, bool is_main_pop) {
		if (!is_main_pop && !pop.is_active())
			return EvalTag::Normal;
		type_pop offspring(2 * pop.size());
		auto tag = EvalTag::Normal;
		for (size_t i = 0; i < pop.size(); i += 2) {
			//for (size_t i = 0; i < pop.size(); ++i) {
				//pop.mutate(i);
				//pop.recombine(i);
			std::vector<size_t> p(2);
			p[0] = pop.tournament_selection();
			do { p[1] = pop.tournament_selection(); } while (p[1] == p[0]);
			pop.crossover(p[0], p[1], offspring[i], offspring[i + 1]);
			pop.mutate(offspring[i]);
			pop.mutate(offspring[i + 1]);
			if (is_main_pop) {
				//if (in_blocks(pop[i].trial()))
					//block_ind(pop[i].trial());
				if (in_blocks(offspring[i]))
					block_ind(offspring[i]);
				if (in_blocks(offspring[i + 1]))
					block_ind(offspring[i + 1]);
			}
			else {
				if (!in_neighbor(m_attractors[pop.id()], offspring[i]))
					init_in_neighbor(m_attractors[pop.id()], offspring[i]);
				if (!in_neighbor(m_attractors[pop.id()], offspring[i + 1]))
					init_in_neighbor(m_attractors[pop.id()], offspring[i + 1]);
			}
			//pop[i].select();
		}
		for (size_t i = 0; i < pop.size(); ++i)
			offspring[i + pop.size()] = pop[i];
		for (size_t i = 0; i < pop.size(); ++i) {
			tag = offspring[i].evaluate();
			if (tag != EvalTag::Normal) break;
		}
		offspring.sort();
		for (size_t i = 0; i < offspring.size(); i++) {
			if (offspring[i].rank() < pop.size())
				pop[offspring[i].rank()] = offspring[i];
		}
		pop.update_best();
		pop.update_worst();
		if (pop.best()[0]->objective_distance(*pop.worst()[0]) < 1e-6)
			pop.set_active_flag(false);
		return tag;
	}

	bool p_fGA::forking_condition(std::vector<size_t>& inds_to_fork) {
		inds_to_fork.clear();
		m_pop.update_best();
		if (m_pop.best()[0]->objective(0) == m_previous_best)
			m_num_stagnate++;
		else
			m_previous_best = m_pop.best()[0]->objective(0);
		if (m_num_stagnate > m_K_H) {
			size_t num_in_neighborhood = 0;
			for (size_t i = 0; i < m_pop.size(); ++i) {
				if (in_neighbor(*m_pop.best()[0], m_pop[i].solut())) {
					num_in_neighborhood++;
					inds_to_fork.push_back(i);
				}
			}
			if (num_in_neighborhood > m_K_R * m_pop.size()) {
				m_num_stagnate = 0;
				return true;
			}
		}
		return false;
	}

	void p_fGA::fork_pop(const std::vector<size_t>& inds_to_fork) {
		size_t size_new_pop = inds_to_fork.size();
		if (size_new_pop % 2 == 1)
			size_new_pop++;
		auto new_child_pop = new type_pop(size_new_pop);
		size_t i = 0;
		for (size_t ind : inds_to_fork) {
			(*new_child_pop)[i++] = m_pop[ind];
			block_ind(m_pop[ind]);
		}
		if (inds_to_fork.size() % 2 == 1) {
			init_in_neighbor(m_attractors.back(), (*new_child_pop)[i]);
		}
		//new_child_pop->set_mutation_strategy(DE::mutation_strategy::rand_1);
		//new_child_pop->set_parameter(0.5, 0.6);
		new_child_pop->set_cr(0.9);
		new_child_pop->set_mr(1.0 / CONTINUOUS_CAST->variable_size());
		new_child_pop->set_eta(20, 20);
		new_child_pop->set_id(m_attractors.size() - 1);
		m_child_pops += new_child_pop;
	}

	bool p_fGA::in_neighbor(const Solution<>& center, const Solution<>& ind) {
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			if (ind.variable()[j] < center.variable()[j] - m_S[j] / 2
				|| ind.variable()[j] > center.variable()[j] + m_S[j] / 2)
				return false;
		}
		return true;
	}

	void p_fGA::init_in_neighbor(const Solution<>& center, Solution<>& ind) {
		Real l, u;
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			l = center.variable()[j] - m_S[j] / 2;
			u = center.variable()[j] + m_S[j] / 2;
			ind.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(l, u);
		}
	}


	void p_fGA::block_ind(Solution<>& ind) {
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			auto& range = CONTINUOUS_CAST->range(j);
			ind.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(range.first, range.second);
		}
		if (in_blocks(ind))
			block_ind(ind);
		else
			ind.evaluate();
	}

	bool p_fGA::in_blocks(const Solution<>& ind) {
		for (auto& sol : m_attractors) {
			if (in_neighbor(sol, ind)) {
				return true;
			}
		}
		return false;
	}

	void p_fGA::update_S() {
		Real delta;
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			delta = (CONTINUOUS_CAST->range(j).second - CONTINUOUS_CAST->range(j).first) / Real(pow(2, 8) - 1);
			m_S[j] = delta * (pow(2, 5) - 1);
		}
	}

	bool p_fGA::overlapped(const type_pop& pop) {
		for (size_t k = 0; k < pop.id(); ++k) {
			bool all_in = true;
			for (size_t i = 0; i < pop.size(); ++i) {
				if (!in_neighbor(m_attractors[k], pop[i])) {
					all_in = false;
					break;
				}
			}
			if (all_in)
				return true;
		}
		return false;
	}
}
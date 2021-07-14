#include "LIPS.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	LI_particle::LI_particle(size_t num_obj, size_t num_con, size_t size_var) : particle(num_obj, num_con, size_var), m_pos(size_var), m_rdm(size_var) {
		set_improved_flag(true);
	}
	
	LI_particle::LI_particle(const Solution<> & rhs) : particle(rhs), m_pos(rhs.variable().size()), m_rdm(rhs.variable().size()) {
		set_improved_flag(true);
	}

	void LI_particle::next_velocity(const Solution<>* lbest, Real w, Real c1, Real c2) {
		for (size_t j = 0; j < m_var.size(); j++) {
			m_vel[j] = w * (m_vel[j] + m_rdm[j] * (m_pos[j] - m_var[j]));
		}
	}

	LI_swarm::LI_swarm(size_t size_pop) : swarm(size_pop), m_M(2), m_dis(size_pop) {}

	void LI_swarm::set_max_evals(int max_evals) { m_max_evals = max_evals; }

	EvalTag LI_swarm::evolve() {
		EvalTag rf = EvalTag::Normal;
		// Update the value of M
		if (m_iter * m_inds.size() > m_max_evals) 
			m_M = 5;
		else
			m_M = 2 + 3. * static_cast<Real>(m_iter * m_inds.size()) / static_cast<Real>(m_max_evals);
		// Breeding process
		for (int i = 0; i < m_inds.size(); i++) {
			sort_distance(i);
			set_best_pos(i);
			m_inds[i]->next_velocity(nullptr, m_weight);   // lbest and c1, c2 are actually not used in LIPS
			m_inds[i]->move();
			m_inds[i]->clamp_velocity();
			rf = m_inds[i]->evaluate();
			if (m_inds[i]->dominate(m_inds[i]->pbest())) {
				m_inds[i]->pbest() = m_inds[i]->solut();
				if (update_best(this->m_inds[i]->solut()))
					m_flag_best_impr = true;
				this->m_best_updated = true;
				m_inds[i]->set_improved_flag(true);
			}
			else
				m_inds[i]->set_improved_flag(false);
			this->handle_evaluation_tag(rf);
			if (rf != EvalTag::Normal) break;
		}
		this->m_iter++;
		return rf;
	}

	void LI_swarm::set_best_pos(int idx_ind) {
		for (auto& j : m_inds[idx_ind]->m_pos) j = 0;
		for (auto& j : m_inds[idx_ind]->m_rdm) j = 0;
		for (auto& i : m_inds[idx_ind]->m_nbr) {
			for (auto j = 0; j < CONTINUOUS_CAST->variable_size(); j++) {
				double rdom = global::ms_global->m_uniform[caller::Algorithm]->next() * (4.1 / m_M);
				m_inds[idx_ind]->m_pos[j] += m_inds[i]->pbest().variable()[j] * rdom;
				m_inds[idx_ind]->m_rdm[j] += rdom;
			}
		}
		for (auto j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			m_inds[idx_ind]->m_pos[j] = m_inds[idx_ind]->m_pos[j] / m_inds[idx_ind]->m_rdm[j];
		}
	}

	void LI_swarm::sort_distance(int idx_ind) {
		for (size_t j = 0; j < m_inds.size(); j++) {
			if (idx_ind == j) continue;
			if (m_inds[idx_ind]->is_improved() || m_inds[j]->is_improved()) {
				std::pair<Real, size_t> dis = std::make_pair(m_inds[idx_ind]->variable_distance(*m_inds[j]), j);
				auto it = m_dis[idx_ind].begin();
				while (it != m_dis[idx_ind].end() && it->first < dis.first) {
					it++;
				}
				if (m_dis[idx_ind].size() >= m_M - 1) {
					if (it != m_dis[idx_ind].end()) {
						m_dis[idx_ind].insert(it, dis);
						m_dis[idx_ind].pop_back();
					}
				}
				else {
					m_dis[idx_ind].insert(it, dis);
				}
			}
		}
		m_inds[idx_ind]->m_nbr.resize(0);
		for (auto& i : m_dis[idx_ind]) m_inds[idx_ind]->m_nbr.push_back(i.second);
		m_inds[idx_ind]->m_nbr.push_back(idx_ind);
	}

	LIPS::LIPS(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		int max_evals = (v.find("maximum evaluations") != v.end()) ? static_cast<int>(v.at("maximum evaluations")) : CONTINUOUS_CAST->variable_size() * 10000;
		m_pop.set_max_evals(max_evals);
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void LIPS::initialize()	{
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

	void LIPS::record()	{
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

	void LIPS::run_() {
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

}
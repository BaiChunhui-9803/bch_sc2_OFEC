#include "DynDE.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	DynDE::DynDE(const ParamMap &v) : algorithm(v.at("algorithm name")), m_r_excl(v.at("exlRadius")),\
		m_multi_pop(v.at("population size") / v.at("subpopulation size"), v.at("subpopulation size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void DynDE::initialize() {
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			m_multi_pop[i].initialize();
			m_multi_pop[i].evaluate();
		}
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(m_multi_pop.size());
		for (size_t i = 0; i < m_multi_pop.size(); i++) {
			for (size_t j = 0; j < m_multi_pop[i].size(); ++j)
				pops[i].emplace_back(&m_multi_pop[i][j].solut());
		}
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void DynDE::run_() {
		EvalTag tag = EvalTag::Normal;
		while (!terminating()) {
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (!m_multi_pop[i].get_flag())
					tag = m_multi_pop[i].evolve();
				handle_evaluation_tag(tag);
			}
			if (tag == EvalTag::Terminate)
				break;
			//exclusion
			exclusion_check();
			for (size_t i = 0; i < m_multi_pop.size(); ++i) {
				if (m_multi_pop[i].get_flag()) {
					m_multi_pop[i].initialize();
					m_multi_pop[i].set_flag(false);
				}
			}
#ifdef OFEC_DEMO
			vector<vector<Solution<>*>> pops(m_multi_pop.size());
			for (size_t i = 0; i < m_multi_pop.size(); i++) {
				for (size_t j = 0; j < m_multi_pop[i].size(); ++j)
					pops[i].emplace_back(&m_multi_pop[i][j].solut());
			}
			dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
		}
	}

	void DynDE::exclusion_check() {
		for (size_t i = 0; i < m_multi_pop.size(); ++i) {
			for (size_t j = i + 1; j < m_multi_pop.size(); ++j) {
				if (m_multi_pop[i].get_flag() == false && m_multi_pop[j].get_flag() == false && m_multi_pop[i].best()[0]->variable_distance(*m_multi_pop[j].best()[0]) < m_r_excl) {
					if (m_multi_pop[i].best()[0]->dominate(*m_multi_pop[j].best()[0])) {
						m_multi_pop[j].set_flag(true);
					}
					else {
						m_multi_pop[i].set_flag(true);
					}
				}
			}
		}
	}

	void DynDE::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::DOP)) {
			// ******* Dynamic Optimization ***********

		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
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

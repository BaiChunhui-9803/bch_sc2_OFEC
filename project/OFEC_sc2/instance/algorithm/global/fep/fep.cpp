#include "FEP.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {

	FEP_pop::FEP_pop(size_t size_pop) : EP::population<>(size_pop) {}

	void FEP_pop::mutate() {
		size_t size_parents = m_inds.size();
		if (m_offspring.size() != 2 * size_parents)
			throw myexcept("Error at EP::population::mutate(). The size of offspring must be 2 times of the size of parents.");
		for (size_t i = 0; i < size_parents; ++i)
			* m_offspring[i] = *m_inds[i];
		for (size_t i = size_parents; i < m_offspring.size(); i++) {
			Real N = global::ms_global->m_normal[caller::Algorithm]->next();
			for (size_t j = 0; j < m_offspring[i]->variable().size(); ++j) {
				Real delta_j = global::ms_global->m_cauchy[caller::Algorithm]->next();
				m_offspring[i]->variable()[j] = m_inds[i - size_parents]->variable()[j] + m_inds[i - size_parents]->eta()[j] * delta_j;
				Real N_j = global::ms_global->m_normal[caller::Algorithm]->next();
				m_offspring[i]->eta()[j] = m_inds[i - size_parents]->eta()[j] * exp(m_tau_prime * N + m_tau * N_j);
			}
		}
	}

	FEP::FEP(param_map & v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void FEP::initialize() {
        m_pop.tau_prime() = 1 / (sqrt(2 / sqrt(m_pop.size())));
        m_pop.tau() = 1 / (sqrt(2 * m_pop.size()));
        m_pop.q() = 10;
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void FEP::run_() {
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

	void FEP::record() {
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
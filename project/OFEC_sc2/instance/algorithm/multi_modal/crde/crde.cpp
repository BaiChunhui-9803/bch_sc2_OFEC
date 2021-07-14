#include "CRDE.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {

	CRDE_pop::CRDE_pop(size_t size_pop) : DE::population<DE::individual>(size_pop) {}

	EvalTag CRDE_pop::evolve() {
		if (m_inds.size() < 5) {
			throw myexcept("the population size cannot be smaller than 5@DE::CRDE::evolve()");
		}
		EvalTag tag = EvalTag::Normal;
		for (size_t i = 0; i < size(); ++i) {
			mutate(i);
			m_inds[i]->recombine(m_CR, m_recombine_strategy);
			tag = m_inds[i]->trial().evaluate();
			if (tag == EvalTag::Terminate) return tag;
			int idx = nearest_neighbour(i).begin()->second;
			if (m_inds[i]->trial().dominate(*m_inds[idx])) {
				m_inds[idx]->solut() = m_inds[i]->trial();
			}
		}
		m_iter++;
		return tag;
	}

	CRDE::CRDE(const ParamMap &v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void CRDE::initialize()	{
		m_pop.set_parameter(0.9, 0.5);
		m_pop.set_mutation_strategy(DE::mutation_strategy::rand_1);
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void CRDE::run_() {
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

	void CRDE::record() {
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


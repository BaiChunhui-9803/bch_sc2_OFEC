#include "./GL_cont.h"
#include "./adaptor_cont.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	GL_cont_pop::GL_cont_pop(size_t size_pop, size_t size_var) : GL<individual<>>(size_pop, size_var)	{}

	void GL_cont_pop::initialize() {
		m_adaptor.reset(new adaptor_cont(m_alpha, CONTINUOUS_CAST->variable_size(), m_inds.size()));
		population<individual<>>::initialize();
		initialize_memory();
		initialize_curpop();
	}

	void GL_cont_pop::initialize_curpop() {
		for (int i = 0; i < this->size(); i++) {
			m_offspring.push_back(*this->m_inds[i]);
		}
	}

	EvalTag GL_cont_pop::evolve() {
		dynamic_cast<adaptor_cont*>(m_adaptor.get())->update_step(m_inds);
		m_adaptor->create_solution(m_inds, m_offspring);
		EvalTag rf = update();
		for (auto& i : m_inds)
			if (i->is_improved())
				update_best(*i);
		update_memory();
		m_iter++;
		return rf;
	}

	GL_cont::GL_cont(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size"), CONTINUOUS_CAST->variable_size()) {
		if (v.find("updateSchemeProbabilityLearning") != v.end())
			m_pop.set_update_scheme(v.at("updateSchemeProbabilityLearning"));
		if (v.find("alpha") != v.end()) m_pop.set_alpha(v.at("alpha"));
		if (v.find("beta") != v.end()) m_pop.set_beta(v.at("beta"));
		if (v.find("gamma") != v.end()) m_pop.set_gamma(v.at("gamma"));
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void GL_cont::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void GL_cont::run_() {
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

	void GL_cont::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
			Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
			measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		}
		else {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			measure::get_measure()->record(global::ms_global.get(), evals, err);
		}
	}
}

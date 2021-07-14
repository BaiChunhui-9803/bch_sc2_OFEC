#include "simulated_annealing.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/measure/measure.h"

namespace OFEC {
	simulated_annealing::simulated_annealing(param_map& v) : simulated_annealing(v.at("maximum evaluations")){}

	simulated_annealing::simulated_annealing(size_t k_max) : algorithm("SA"), m_k_max(k_max) {
		size_t size_var = CONTINUOUS_CAST->variable_size();
		size_t size_obj = CONTINUOUS_CAST->objective_size();
		size_t num_con = CONTINUOUS_CAST->num_constraints();
		m_s.reset(new Solution<>(size_obj, num_con, size_var));
		m_s_new.reset(new Solution<>(size_obj, num_con, size_var));
	}

	void simulated_annealing::initialize() {
		m_s->initialize();
		m_s->evaluate();
	}

	void simulated_annealing::run_() {
		while (!terminating()) {
			size_t k = global::ms_global->m_problem->evaluations();
			Real T = temperature(static_cast<Real>(k) / m_k_max);
			neighbour(*m_s, *m_s_new);
			m_s_new->evaluate();
			if (P(*m_s, *m_s_new, T) >= global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(0., 1.))
				*m_s = *m_s_new;
		}
	}

	void simulated_annealing::record() {
		// ******* Global Optimization ************
		size_t evals = CONTINUOUS_CAST->evaluations();
		Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
		measure::get_measure()->record(global::ms_global.get(), evals, err);
	}

	Real simulated_annealing::temperature(Real val)	{
		return -log10(val);
	}

	void simulated_annealing::neighbour(const Solution<>& s, Solution<>& s_new) {
		for (size_t i = 0; i < s.variable().size(); i++)	{
			if (CONTINUOUS_CAST->range()[i].limited) {
				Real variance = (CONTINUOUS_CAST->range(i).second - CONTINUOUS_CAST->range(i).first) / 100;
				do {
					s_new.variable()[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(s.variable()[i], variance);
				} while (s_new.variable()[i] < CONTINUOUS_CAST->range(i).first || s.variable()[i] > CONTINUOUS_CAST->range(i).second);
			}
			else
				s_new.variable()[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(s.variable()[i], 100);
		}
	}

	Real simulated_annealing::P(const Solution<>& s, const Solution<>& s_new, Real T)	{
		if (s_new.dominate(s))
			return 1.;
		else
			return exp(-abs(s.objective()[0] - s_new.objective()[0]) / T);
	}
#ifdef OFEC_DEMO
	void simulated_annealing::updateBuffer() {
	
	}
#endif
}
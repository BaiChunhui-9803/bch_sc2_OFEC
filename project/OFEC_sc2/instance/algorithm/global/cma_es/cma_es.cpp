#include "CMA_ES.h"
#include "cmaes_interface.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	CMA_ES::CMA_ES(param_map & v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void CMA_ES::initialize() {
		m_initialized = true;
	}

	void CMA_ES::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}

	void CMA_ES::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
			Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
			std::vector<Real> vals = { Real(evals), peak_ratio, success_rate };
			measure::get_measure()->record(global::ms_global.get(), vals);
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			std::vector<Real> vals = { Real(evals), err };
			measure::get_measure()->record(global::ms_global.get(), vals);
		}
	}
	
	cmaes_pop::cmaes_pop(size_t size_pop) : population(size_pop, CONTINUOUS_CAST->variable_size()) {
		initialsFilePathName = (std::string)g_working_dir + "/instance/algorithm/ES/CMA_ES/initials.par";
		signalsFilePathName = (std::string)g_working_dir + "/instance/algorithm/ES/CMA_ES//signals.par";
		size_t size_var = CONTINUOUS_CAST->variable_size();
		arFunvals = cmaes_init(&evo, size_var, NULL, NULL, 0, size_pop, initialsFilePathName.c_str()); /* "cmaes_initials.par"*/
		/* printf("%s\n", cmaes_SayHello(&evo)); */
		//cmaes_ReadSignals(&evo, signalsFilePathName.c_str());  /* "cmaes_signals.par" write header and initial values */
	}

	cmaes_pop::cmaes_pop(size_t size_pop, const std::vector<Real>& start_ind, Real step_size) : population(size_pop, CONTINUOUS_CAST->variable_size()) {
		size_t size_var = CONTINUOUS_CAST->variable_size();
		std::vector<Real> stepsize(start_ind.size(), step_size);
		std::vector<double> xstart(start_ind.size()), stddev(start_ind.size());
		reverse_x(start_ind, xstart.data());
		reverse_size(stepsize, stddev.data());
		arFunvals = cmaes_init(&evo, size_var, xstart.data(), stddev.data(), 0, size_pop, NULL);
	}

	void cmaes_pop::reinit_cmaest() {
		size_t size_pop = m_inds.size();
		pop = cmaes_GetPop(&evo);
		for (size_t i = 0; i < size_pop; i++)
			reverse_x(m_inds[i]->variable().vect(), pop[i]);
		arFunvals = cmaes_GetArFunvals(&evo);
		for (size_t i = 0; i < size_pop; i++)
			arFunvals[i] = fit_map(m_inds[i]->solut());
		cmaes_InitDistribution(&evo, arFunvals);
	}

	void cmaes_pop::resizepop_cmaest() {
		size_t size_pop = m_inds.size();
		cmaes_ResizeLambda(&evo, size_pop);
	}

	void cmaes_pop::initialize() {
		size_t size_pop = m_inds.size();
		/* generate lambda new search points, sample population */
		pop = cmaes_SamplePopulation(&evo); /* do not change content of pop */
		for (size_t i = 0; i < size_pop; i++) {
			copy(pop[i], m_inds[i]->variable().vect());
		}
		/* resample the solution i until it satisfies given	box constraints (variable boundaries) */
		for (size_t i = 0; i < cmaes_Get(&evo, "popsize"); ++i) {
			if (cmaes_Get(&evo, "popsize") != size_pop)
				std::cout << size_pop << " " << cmaes_Get(&evo, "popsize") << std::endl;
			while (CONTINUOUS_CAST->check_boundary_violation(*m_inds[i]) != violation_type::None) {
				cmaes_ReSampleSingle(&evo, i);
				copy(pop[i], m_inds[i]->variable().vect());
			}
		}
		/* evalutate the solutions */
		arFunvals = cmaes_GetArFunvals(&evo);
		for (size_t i = 0; i < size_pop; i++) {
			m_inds[i]->evaluate();
			arFunvals[i] = fit_map(m_inds[i]->solut());
		}
	}
	
	EvalTag cmaes_pop::evolve() {
		EvalTag flag = EvalTag::Normal;
		size_t size_pop = m_inds.size();
		/* generate lambda new search points, sample population */
		pop = cmaes_SamplePopulation(&evo); /* do not change content of pop */
		for (size_t i = 0; i < size_pop; i++) {
			copy(pop[i], m_inds[i]->variable().vect());
		}
		/* resample the solution i until it satisfies given	box constraints (variable boundaries) */
		for (size_t i = 0; i < cmaes_Get(&evo, "popsize"); ++i) {
			while (CONTINUOUS_CAST->check_boundary_violation(*m_inds[i]) != violation_type::None) {
				cmaes_ReSampleSingle(&evo, i);
				copy(pop[i], m_inds[i]->variable().vect());
			}
		}
		/* evalutate the solutions */
		arFunvals = cmaes_GetArFunvals(&evo);
		for (size_t i = 0; i < size_pop; i++) {
			flag = m_inds[i]->evaluate();
			arFunvals[i] = fit_map(m_inds[i]->solut());
		}
		/* update the search distribution used for cmaes_SamplePopulation() */
		cmaes_UpdateDistribution(&evo, arFunvals);
		/* read instructions for printing output or changing termination conditions */
		//cmaes_ReadSignals(&evo, signalsFilePathName.c_str());//"cmaes_signals.par"
		//fflush(stdout); /* useful in MinGW */
		m_best_updated = false;
		m_worst_updated = false;
		m_iter++;
		return flag;
	}
	
	void cmaes_pop::copy(double* x, std::vector<Real>& vx) {
		auto& range = CONTINUOUS_CAST->range();
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			vx[j] = range[j].limit.first + (Real)x[j] * (range[j].limit.second - range[j].limit.first);
		}
	}

	void cmaes_pop::reverse_x(const std::vector<Real>& vx, double* x) {
		auto& range = CONTINUOUS_CAST->range();
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			x[j] = double((vx[j] - range[j].limit.first) / (range[j].limit.second - range[j].limit.first));
		}
	}

	void cmaes_pop::reverse_size(const std::vector<Real>& vstddev, double* stddev) {
		auto& range = CONTINUOUS_CAST->range();
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j) {
			stddev[j] = double(vstddev[j] / (range[j].limit.second - range[j].limit.first));
		}
	}

	double cmaes_pop::fit_map(Solution<>& s) {
		if (CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Minimization)
			return s.objective()[0];
		else
			return -s.objective()[0];
	}

	cmaes_pop::~cmaes_pop() {
		/* get best estimator for the optimum, xmean */
		xfinal = cmaes_GetNew(&evo, "xmean"); /* "xbestever" might be used as well */
		cmaes_exit(&evo); /* release memory */
		/* do something with final solution and finally release memory */
		free(xfinal);
	}
}
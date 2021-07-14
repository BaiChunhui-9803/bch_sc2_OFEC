#include "NEA2.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> msp_buffer;
#endif

namespace OFEC {
	NEA2::NEA2(param_map& v) : algorithm(v.at("algorithm name")), m_sizepop(v.at("population size")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	void NEA2::initialize() {
		add_subpops();
		m_initialized = true;
	}

	void NEA2::record() {
		size_t num_opt = CONTINUOUS_CAST->get_optima().number_variable();
		std::vector<Real> vals(num_opt + 1);
		vals[0] = CONTINUOUS_CAST->evaluations();
		Real dis;
		for (size_t i = 0; i < num_opt; i++) {
			dis = CONTINUOUS_CAST->get_optima().variable_min_dis(i);
			if (dis == 0)
				vals[i + 1] = -17;
			else
				vals[i + 1] = log10(dis);
		}
		measure::get_measure()->record(global::ms_global.get(), vals);
	}

#ifdef OFEC_DEMO
	void NEA2::updateBuffer() {
		if (!m_initialized) return;
		vector<vector<Solution<>*>> pops(this->m_subpops.size());
		for (size_t k = 0; k < pops.size(); ++k) {
			for (size_t i = 0; i < m_subpops[k].size(); ++i)
				pops[k].emplace_back(&m_subpops[k][i].solut());
		}
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
	}
#endif

	void NEA2::run_() {
		auto rf = EvalTag::Normal;
		size_t iterations = 0;
		while (!terminating()) {
			rf = m_subpops.evolve();
			if (rf == EvalTag::Terminate)
				break;
			for (size_t k = 0; k < m_subpops.size(); ++k)
				if (m_subpops[k].is_active() && stopTolFun(m_subpops[k]))
					m_subpops[k].set_active_flag(false);
			bool flag_all_inactive = true;
			for (size_t k = 0; k < m_subpops.size(); ++k) {
				if (m_subpops[k].is_active()) {
					flag_all_inactive = false;
					break;
				}
			}
			if (flag_all_inactive)
				add_subpops();
			iterations++;
		}
	}

	void NEA2::add_subpops() {
		size_t num_var = CONTINUOUS_CAST->variable_size();
		population<ind_type> global_sample(m_sizepop, num_var);
		global_sample.initialize();
		global_sample.evaluate();
		m_nbc.update_data(global_sample);
		m_nbc.clustering();
		auto clusters = m_nbc.get_clusters();
		std::vector<size_t> filtered_start_individuals;
		for (auto& cluster : clusters) {
			auto iter = cluster.begin();
			size_t center = *iter;
			while (++iter != cluster.end()) {
				if (global_sample[*iter].dominate(global_sample[center]))
					center = *iter;
			}
			filtered_start_individuals.push_back(center);
		}
		auto& domain = CONTINUOUS_CAST->range();
		Real diag = 0;
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			diag += pow(domain[j].limit.second - domain[j].limit.first, 2);
		diag = sqrt(diag);
		Real step_size;
		for (size_t start_ind : filtered_start_individuals) {
			step_size = 0.05 * diag;
			//step_size = max(0.025 * diag, global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.05 * diag, 0.025 * diag));
			auto pop = new pop_type(4 * pow(CONTINUOUS_CAST->variable_size(), 2), global_sample[start_ind].variable().vect(), step_size);
			pop->initialize();
			pop->set_id(m_subpops.size());
			m_subpops += pop;
		}
	}

	bool NEA2::stopTolFun(pop_type& subpop) {
		subpop.update_best();
		subpop.update_worst();
		return (subpop.best()[0]->objective_distance(*subpop.worst()[0]) < 1e-6) ? true : false;
	}
}

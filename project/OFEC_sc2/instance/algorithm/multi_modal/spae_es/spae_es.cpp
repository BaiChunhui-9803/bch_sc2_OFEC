#include "HLC_ES.h"
#include "../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include <functional>
#include "../../../../../ui/buffer/continuous/HLC/buffer_HLC.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	HLC_ES::HLC_ES(param_map& v) : algorithm(v.at("algorithm name")),
		m_subpop_size(v.at("subpopulation size")), m_num_segs(v.at("numPartition")),
		m_hlc(CONTINUOUS_CAST->variable_size()) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	void HLC_ES::initialize() {
		/* Initialize HLC */
		Solution<> temp(CONTINUOUS_CAST->objective_size(), CONTINUOUS_CAST->num_constraints(), CONTINUOUS_CAST->variable_size());
		std::vector<std::pair<Real, Real>> boundary(CONTINUOUS_CAST->variable_size());
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			boundary[j] = CONTINUOUS_CAST->range(j);
		m_hlc.initialize(boundary, m_num_segs);
		for (size_t i = 0; i < m_hlc.get_num_regions(); ++i) {
			const auto& box = m_hlc.get_box(i);
			for (size_t k = 0; k < CONTINUOUS_CAST->variable_size(); k++) {
				for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
					temp.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(box[j].first, box[j].second);
				temp.evaluate();
				m_hlc.input_sample(temp);
			}
		}
		m_hlc.clustering();

		/* Initialize population */
		for (const auto& cluster : m_hlc.get_clusters()) {
			pop_type* pop = new pop_type(m_subpop_size);
			for (size_t i = 0; i < pop->size(); ++i)
				init_in_cluster(cluster, (*pop)[i]);
			pop->reinit_cmaest();
			m_subpops += pop;
		}

		m_initialized = true;
	}

	void HLC_ES::record() {
		//if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
		//	// ******* Multi-Modal Optimization *******
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
		//	size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
		//	Real peak_ratio = (Real)num_opt_found / (Real)num_opt_known;
		//	Real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
		//	std::vector<Real> vals = { Real(evals), peak_ratio, success_rate };
		//	measure::get_measure()->record(global::ms_global.get(), vals);
		//}
		//else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
		//	// ******* Global Optimization ************
		//	size_t evals = CONTINUOUS_CAST->evaluations();
		//	Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
		//	std::vector<Real> vals = { Real(evals), err };
		//	measure::get_measure()->record(global::ms_global.get(), vals);
		//}
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

	void HLC_ES::run_() {
		size_t iterations = 1;
		while (!terminating()) {
			/* Reproduction */
			for (auto iter = m_subpops.begin(); iter != m_subpops.end();++iter) {
				if (is_converged(**iter)) {
					explore(iter);
				}
				else {
					exploit(**iter);
				}
			}
			if (iterations % 10 == 0) {
				m_hlc.clustering();
			}
			update_accu_pro_clu();
			iterations++;
		}
	}

	bool HLC_ES::is_converged(pop_type& subpop) {
		subpop.update_best();
		subpop.update_worst();
		return (subpop.best()[0]->objective_distance(*subpop.worst()[0]) < 1e-6) ? true : false;
	}

	void HLC_ES::update_accu_pro_clu() {
		m_hlc.update_potential();
		m_accu_pro_clu.resize(m_hlc.get_clusters().size());
		m_accu_pro_clu[0] = 1. / m_hlc.get_potential(0);
		for (size_t i = 1; i < m_accu_pro_clu.size(); ++i)
			m_accu_pro_clu[i] = m_accu_pro_clu[i - 1] + 1. / m_hlc.get_potential(i);
	}

	void HLC_ES::explore(iter_subpop& iter_subpop) {
		pop_type* pop = new pop_type(m_subpop_size);
		Real p = global::ms_global->m_uniform[caller::Algorithm]->next() * m_accu_pro_clu.back();
		size_t idx_clu = lower_bound(m_accu_pro_clu.begin(), m_accu_pro_clu.end(), p) - m_accu_pro_clu.begin();
		for (size_t i = 0; i < pop->size(); ++i)
			init_in_cluster(m_hlc.get_clusters()[idx_clu], (*pop)[i]);
		pop->reinit_cmaest();
		iter_subpop->reset(pop);
	}

	void HLC_ES::exploit(pop_type& subpop) {
		if (subpop.size() < 4)	return;
		subpop.evolve();
		for (size_t i = 0; i < subpop.size(); ++i)
			m_hlc.input_sample(subpop[i].solut());
	}

	void HLC_ES::init_in_cluster(const std::vector<size_t>& cluster, ind_type& ind) {
		size_t ridx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<size_t>(0, cluster.size());
		size_t immigrant_region = cluster[ridx];
		auto& box = m_hlc.get_box(immigrant_region);
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
			ind.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(box[j].first, box[j].second);
		ind.evaluate();
		m_hlc.input_sample(ind.solut());
	}

#ifdef OFEC_DEMO
	void HLC_ES::updateBuffer() {
		if (!m_initialized) return;
		vector<vector<Solution<>*>> pops(m_subpops.size());
		for (size_t k = 0; k < m_subpops.size(); k++) {
			for (size_t i = 0; i < m_subpops[k].size(); ++i)
				pops[k].emplace_back(&m_subpops[k][i].solut());
		}
		m_hlc.update_potential();
		dynamic_cast<Demo::buffer_HLC*>(Demo::msp_buffer.get())->updateBuffer_(&pops, m_hlc);
		size_t num_improved = 0, num_inds = 0;
		for (size_t k = 0; k < m_subpops.size(); k++) {
			num_inds += m_subpops[k].size();
			for (size_t i = 0; i < m_subpops[k].size(); ++i)
				if (m_subpops[k][i].is_improved())
					num_improved++;
		}
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			std::cout << evals << "\t" << err << "\t\t" << num_inds - m_inds_explore.size() << ":" << m_inds_explore.size() << std::endl;
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			std::cout << evals << "\t" << num_opt_found << "/" << num_opt_known << "\t" << num_inds - m_inds_explore.size() << ":" << m_inds_explore.size() << std::endl;
		}
	}
#endif
}

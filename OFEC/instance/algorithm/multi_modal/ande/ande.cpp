#include "ANDE.h"

#ifdef OFEC_DEMO
#include "../../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> msp_buffer;
#endif

namespace OFEC {
	ANDE::ANDE(param_map& v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")), m_apc(0.9, 100, 30) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
		CONTINUOUS_CAST->set_variable_track_flag(true);
	}

	void ANDE::initialize() {
		m_pop.initialize();
		m_pop.evaluate();
		m_pop.default_parameter();
	}

	void ANDE::run_() {
		std::vector<size_t> ridx;
		while (!terminating()) {
			m_apc.update_data(m_pop);
			m_apc.clustering();
			auto clusters = m_apc.clusters();
			for (auto cluster : clusters) {
				if (cluster.size() >= 4) {
					for (size_t i : cluster) {
						m_pop.select_in_neighborhood(3, cluster, ridx);
						m_pop[i].mutate(m_pop.F(), &m_pop[ridx[0]].solut(), &m_pop[ridx[1]].solut(), &m_pop[ridx[2]].solut());
						m_pop.recombine(i);
						m_pop[i].trial().evaluate();
						size_t idx_nearest = cluster[0];
						Real min_dis = m_pop[i].trial().variable_distance(m_pop[idx_nearest].solut());
						Real temp_dis;
						for (size_t j = 1; j < cluster.size(); ++j) {
							temp_dis = m_pop[i].trial().variable_distance(m_pop[cluster[j]].solut());
							if (min_dis > temp_dis) {
								min_dis = temp_dis;
								idx_nearest = cluster[j];
							}
						}
						if (m_pop[i].trial().dominate(m_pop[idx_nearest].solut())) {
							m_pop[idx_nearest].solut() = m_pop[i].trial();
						}
					}
				}
				CPA(cluster);
			}
			TLLS(clusters);
		}
	}

	void ANDE::record() {
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

	void ANDE::CPA(const std::vector<size_t>& cluster) {
		if (cluster.size() < 4) return;
		/* Find lbest */
		size_t lbest = cluster[0];
		for (size_t i = 0; i < cluster.size(); i++)	{
			if (m_pop[cluster[i]].dominate(m_pop[lbest]))
				lbest = cluster[i];
		}
		/* Find its neighbors */
		std::vector<Real> dis_to_lbest(cluster.size());
		std::vector<int> sequence;
		std::vector<int> neighbors;
		for (size_t i = 0; i < cluster.size(); i++) {
			dis_to_lbest[i] = m_pop[lbest].variable_distance(m_pop[cluster[i]]);
		}
		merge_sort(dis_to_lbest, dis_to_lbest.size(), sequence);
		for (size_t i = 1; i < sequence.size(); ++i) {
			neighbors.emplace_back(sequence[i]);
			if (neighbors.size() >= 5)
				break;
		}
		/* Determine the contour value f */
		Real f, f_lbest = m_pop[lbest].objective(0), f_i;
		if (CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Maximization)
			f = f_lbest + 0.2 * abs(f_lbest) + 0.1;
		else
			f = f_lbest - 0.2 * abs(f_lbest) - 0.1;
		/* Calculate the interpolated points */
		size_t D = CONTINUOUS_CAST->variable_size();
		const auto& x_lbest = m_pop[lbest].variable();
		std::vector<std::vector<Real>> inters(neighbors.size(), std::vector<Real>(D));
		for (size_t i = 0; i < inters.size(); i++) {
			const auto& x_i = m_pop[neighbors[i]].variable();
			f_i = m_pop[neighbors[i]].objective(0);
			for (size_t j = 0; j < D; j++) {
				inters[i][j] = x_lbest[j] + (f - f_lbest) / (f_i - f_lbest) * (x_i[j] - x_lbest[j]);
			}
		}
		/* Estimate the potential optima */
		std::vector<Real> optima(D, 0);
		for (size_t j = 0; j < D; j++) {
			for (const auto& inter : inters) {
				optima[j] += inter[j];
			}
			optima[j] /= inters.size();
		}
		/* Compete the potential optima with the lbest */
		Solution<> temp_sol(1, 0, D);
		temp_sol.variable().vect() = optima;
		temp_sol.evaluate();
		if (temp_sol.dominate(m_pop[lbest].solut())) {
			//std::cout << "CPA works" << std::endl;
			m_pop[lbest].solut() = temp_sol;
		}
	}

	void ANDE::TLLS(const std::vector<std::vector<size_t>>& clusters) {
		/* Generate the sample standard deviation sigma */
		size_t D = CONTINUOUS_CAST->variable_size();
		size_t FEs = CONTINUOUS_CAST->evaluations();
		Real sigma = pow(10, (-1 - (10 / (Real)D + 3) * (Real)FEs / (Real)m_MaxFEs));
		/* Calculate the niche-level search probability P */
		size_t n = clusters.size();
		std::vector<Real> P(n);
		std::vector<Real> f_niche_seed(n);
		for (size_t i = 0; i < n; i++) {
			size_t lbest = clusters[i][0];
			for (size_t k = 1; k < clusters[i].size(); k++) {
				if (m_pop[clusters[i][k]].dominate(m_pop[lbest])) {
					lbest = clusters[i][k];
					f_niche_seed[i] = m_pop[lbest].objective(0);
				}
			}
		}
		std::vector<int> sequence;
		bool ascending = CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Minimization;
		ascending = !ascending; // Add a exclamatory mark for sorting from worse to better
		merge_sort(f_niche_seed, n, sequence, ascending);
		std::vector<size_t> r(n);
		for (size_t i = 0; i < n; i++)
			r[sequence[i]] = i+1;
		for (size_t i = 0; i < n; i++)
			P[i] = (Real)r[i] / n;
		/*  */
		Real rand;
		for (size_t i = 0; i < n; i++) {
			rand = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (rand < P[i]) {
				/*  Calculater the individual-level local search probability */
				size_t n_i = clusters[i].size();
				std::vector<Real> P_i(n_i);
				std::vector<Real> f_inds_in_niche(n_i);
				for (size_t k = 0; k < n_i; k++)
					f_inds_in_niche[k] = m_pop[clusters[i][k]].objective(0);
				merge_sort(f_inds_in_niche, n_i, sequence, ascending);
				r.resize(n_i);
				for (size_t k = 0; k < n_i; k++)
					r[sequence[k]] = i + 1;
				for (size_t k = 0; k < n_i; k++)
					P_i[k] = (Real)r[k] / n_i;
				/* */
				for (size_t k = 0; k < n_i; k++) {
					rand = global::ms_global->m_uniform[caller::Algorithm]->next();
					if (rand < P_i[k]) {
						const auto& x_ik = m_pop[clusters[i][k]].variable();
						Solution<> s1(1, 0, D), s2(1, 0, D);
						for (size_t j = 0; j < D; j++) {
							s1.variable()[j] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(x_ik[j], sigma);
							s2.variable()[j] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(x_ik[j], sigma);
						}
						s1.evaluate();
						s2.evaluate();
						s1 = s1.dominate(s2) ? s1 : s2;
						if (s1.dominate(m_pop[clusters[i][k]].solut())) {
							//std::cout << "TLLS works" << std::endl;
							m_pop[clusters[i][k]].solut() = s1;
						}
					}
				}

			}
		}
	}

#ifdef OFEC_DEMO
	void ANDE::updateBuffer() {
		auto clusters = m_apc.clusters();
		if (clusters.empty()) return;
		vector<vector<Solution<>*>> pops(clusters.size());
		for (size_t k = 0; k < clusters.size(); k++) {
			for (size_t i : clusters[k])
				pops[k].emplace_back(&m_pop[i].solut());
		}
		dynamic_cast<Demo::buffer_cont*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
		Real num_improved = 0;
		for (size_t i = 0; i < m_pop.size(); ++i)
			if (m_pop[i].is_improved())
				num_improved++;
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			std::cout << evals << "\t" << err << "\tImprove Ratio:" << num_improved / m_pop.size() * 100 << "\%" << std::endl;
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			std::cout << evals << "\t" << num_opt_found << "/" << num_opt_known << "\tImprove Ratio:" << num_improved / m_pop.size() * 100 << "\%" << std::endl;
		}
	}
#endif
}
#include "EMO_MMO.h"
#include "../MOEA/NSGAII/NSGAII.h"
#include <cmath>
#include "../../../utility/functional.h"
#include <deque>
#include <fstream>

#ifdef OFEC_DEMO
#include "../../../../ui/buffer/continuous/buffer_cont.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	EMO_MMO::EMO_MMO(param_map& v) :
		algorithm(v.at("algorithm name")),
		m_size_pop(v.at("population size")),
		m_ratio_MOFLA_FEs(0.5),
		m_t_max(0),
		m_eta(0.1),
#ifdef OFEC_DEMO
		m_state(state::MOFLA),
		m_update_buffer(false),
#endif
		m_MOFLA_pop(m_size_pop) {
		m_t_max = m_ratio_MOFLA_FEs * ceil(static_cast<int>(v.at("maximum evaluations")) / m_size_pop);
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void EMO_MMO::initialize() {
		m_initialized = true;
	}

	void EMO_MMO::record() {
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

#ifdef OFEC_DEMO
	void EMO_MMO::updateBuffer() {
		if (!m_update_buffer) return;
		vector<vector<Solution<>*>> pops;
		switch (m_state)
		{
		case OFEC::EMO_MMO::state::MOFLA: {
			pops.resize(1);
			for (size_t i = 0; i < m_D.size(); i++)
				pops[0].push_back(m_D[i].get());
			break;
		}
		case OFEC::EMO_MMO::state::APD:
			break;
		case OFEC::EMO_MMO::state::LS: {
			pops.resize(m_LS_pops.size());
			for (size_t k = 0; k < m_LS_pops.size(); k++) {
				for (size_t i = 0; i < m_LS_pops[k].size(); i++) {
					pops[k].push_back(&m_LS_pops[k][i].solut());
				}
			}
			break;
		}
		default:
			break;
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
		m_update_buffer = false;
	}
#endif

	void EMO_MMO::run_() {
		MOFLA();
		binary_cutting_APD();
		local_search();
	}

	void EMO_MMO::MOFLA() {
#ifdef OFEC_DEMO
		m_state = state::MOFLA;
#endif
		m_MOFLA_pop.set_cr(0.9);
		m_MOFLA_pop.set_mr(0.01);
		m_MOFLA_pop.set_eta(50, 20);
		m_MOFLA_pop.initialize();
		m_MOFLA_pop.evaluate();
#ifdef OFEC_DEMO
		m_update_buffer = true;
#endif
		for (size_t i = 0; i < m_MOFLA_pop.size(); ++i) {
			if (m_D_x.count(m_MOFLA_pop[i].variable().vect()) == 0) {
				m_D.emplace_back(std::make_unique<Solution<>>(m_MOFLA_pop[i].solut()));
				m_D_x.insert(m_MOFLA_pop[i].variable().vect());
			}
		}
		m_MOFLA_pop.resize_objective(2);
		NSGAII<individual<>> nsgaii(2, { CONTINUOUS_CAST->opt_mode(0), optimization_mode::Maximization });
		std::vector<individual<>> offspring(2*m_size_pop, individual<>(2, 0, CONTINUOUS_CAST->variable_size()));
		transfered_MOP mop(m_t_max, CONTINUOUS_CAST->variable_size(), 2*m_size_pop);
		size_t t = 0;
		std::vector<size_t> rand_perm(m_MOFLA_pop.size());
		std::iota(rand_perm.begin(), rand_perm.end(), 0);
		while (!terminating() && t < m_t_max) {
			global::ms_global->m_uniform[caller::Algorithm]->shuffle(rand_perm.begin(), rand_perm.end());
			for (size_t i = 0; i < m_MOFLA_pop.size(); i += 2) {
				m_MOFLA_pop.crossover(rand_perm[i], rand_perm[i+1], offspring[i], offspring[i + 1]);
				m_MOFLA_pop.mutate(offspring[i]);
				m_MOFLA_pop.mutate(offspring[i + 1]);
			}
			for (size_t i = 0; i < m_MOFLA_pop.size(); ++i) {
				offspring[i].evaluate();
				offspring[i + m_MOFLA_pop.size()] = m_MOFLA_pop[i];
			}
			mop.update_norm_x(offspring);
			mop.update_delta(t);
			mop.evaluate_2nd_obj(offspring);
			nsgaii.survivor_selection(m_MOFLA_pop.inds(), offspring);
			for (size_t i = 0; i < m_MOFLA_pop.size(); ++i) {
				if (m_D_x.count(m_MOFLA_pop[i].variable().vect()) == 0) {
					m_D.emplace_back(std::make_unique<Solution<>>(m_MOFLA_pop[i].solut()));
					m_D.back()->resize_objective(1);           // only preserve the original objetive value
					m_D_x.insert(m_MOFLA_pop[i].variable().vect());
				}
			}
			t++;
#ifdef OFEC_DEMO
			m_update_buffer = true;
#endif
		}
	}

	void EMO_MMO::binary_cutting_APD()	{
#ifdef OFEC_DEMO
		m_state = state::APD;
#endif
		std::unordered_set<size_t> D_c;
		for (size_t i = 0; i < m_D.size(); i++)
			D_c.insert(i);
		landscape_cutting(D_c, m_eta);
		std::vector<std::vector<Real>> manh_dis_mat(m_D.size(), std::vector<Real>(m_D.size()));
		update_manh_dis_mat(manh_dis_mat, D_c);
		while (!D_c.empty()) {
			APD(manh_dis_mat, D_c);
			landscape_cutting(D_c, 0.5);
		}
	}

	void EMO_MMO::local_search() {
#ifdef OFEC_DEMO
		m_state = state::LS;
#endif
		const auto& domain = CONTINUOUS_CAST->range();
		size_t size_var = CONTINUOUS_CAST->variable_size();
		std::vector<Real> size_dim(size_var);
		for (size_t j = 0; j < size_var; j++){
			size_dim[j] = (domain[j].limit.second - domain[j].limit.first) * 0.05;
		}
		std::set<size_t> idx_bests;
		for (size_t k = 0; k < m_P.size(); k++) {
			size_t idx_best = *m_P[k].begin();
			for (size_t idx_ind : m_P[k]) {
				if (m_D[idx_ind]->dominate(*m_D[idx_best]))
					idx_best = idx_ind;
			}
			if (idx_bests.count(idx_best) == 1)
				continue;
			else
				idx_bests.insert(idx_best);
			auto pop = new DE::population<>(10);
			(*pop)[0] = *m_D[idx_best];
			for (size_t j = 0; j < size_var; j++) {
				Real min_x_j = m_D[idx_best]->variable()[j] - size_dim[j] / 2;
				Real max_x_j = m_D[idx_best]->variable()[j] + size_dim[j] / 2;
				for (size_t i = 1; i < 10; i++) {
					(*pop)[i].variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<Real>(min_x_j, max_x_j);
				}
			}
			for (size_t i = 1; i < 10; i++)
				(*pop)[i].evaluate();
			m_LS_pops += pop;
		}

		std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + std::string(global::ms_arg.at("problem name")) + "_APD.csv");
		std::stringstream out;
		out << "popID,x1,x2\n";
		//for (size_t k = 0; k < m_LS_pops.size(); k++) {
		//	for (size_t i = 0; i < m_LS_pops[k].size(); i++) {
		//		out << k << "," << m_LS_pops[k][i].variable()[0] << "," << m_LS_pops[k][i].variable()[1] << std::endl;
		//	}
		//}
		size_t k = 0;
		for (size_t idx_best : idx_bests) {
			out << k++ << "," << m_D[idx_best]->variable()[0] << "," << m_D[idx_best]->variable()[1] << std::endl;
		}
		outfile << out.str();
		outfile.close();

		while (!terminating()) {
			m_LS_pops.evolve();
#ifdef OFEC_DEMO
			m_update_buffer = true;
#endif
		}
	}

	void EMO_MMO::landscape_cutting(std::unordered_set<size_t>& D_c, Real eta) {
		Real min_obj, max_obj;
		auto iter = D_c.begin();
		min_obj = max_obj = m_D[*iter]->objective(0);
		while (++iter != D_c.end()) {
			if (m_D[*iter]->objective(0) > max_obj)
				max_obj = m_D[*iter]->objective(0);
			if (m_D[*iter]->objective(0) < min_obj)
				min_obj = m_D[*iter]->objective(0);
		}
		Real threshold;
		if (CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Maximization) {
			threshold = max_obj - eta * (max_obj - min_obj);
			for (auto iter = D_c.begin(); iter != D_c.end();) {
				if (m_D[*iter]->objective(0) > threshold)
					++iter;
				else
					iter = D_c.erase(iter);
			}
		}
		else {
			threshold = min_obj + eta * (max_obj - min_obj);
			for (auto iter = D_c.begin(); iter != D_c.end();) {
				if (m_D[*iter]->objective(0) < threshold)
					++iter;
				else
					iter = D_c.erase(iter);
			}
		}
		if (D_c.size() > 5000) {
			std::vector<size_t> D_c_cp;
			for (size_t idx_ind : D_c)
				D_c_cp.push_back(idx_ind);
			global::ms_global->m_uniform[caller::Algorithm]->shuffle(D_c_cp.begin(), D_c_cp.end());
			D_c.clear();
			for (size_t i = 0; i < 5000; ++i)
				D_c.insert(D_c_cp[i]);
		}
	}

	void EMO_MMO::APD(std::vector<std::vector<Real>>& manh_dis_mat, std::unordered_set<size_t> D_c) {
		int k = 0;
		size_t num_B = D_c.size();
		normc_manh_dis_mat(manh_dis_mat, D_c);
		while (num_B >1) {
			k++;
			auto iter = D_c.begin();
			auto iter_sigma = iter;
			Real min_dis, sigma = min_manh_dis(manh_dis_mat, D_c, *iter);
			while (++iter != D_c.end()) {
				min_dis = min_manh_dis(manh_dis_mat, D_c, *iter);
				if (min_dis > sigma) {
					sigma = min_dis;
					iter_sigma = iter;
				}
			}
			std::list<size_t> psi_k;
			std::deque<size_t> neighbors = { *iter_sigma };
			D_c.erase(iter_sigma);
			while (!neighbors.empty()) {
				psi_k.push_back(neighbors.front());
				for (iter = D_c.begin(); iter != D_c.end();) {
					if (manh_dis_mat[neighbors.front()][*iter] == 0) {
						std::cout << "error" << std::endl;
						std::cout << "neighbors.front(): " << neighbors.front() << std::endl;
						std::cout << "*iter: " << *iter << std::endl;
						std::cout << m_D[neighbors.front()]->variable()[0] << ", " << m_D[neighbors.front()]->variable()[1] << std::endl;
						std::cout << m_D[*iter]->variable()[0] << ", " << m_D[*iter]->variable()[1] << std::endl;
					}
					if (manh_dis_mat[neighbors.front()][*iter] <= std::max<Real>(sigma, 1e-3)) {
						neighbors.push_back(*iter);
						iter = D_c.erase(iter);
					}
					else
						iter++;
				}
				num_B--;
				neighbors.pop_front();
			}
			m_P.push_back(std::move(psi_k));
		}
	}

	void EMO_MMO::update_manh_dis_mat(std::vector<std::vector<Real>>& manh_dis_mat, const std::unordered_set<size_t>& D_c) {
		for (auto iter1 = D_c.begin(); iter1 != D_c.end(); iter1++) {
			auto iter2 = iter1;
			iter2++;
			for (; iter2 != D_c.end(); iter2++) {
				manh_dis_mat[*iter1][*iter2] = manhattan_distance(m_D[*iter1]->variable().begin(), m_D[*iter1]->variable().end(), m_D[*iter2]->variable().begin());
				manh_dis_mat[*iter2][*iter1] = manh_dis_mat[*iter1][*iter2];
			}
		}
	}

	void EMO_MMO::normc_manh_dis_mat(std::vector<std::vector<Real>>& manh_dis_mat, const std::unordered_set<size_t>& D_c) {
		std::vector<std::vector<Real>> normc_x(D_c.size(), std::vector<Real>(CONTINUOUS_CAST->variable_size()));
		for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); j++) {
			Real sum = 0;
			for (size_t idx_ind : D_c)	{
				sum += pow(m_D[idx_ind]->variable()[j], 2);
			}
			Real beta = sqrt(1 / sum);
			size_t i = 0;
			for (size_t idx_ind : D_c)	{
				normc_x[i][j] = beta * m_D[idx_ind]->variable()[j];
				i++;
			}
		}
		size_t i = 0;
		for (auto iter1 = D_c.begin(); iter1 != D_c.end(); iter1++) {
			size_t k = i+1;
			auto iter2 = iter1;
			iter2++;
			for (; iter2 != D_c.end(); iter2++) {
				manh_dis_mat[*iter1][*iter2] = manhattan_distance(normc_x[i].begin(), normc_x[i].end(), normc_x[k].begin());
				manh_dis_mat[*iter2][*iter1] = manh_dis_mat[*iter1][*iter2];
				k++;
			}
			i++;
		}
	}

	Real EMO_MMO::min_manh_dis(const std::vector<std::vector<Real>>& manh_dis_mat, const std::unordered_set<size_t>& D_c, size_t i) {
		auto iter = D_c.begin();
		if (*iter == i) iter++;
		Real min_dis = manh_dis_mat[i][*iter];
		while (++iter != D_c.end()) {
			if (*iter == i) continue;
			if (manh_dis_mat[i][*iter] < min_dis)
				min_dis = manh_dis_mat[i][*iter];
		}
		return min_dis;
	}

	transfered_MOP::transfered_MOP(size_t t_max, size_t size_var, size_t size_pop) :
		m_t_max(t_max), 
		m_delta(0),
		m_N(size_pop),
		m_size_var(size_var),
		m_norm_x(size_pop, std::vector<int>(size_var)),
		m_manh_dis(size_pop, std::vector<int>(size_pop, 0)) {}

	void transfered_MOP::evaluate_2nd_obj(std::vector<individual<>>& offspring) {
		for (size_t i = 0; i < m_N; i++) {
			int sum = 0,  num = 0;
			for (size_t k = 0; k < m_N; k++) {
				if (m_manh_dis[i][k] < m_delta) {
					sum += m_manh_dis[i][k];
					num++;
				}
			}
			offspring[i].objective(1) = sum / m_delta - num;
		}
	}

	void transfered_MOP::update_delta(int t) {
		update_manh_dis();
		int max = min_manh_dis(0);
		for (size_t i = 1; i < m_N; i++) {
			int min_dis = min_manh_dis(i);
			if (max < min_dis)
				max = min_dis;
		}
		m_delta = (1 - Real(t - 1) / m_t_max) * max;
	}

	void transfered_MOP::update_norm_x(const std::vector<individual<>>& pop) {
		std::vector<Real> x_max(m_size_var), x_min(m_size_var);
		for (size_t j = 0; j < m_size_var; j++) {
			x_max[j] = x_min[j] = pop[0].variable()[j];
			for (size_t i = 1; i < m_N; i++) {
				if (x_max[j] < pop[i].variable()[j])
					x_max[j] = pop[i].variable()[j];
				if (x_min[j] > pop[j].variable()[j])
					x_min[j] = pop[j].variable()[j];
			}
		}
		for (size_t i = 0; i < m_N; i++) {
			auto& x = pop[i].variable();
			for (size_t j = 0; j < m_size_var; j++) {
				m_norm_x[i][j] = floor((m_N - 1) * (x[j] - x_min[j]) / (x_max[j] - x_min[j])) + 1;
			}
		}
	}

	void transfered_MOP::update_manh_dis() {
		for (size_t i = 0; i < m_N; i++) {
			for (size_t k = i + 1; k < m_N; k++) {
				m_manh_dis[i][k] = 0;
				for (size_t j = 0; j < m_size_var; j++) {
					m_manh_dis[i][k] += abs(m_norm_x[i][j] - m_norm_x[k][j]);
				}
				m_manh_dis[k][i] = m_manh_dis[i][k];
			}
		}
	}
	
	int transfered_MOP::min_manh_dis(size_t i) {
		int min_dis = i != 0 ? m_manh_dis[i][0] : m_manh_dis[i][1];
		for (size_t k = 0; k < m_N; k++) {
			if (i == k) continue;
			if (min_dis > m_manh_dis[i][k])
				min_dis = m_manh_dis[i][k];
		}
		return min_dis;
	}
}

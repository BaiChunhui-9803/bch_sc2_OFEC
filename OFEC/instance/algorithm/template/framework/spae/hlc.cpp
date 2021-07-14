#include "./hlc.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../../../utility/functional.h"
#include "../../../../../utility/nondominated_sorting/filter_sort.h"
#include <numeric>
#include "logarithmic.h"
#include "../../../global/canonical_de/canonical_de.h"
#include "../../../../../utility/min_max_heap.h"
#include <unordered_set>
#include <unordered_map>
#include "../../../../../utility/kd-tree/nanoflann.hpp"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace OFEC {
	HLC::HLC(size_t size_var, size_t num_divs) :
		m_num_divs(num_divs),
		m_size_var(size_var),
		m_opt_mode_sol(1),
		m_opt_mode_bsn_atrct(2) {}

	void HLC::initialize(
		const std::vector<std::pair<Real, Real>> &boundary, OptMode opt_mode, int init_num)
	{
		m_opt_mode_sol[0] = opt_mode;
		m_opt_mode_bsn_atrct[0] = opt_mode;
		m_opt_mode_bsn_atrct[1] = OptMode::Minimize;
		m_subspace_tree.reset(new SplitTree(m_size_var, 2));
		m_subspace_tree->setInitBox(boundary);
		m_subspace_tree->inputRatioData(std::vector<Real>(init_num, 1. / init_num));
		m_subspace_tree->buildIndex();
		m_info_subspaces.resize(init_num);
		for (size_t i = 0; i < init_num; i++)
			m_subspace_tree->findNeighbor(i, m_info_subspaces[i].neighbors);
		m_info_bsns_atrct.resize(1);
		m_info_bsns_atrct[0].subspaces.resize(init_num);
		std::iota(m_info_bsns_atrct[0].subspaces.begin(), m_info_bsns_atrct[0].subspaces.end(), 0);
		m_info_bsns_atrct[0].volume = 0;
		for (size_t i = 0; i < init_num; i++)
			m_info_subspaces[i].id_bsn_atr = 0;
		for (size_t i = 0; i < init_num; i++)
			m_info_bsns_atrct[0].volume += m_subspace_tree->getBoxVolume(i);
		divideBsnAtrct(0);
	}

	void HLC::inputSample(const Solution<> &sol) {
		m_his_inds.push_back(std::make_shared<const Solution<>>(sol));
		if (!m_worst_ind || m_worst_ind->dominate(*m_his_inds.back(), m_opt_mode_sol))
			m_worst_ind = m_his_inds.back();
		updateSubspaceInfo(m_his_inds.back());
	}

	void HLC::divideBsnAtrct(size_t id_bsn_atrct) {
		auto &info = m_info_bsns_atrct.at(id_bsn_atrct);
		info.vols_divs.clear();
		info.div_to_ssp_atp.clear();
		info.ssp_atp_to_div.clear();
		for (size_t id_subspace : info.subspaces)
			m_info_subspaces.at(id_subspace).atomspace_tree.reset();
		if (info.subspaces.size() <= m_num_divs) {
			for (size_t id_subspace : info.subspaces) {
				info.vols_divs.emplace_back(m_subspace_tree->getBoxVolume(id_subspace));
				info.ssp_atp_to_div[id_subspace][-1] = info.div_to_ssp_atp.size();
				info.div_to_ssp_atp.push_back({ std::make_pair(id_subspace, -1) });
			}
			while (info.div_to_ssp_atp.size() < m_num_divs) {
				size_t div_max = 0;
				for (size_t i = 1; i < info.vols_divs.size(); i++) {
					if (info.vols_divs[i] > info.vols_divs[div_max])
						div_max = i;
				}
				size_t aff_ssp = info.div_to_ssp_atp[div_max].front().first;
				if (m_info_subspaces.at(aff_ssp).atomspace_tree) {
					size_t aff_asp = info.div_to_ssp_atp[div_max].front().second;
					size_t new_asp = m_info_subspaces.at(aff_ssp).atomspace_tree->splitRegion(aff_asp);
					info.ssp_atp_to_div[aff_ssp][new_asp] = info.div_to_ssp_atp.size();
					info.div_to_ssp_atp.push_back({ std::make_pair(aff_ssp, new_asp) });
				}
				else {
					m_info_subspaces.at(aff_ssp).atomspace_tree.reset(new SplitTree(m_size_var, 2));
					m_info_subspaces.at(aff_ssp).atomspace_tree->setInitBox(m_subspace_tree->getBox(aff_ssp));
					m_info_subspaces.at(aff_ssp).atomspace_tree->inputRatioData({ 0.5,0.5 });
					m_info_subspaces.at(aff_ssp).atomspace_tree->buildIndex();	
					info.ssp_atp_to_div[aff_ssp][0] = div_max;
					info.div_to_ssp_atp[div_max].front().second = 0;
					info.ssp_atp_to_div[aff_ssp][1] = info.div_to_ssp_atp.size();
					info.div_to_ssp_atp.push_back({ std::make_pair(aff_ssp, 1) });
				}
				info.vols_divs[div_max] /= 2;
				info.vols_divs.push_back(info.vols_divs[div_max]);
			}
		}
		else {
			std::unordered_set<size_t> ssps;
			std::unordered_map<size_t, size_t> ids_in_bsn;
			std::vector<size_t> aff_ssp(info.subspaces.size());
			std::vector<Real> volume(info.subspaces.size());
			std::vector<Real> min_sum_vol(info.subspaces.size());
			std::vector<std::vector<size_t>> nbrs(info.subspaces.size());
			std::vector<std::unordered_map<size_t, int>> ids_in_nbr(info.subspaces.size());
			std::vector<MinMaxHeap<Real>> nbrs_heap;
			std::vector<std::list<size_t>> cncld_ssps(info.subspaces.size());
			size_t id_in_bsn = 0;
			for (size_t id_subspace : info.subspaces) {
				ssps.insert(id_subspace);		
				volume[id_in_bsn++] = m_subspace_tree->getBoxVolume(id_subspace);
			}
			id_in_bsn = 0;
			for (size_t id_subspace : info.subspaces) {
				ids_in_bsn[id_subspace] = id_in_bsn;
				aff_ssp[id_in_bsn] = id_subspace;	
				cncld_ssps[id_in_bsn].push_back(id_subspace);
				std::vector<Real> nbr_vols;
				size_t id_in_nbr = 0;
				for (size_t nbr_ssp : m_info_subspaces.at(id_subspace).neighbors) {
					if (ssps.count(nbr_ssp)) {
						nbrs[id_in_bsn].push_back(nbr_ssp);
						ids_in_nbr[id_in_bsn][nbr_ssp] = id_in_nbr++;
						nbr_vols.push_back(m_subspace_tree->getBoxVolume(nbr_ssp));
					}
				}
				nbrs_heap.emplace_back(nbr_vols);
				size_t nbr_min = nbrs[id_in_bsn][nbrs_heap.back().top()];
				min_sum_vol[id_in_bsn] = volume[id_in_bsn] + volume[ids_in_bsn[nbr_min]];
				id_in_bsn++;
			}

			MinMaxHeap heap(min_sum_vol);
			std::set<size_t> del_ssps;
			while (del_ssps.size() < info.subspaces.size() - m_num_divs) {
				id_in_bsn = heap.top();
				size_t id_ssp_to_merge = info.subspaces[id_in_bsn];
				size_t id_ssp_merged = nbrs[id_in_bsn][nbrs_heap[id_in_bsn].top()];
				volume[id_in_bsn] += volume[ids_in_bsn[id_ssp_merged]];
				for (size_t ssp : cncld_ssps[ids_in_bsn[id_ssp_merged]]) {
					aff_ssp[ids_in_bsn[ssp]] = aff_ssp[id_in_bsn];
					cncld_ssps[id_in_bsn].push_back(ssp);
				}
				cncld_ssps[ids_in_bsn[id_ssp_merged]].clear();
				for (size_t n : nbrs[ids_in_bsn[id_ssp_merged]]) {
					if (del_ssps.count(n) > 0 || n == id_ssp_to_merge)
						continue;
					if (ids_in_nbr[ids_in_bsn[n]].count(id_ssp_to_merge) == 0) {
						nbrs[ids_in_bsn[n]][ids_in_nbr[ids_in_bsn[n]][id_ssp_merged]] = id_ssp_to_merge;
						ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge] = ids_in_nbr[ids_in_bsn[n]][id_ssp_merged];
						ids_in_nbr[ids_in_bsn[n]][id_ssp_merged] = -1;
						nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);

						ids_in_nbr[id_in_bsn][n] = nbrs[id_in_bsn].size();
						nbrs[id_in_bsn].push_back(n);
						nbrs_heap[id_in_bsn].add(volume[ids_in_bsn[n]]);
					}
					else {
						nbrs_heap[ids_in_bsn[n]].remove(ids_in_nbr[ids_in_bsn[n]][id_ssp_merged]);
						nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);
					}
					size_t nbr_min = nbrs[ids_in_bsn[n]][nbrs_heap[ids_in_bsn[n]].top()];
					min_sum_vol[ids_in_bsn[n]] = volume[ids_in_bsn[n]] + volume[ids_in_bsn[nbr_min]];
					heap.updateValue(ids_in_bsn[n], min_sum_vol[ids_in_bsn[n]]);
				}
				for (size_t n : nbrs[id_in_bsn]) {
					if (del_ssps.count(n) > 0)
						continue;
					nbrs_heap[ids_in_bsn[n]].updateValue(ids_in_nbr[ids_in_bsn[n]][id_ssp_to_merge], volume[id_in_bsn]);
					size_t nbr_min = nbrs[ids_in_bsn[n]][nbrs_heap[ids_in_bsn[n]].top()];
					min_sum_vol[ids_in_bsn[n]] = volume[ids_in_bsn[n]] + volume[ids_in_bsn[nbr_min]];
					heap.updateValue(ids_in_bsn[n], min_sum_vol[ids_in_bsn[n]]);
				}

				nbrs_heap[id_in_bsn].remove(ids_in_nbr[id_in_bsn][id_ssp_merged]);
				size_t nbr_min = nbrs[id_in_bsn][nbrs_heap[id_in_bsn].top()];
				min_sum_vol[id_in_bsn] = volume[id_in_bsn] + volume[ids_in_bsn[nbr_min]];
				heap.updateValue(id_in_bsn, min_sum_vol[id_in_bsn]);

				heap.remove(ids_in_bsn[id_ssp_merged]);
				del_ssps.insert(id_ssp_merged);
			}

			std::unordered_map<size_t, size_t> ssps_to_div;
			size_t id_div = 0;
			for (size_t id_subspace : info.subspaces) {
				if (ssps_to_div.count(aff_ssp[ids_in_bsn[id_subspace]]) == 0) {
					ssps_to_div[aff_ssp[ids_in_bsn[id_subspace]]] = id_div++;
				}
			}
			info.div_to_ssp_atp.resize(id_div);
			for (size_t id_subspace : info.subspaces) {
				id_div = ssps_to_div[aff_ssp[ids_in_bsn[id_subspace]]];
				info.div_to_ssp_atp[id_div].emplace_back(id_subspace, -1);
				info.ssp_atp_to_div[id_subspace][-1] = id_div;
			}
			info.vols_divs.resize(m_num_divs);
			for (size_t i = 0; i < m_num_divs; i++) {
				info.vols_divs[i] = 0;
				for (auto& div : info.div_to_ssp_atp[i]) {
					info.vols_divs[i] += m_subspace_tree->getBoxVolume(div.first);
				}
			}
		}
	}

	void HLC::updateSubspaceInfo(const std::shared_ptr<const Solution<>> &ptr_sol) {
		size_t reg = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
		auto &info = m_info_subspaces[reg];
		if (!info.best_sol || ptr_sol->dominate(*info.best_sol, m_opt_mode_sol)) {
			info.stats[0] = ptr_sol->objective(0);
			info.best_sol = ptr_sol;
		}
		if (!info.worst_sol || info.worst_sol->dominate(*ptr_sol, m_opt_mode_sol)) {
			info.worst_sol = ptr_sol;
		}
		info.sols.push_back(ptr_sol);
		info.stats[1]++;
	}

	bool HLC::isAdjacent(const std::vector<std::pair<Real, Real>> &box1, const std::vector<std::pair<Real, Real>> &box2) {
		for (size_t j = 0; j < box1.size(); ++j) {
			if (box1[j].second < box2[j].first || box2[j].second < box1[j].first)
				return false;
		}
		return true;
	}

	void HLC::regressCovLines() {
		std::vector<std::thread> thrds;
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			if (m_info_bsns_atrct.at(i).coverages.empty()) continue;
			thrds.push_back(std::thread(&HLC::regressCovLine, this, i));
		}
		for (auto &thrd : thrds)
			thrd.join();
	}

	void HLC::interpolateMissing() {
		// Interpolation of objective values in the unsampled subspaces
		std::vector<size_t> unsampled_ssps;
		std::unordered_map<size_t, size_t> id_in_uspl_ssps;
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (m_info_subspaces[i].sols.empty()) {
				id_in_uspl_ssps[i] = unsampled_ssps.size();
				unsampled_ssps.push_back(i);
			}
		}
		std::vector<int> num_miss_nbrs(unsampled_ssps.size(), 0);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (m_info_subspaces[i].sols.empty()) {
				for (size_t n : m_info_subspaces.at(i).neighbors) {
					if (id_in_uspl_ssps.count(n) > 0)
						num_miss_nbrs[id_in_uspl_ssps[n]]++;
				}
			}
		}
		MinMaxHeap heap(num_miss_nbrs);
		m_interpolated.clear();
		while (m_interpolated.size() < unsampled_ssps.size()) {
			size_t id_top = heap.top();
			size_t id_ssp = unsampled_ssps[id_top];
			m_info_subspaces[id_ssp].stats[0] = 0;
			int num = 0;
			for (size_t n : m_info_subspaces.at(id_ssp).neighbors) {
				if (!m_info_subspaces[n].sols.empty() || m_interpolated.count(n) > 0) {
					m_info_subspaces[id_ssp].stats[0] += m_info_subspaces[n].stats[0];
					num++;
				}
			}
			m_info_subspaces[id_ssp].stats[0] /= num;
			m_interpolated.insert(id_ssp);
			heap.remove(id_top);
			for (size_t n : m_info_subspaces.at(id_ssp).neighbors) {
				if (id_in_uspl_ssps.count(n) > 0) {
					size_t id_in_heap = id_in_uspl_ssps.at(n);
					num_miss_nbrs[id_in_heap]--;
					heap.updateValue(id_in_heap, num_miss_nbrs[id_in_heap]);
				}
			}
		}
	}

	void HLC::regressCovLine(size_t id_basin) {
		std::vector<Real> iters(m_info_bsns_atrct.at(id_basin).cuml_covs.size());
		std::iota(iters.begin(), iters.end(), 1);
		ParamMap param;

		param["problem name"] = std::string("logarithmic regression for SPAE");
		int id_param_pro = ADD_PARAM(param);
		std::unique_ptr<Problem> pro(new RegressLog(iters, m_info_bsns_atrct.at(id_basin).cuml_covs));
		int id_pro = ADD_PRO_EXST(id_param_pro, pro, 0.5);
		GET_PRO(id_pro).initialize();

		param.clear();
		param["algorithm name"] = std::string("DE/rand/1");
		param["mutation strategy"] = static_cast<int>(MutationDE::rand_1);
		param["population size"] = static_cast<int>(100);
		param["maximum evaluations"] = static_cast<int>(10000);
		int id_param_alg = ADD_PARAM(param);
		std::unique_ptr<Algorithm> alg(new Canonical_DE);
		int id_alg = ADD_ALG_EXST(id_param_alg, alg, id_pro, 0.5, -1);
		GET_ALG(id_alg).initialize();
		GET_ALG(id_alg).setKeepCandidatesUpdated(true);

		GET_ALG(id_alg).run();
		auto best = dynamic_cast<Solution<>*>(GET_ALG(id_alg).candidates().front().get());
		m_info_bsns_atrct.at(id_basin).log_param[0] = best->variable()[0];
		m_info_bsns_atrct.at(id_basin).log_param[1] = best->variable()[1];
		DEL_ALG(id_alg);
		DEL_PRO(id_pro);
		DEL_PARAM(id_param_alg);
		DEL_PARAM(id_param_pro);
	}

	size_t HLC::clustering() {
//		updateDistance();
//#ifdef OFEC_DEMO
//		if (Demo::g_term_alg) return 0;
//#endif
//		std::multimap<Real, size_t> sort_fitness;
//		std::vector<std::list<size_t>> affiliated_clu(m_info_subspaces.size());
//		std::list<std::set<size_t>> clusters;
//		for (size_t i = 0; i < m_info_subspaces.size(); ++i)
//			sort_fitness.emplace(std::make_pair(m_info_subspaces[i].stats[0], i));
//		size_t idx_cluster = 0;
//		if (m_opt_mode_bsn_atrct[0] == OptMode::Minimize) {
//			for (auto i = sort_fitness.begin(); i != sort_fitness.end(); ++i) {
//				if (affiliated_clu[i->second].empty()) {
//					std::set<size_t> cluster;
//					cluster.emplace(i->second);
//					affiliated_clu[i->second].push_back(idx_cluster);
//					findWorseNeighbor(i->second, cluster, affiliated_clu, idx_cluster);
//					clusters.emplace_back(cluster);
//					idx_cluster++;
//				}
//			}
//		}
//		else {
//			for (auto i = sort_fitness.rbegin(); i != sort_fitness.rend(); ++i) {
//				if (affiliated_clu[i->second].empty()) {
//					std::set<size_t> cluster;
//					cluster.emplace(i->second);
//					affiliated_clu[i->second].push_back(idx_cluster);
//					findWorseNeighbor(i->second, cluster, affiliated_clu, idx_cluster);
//					clusters.emplace_back(cluster);
//					idx_cluster++;
//				}
//			}
//		}
//		for (size_t i = 0; i < m_info_subspaces.size(); ++i)
//			m_info_subspaces[i].id_bsn_atr  = -1;
//		m_info_bsns_atrct.clear();
//		m_info_bsns_atrct.resize(clusters.size());
//		size_t id_bsn_atr = 0;
//		for (auto &cluster : clusters) {
//			m_info_bsns_atrct[id_bsn_atr].volume = 0;
//			for (auto i : cluster) {
//				if (affiliated_clu[i].size() == 1) {
//					m_info_subspaces[i].id_bsn_atr = affiliated_clu[i].front();
//					m_info_bsns_atrct[id_bsn_atr].subspaces.push_back(i);
//					m_info_bsns_atrct[id_bsn_atr].volume += m_subspace_tree->getBoxVolume(i);
//				}
//			}
//			id_bsn_atr++;
//		}
//		for (size_t i = 0; i < m_info_subspaces.size(); ++i)
//			if (affiliated_clu[i].size() > 1)
//				m_info_subspaces[i].id_bsn_atr = idxNearestCluster(i, affiliated_clu);
//		for (size_t i = 0; i < m_info_subspaces.size(); ++i) {
//			if (affiliated_clu[i].size() > 1) {
//				m_info_bsns_atrct[m_info_subspaces[i].id_bsn_atr].subspaces.push_back(i);
//				m_info_bsns_atrct[m_info_subspaces[i].id_bsn_atr].volume += m_subspace_tree->getBoxVolume(i);
//			}
//		}

		/* merge adjacent subspaces of equal fitness */
		std::vector<size_t> belonged(m_info_subspaces.size());
		std::vector<std::set<size_t>> neighbors(m_info_subspaces.size());
		std::vector<bool> visited(m_info_subspaces.size(), false);
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (!visited[i]) {
				belonged[i] = i;
				for (size_t nn : m_info_subspaces[i].neighbors)
					neighbors[i].insert(nn);
				std::list<int> neis;
				neis.push_back(i);
				visited[i] = true;
				while (!neis.empty()) {
					int cur_id = neis.front();
					neis.pop_front();
					for (size_t n : m_info_subspaces[cur_id].neighbors) {
						if (m_info_subspaces[cur_id].stats[0] == m_info_subspaces[n].stats[0] && !visited[n]) {
							belonged[n] = i;
							for (size_t nn : m_info_subspaces[n].neighbors)
								neighbors[i].insert(nn);
							neis.push_back(n);
							visited[n] = true;
						}
					}
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			neighbors[belonged[i]].erase(i);
		}


		std::vector<size_t> best_neighbor(m_info_subspaces.size());

		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] != i) continue;
			best_neighbor[i] = i;
			for (size_t n : neighbors[i]) {
				if ((m_opt_mode_bsn_atrct[0] == OptMode::Minimize && m_info_subspaces[best_neighbor[i]].stats[0] > m_info_subspaces[n].stats[0])
					|| (m_opt_mode_bsn_atrct[0] == OptMode::Maximize && m_info_subspaces[best_neighbor[i]].stats[0] < m_info_subspaces[n].stats[0]))
					best_neighbor[i] = belonged[n];
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] == i) {
				while (best_neighbor[best_neighbor[i]] != best_neighbor[i]) {
					best_neighbor[i] = best_neighbor[best_neighbor[i]];
				}
			}
		}
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (belonged[i] != i)
				best_neighbor[i] = best_neighbor[belonged[i]];
		}
		size_t id_bsn = 0;
		std::unordered_map<size_t, size_t> center_to_id_bsn;
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			if (center_to_id_bsn.count(best_neighbor[i]) == 0) {
				center_to_id_bsn[best_neighbor[i]] = id_bsn++;
			}
		}
		m_info_bsns_atrct.clear();
		m_info_bsns_atrct.resize(center_to_id_bsn.size());
		for (size_t i = 0; i < m_info_subspaces.size(); i++) {
			m_info_subspaces[i].id_bsn_atr = center_to_id_bsn[best_neighbor[belonged[i]]];
			m_info_bsns_atrct[m_info_subspaces[i].id_bsn_atr].subspaces.push_back(i);
			m_info_bsns_atrct[m_info_subspaces[i].id_bsn_atr].volume += m_subspace_tree->getBoxVolume(i);
		}
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++)
			divideBsnAtrct(i);
		return m_info_bsns_atrct.size();
	}

	void HLC::updatePotential() {
		std::vector<std::vector<Real>> stat_basn(m_info_bsns_atrct.size(), std::vector<Real>(2));
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			size_t ssp_best = m_info_bsns_atrct[i].subspaces.front();
			stat_basn[i][1] = 0;
			for (size_t ssp : m_info_bsns_atrct[i].subspaces) {
				stat_basn[i][1] += m_info_subspaces[ssp].stats[1];
				if (!m_info_subspaces[ssp_best].best_sol || (m_info_subspaces[ssp].best_sol && m_info_subspaces[ssp].best_sol->dominate(*m_info_subspaces[ssp_best].best_sol, m_opt_mode_sol)))
					ssp_best = ssp;
			}
			stat_basn[i][0] = m_info_subspaces[ssp_best].stats[0];
		}
		std::vector<std::vector<Real> *> data(stat_basn.size());
		for (size_t i = 0; i < stat_basn.size(); i++)
			data[i] = &stat_basn[i];
		std::vector<int> rank;
		int num_rank = NS::fast_sort<Real>(data, rank, m_opt_mode_bsn_atrct);
		for (size_t i = 0; i < m_info_bsns_atrct.size(); i++) {
			m_info_bsns_atrct[i].potential = 1. - Real(rank[i]) / num_rank;
		}
	}

	void HLC::splitBasin(size_t id_basin, size_t num_parts) {
		auto &basin = m_info_bsns_atrct.at(id_basin).subspaces;
		size_t cur_size = basin.size();
		while (basin.size() < num_parts) {
			size_t largest = basin.front();
			for (size_t id_subspace : basin) {
				if (m_subspace_tree->getBoxVolume(id_subspace) > m_subspace_tree->getBoxVolume(largest))
					largest = id_subspace;
			}
			size_t added = m_subspace_tree->splitRegion(largest);
			m_info_subspaces.resize(m_info_subspaces.size() + 1);
			auto sols = m_info_subspaces[largest].sols;
			resetSubspaceInfo(largest);
			resetSubspaceInfo(added);
			for (auto &ptr_sol : sols) {
				updateSubspaceInfo(ptr_sol);
			}
			basin.push_back(added);	
			m_info_subspaces[added].neighbors.clear();
			for (auto iter = m_info_subspaces[largest].neighbors.begin(); iter != m_info_subspaces[largest].neighbors.end();) {
				if (isAdjacent(m_subspace_tree->getBox(added), m_subspace_tree->getBox(*iter))) {
					m_info_subspaces[added].neighbors.push_back(*iter);
					m_info_subspaces[*iter].neighbors.push_back(added);

				}
				if (!isAdjacent(m_subspace_tree->getBox(largest), m_subspace_tree->getBox(*iter))) {
					m_info_subspaces[*iter].neighbors.erase(std::find(m_info_subspaces[*iter].neighbors.begin(), m_info_subspaces[*iter].neighbors.end(), largest));
					iter = m_info_subspaces[largest].neighbors.erase(iter);
				}
				else
					iter++;
			}
			m_info_subspaces[largest].neighbors.push_back(added);
			m_info_subspaces[added].neighbors.push_back(largest);
		}
	}

	void HLC::calCoverage(size_t id_basin, const std::vector<const Solution<>*> &sub_pop) {
		std::unordered_set<size_t> filled_divs;
		for (auto ptr_sol : sub_pop) {
			size_t aff_ssp = m_subspace_tree->getRegionIdx(ptr_sol->variable().vect());
			int aff_asp = -1;
			if (m_info_subspaces[aff_ssp].atomspace_tree) {
				aff_asp = m_info_subspaces[aff_ssp].atomspace_tree->getRegionIdx(ptr_sol->variable().vect());
			}
			size_t aff_div = m_info_bsns_atrct[id_basin].ssp_atp_to_div[aff_ssp][aff_asp];
			filled_divs.insert(aff_div);
		}
		Real filled_vol = 0;
		for (size_t i = 0; i < m_num_divs; i++) {
			if (filled_divs.count(i) > 0)
				filled_vol += m_info_bsns_atrct[id_basin].vols_divs[i];
		}
		m_info_bsns_atrct[id_basin].coverages.push_back(filled_vol / m_info_bsns_atrct[id_basin].volume);
		if (m_info_bsns_atrct[id_basin].cuml_covs.empty())
			m_info_bsns_atrct[id_basin].cuml_covs.push_back(m_info_bsns_atrct[id_basin].coverages.back());
		else
			m_info_bsns_atrct[id_basin].cuml_covs.push_back(m_info_bsns_atrct[id_basin].cuml_covs.back() + m_info_bsns_atrct[id_basin].coverages.back());
	}

	void HLC::randomSmpl(size_t id_bsn_atrct, size_t id_div, size_t id_rnd, std::vector<Real> &smpl) {
		auto &division = m_info_bsns_atrct[id_bsn_atrct].div_to_ssp_atp[id_div];
		auto rnd_div = GET_RND(id_rnd).uniform.nextElem(division.begin(), division.end());
		auto box(m_subspace_tree->getBox(rnd_div->first));
		if (rnd_div->second > -1)
			box = m_info_subspaces[rnd_div->first].atomspace_tree->getBox(rnd_div->second);
		for (size_t j = 0; j < m_size_var; j++) 
			smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	void HLC::randomSmpl(size_t id_subspace, size_t id_rnd, std::vector<Real> &smpl) {
		auto &box(m_subspace_tree->getBox(id_subspace));
		for (size_t j = 0; j < m_size_var; j++)
			smpl[j] = GET_RND(id_rnd).uniform.nextNonStd(box[j].first, box[j].second);
	}

	void HLC::calRuggedness(size_t id_bsn_atrct, int id_rnd, int id_pro) {
		auto &info = m_info_bsns_atrct[id_bsn_atrct];
		size_t id = 0;
		auto worst_sol = m_info_subspaces[info.subspaces[id]].worst_sol;
		auto best_sol = m_info_subspaces[info.subspaces[id++]].best_sol;
		while (!worst_sol) {
			worst_sol = m_info_subspaces[info.subspaces[id]].worst_sol;
			best_sol = m_info_subspaces[info.subspaces[id++]].best_sol;
		}
		while (id < info.subspaces.size()) {
			if (!m_info_subspaces[info.subspaces[id]].sols.empty()) {
				if (worst_sol->dominate(*m_info_subspaces[info.subspaces[id]].worst_sol, m_opt_mode_sol))
					worst_sol = m_info_subspaces[info.subspaces[id]].worst_sol;
				if (m_info_subspaces[info.subspaces[id]].best_sol->dominate(*best_sol, m_opt_mode_sol))
					best_sol = m_info_subspaces[info.subspaces[id]].best_sol;
			}
			id++;
		}
		Real max_obj_dif = best_sol->objectiveDistance(*worst_sol);
		info.ruggedness = 0;
		for (size_t id_ssp : info.subspaces) {
			Solution<> tmp_sol(*m_info_subspaces[id_ssp].best_sol);
			auto &box = m_subspace_tree->getBox(id_ssp);
			for (size_t j = 0; j < m_size_var; ++j)	{
				tmp_sol.variable()[j] = GET_RND(id_rnd).normal.nextNonStd(tmp_sol.variable()[j], (box[j].second - box[j].first) / 1000);
			}
			tmp_sol.evaluate(id_pro, -1);
			info.ruggedness += tmp_sol.objectiveDistance(*m_info_subspaces[id_ssp].best_sol) / max_obj_dif;
		}
		info.ruggedness /= info.subspaces.size();
		//auto &bsn_info = m_info_bsns_atrct[id_bsn_atrct];
		//bsn_info.ruggedness = log(base) / log(bsn_info.log_param[1]);
	}

	//void HLC::calRuggedness(size_t id_bsn_atrct, const std::vector<const Solution<>*> &sub_pop) {
	//	std::vector<std::vector<Real>> data(sub_pop.size());
	//	for (size_t i = 0; i < sub_pop.size(); i++)
	//		data[i] = sub_pop[i]->variable().vect();
	//	size_t num_nghbrs = 3;
	//	std::vector<size_t> nghbrs(num_nghbrs + 1);
	//	std::vector<Real> distns(num_nghbrs + 1);
	//	KDTreeVectorOfVectorsAdaptor<std::vector<std::vector<Real>>, Real> kd_tree(m_size_var, data, 2);
	//	m_info_bsns_atrct[id_bsn_atrct].ruggedness = 0;
	//	for (size_t i = 0; i < sub_pop.size(); i++) {
	//		kd_tree.index->knnSearch(data[i].data(), num_nghbrs + 1, nghbrs.data(), distns.data());
	//		Real dev = 0;
	//		for (size_t k = 0; k < num_nghbrs; k++) {
	//			dev += sub_pop[nghbrs[k + 1]]->objectiveDistance(*sub_pop[i]) / distns[k + 1];
	//		}
	//		dev /= num_nghbrs;
	//		m_info_bsns_atrct[id_bsn_atrct].ruggedness += dev;
	//	}
	//	m_info_bsns_atrct[id_bsn_atrct].ruggedness /= sub_pop.size();
	//}

	void HLC::resetSubspaceInfo(size_t id_subspace) {
		m_info_subspaces[id_subspace].stats = { 0,0 };
		m_info_subspaces[id_subspace].sols.clear();
		m_info_subspaces[id_subspace].best_sol.reset();
		m_info_subspaces[id_subspace].worst_sol.reset();
	}

	//size_t HLC::cluster_in_attractor(const std::vector<size_t>& clu) {
	//	std::vector<std::vector<std::pair<Real,Real>>> atom_spaces;
	//	std::vector<Real> atom_fitness;
	//	std::multimap<Real, size_t> sort_fitness;
	//	for (size_t idx_reg : clu) {
	//		for (size_t id_atom = 0; id_atom < m_info_subspaces[idx_reg].hlc->num_regions(); ++id_atom) {
	//			atom_fitness.push_back(m_info_subspaces[idx_reg].hlc->get_stat(id_atom)[0]);
	//			sort_fitness.emplace(std::make_pair(m_info_subspaces[idx_reg].hlc->get_stat(id_atom)[0], atom_spaces.size()));
	//			atom_spaces.push_back(m_info_subspaces[idx_reg].hlc->getBox(id_atom));
	//		}
	//	}
	//	std::vector<std::list<size_t>> neighbors(atom_spaces.size());
	//	for (size_t i = 0; i < atom_spaces.size(); i++) {
	//		for (size_t j = i + 1; j < atom_spaces.size(); j++) {
	//			if (isAdjacent(atom_spaces[i], atom_spaces[j])) {
	//				neighbors[i].push_back(j);
	//				neighbors[j].push_back(i);
	//			}
	//		}
	//	}
	//	std::vector<bool> clustered(atom_spaces.size(), false);
	//	std::list<std::set<size_t>> clusters;
	//	if (CONTINUOUS_CAST->optMode(0) == optimization_mode::Minimize) {
	//		for (auto i = sort_fitness.begin(); i != sort_fitness.end(); ++i) {
	//			if (!clustered[i->second]) {
	//				std::set<size_t> cluster;
	//				cluster.emplace(i->second);
	//				clustered[i->second] = true;
	//				get_less_fitter_atom(i->second, cluster, atom_fitness, neighbors, clustered);
	//				clusters.emplace_back(cluster);
	//			}
	//		}
	//	}
	//	else {
	//		for (auto i = sort_fitness.rbegin(); i != sort_fitness.rend(); ++i) {
	//			if (!clustered[i->second]) {
	//				std::set<size_t> cluster;
	//				cluster.emplace(i->second);
	//				clustered[i->second]=true;
	//				get_less_fitter_atom(i->second, cluster, atom_fitness, neighbors, clustered);
	//				clusters.emplace_back(cluster);
	//			}
	//		}
	//	}
	//	return clusters.size();
	//}

	//void HLC::findWorseNeighbor(size_t center, std::set<size_t>& cluster, size_t id_clu, std::vector<int>& aff_clu, size_t step, std::vector<size_t>& min_step) {
	//	for (auto i : m_neighbor[center]) {
	//		if ((CONTINUOUS_CAST->optMode(0) == optimization_mode::Minimize && m_info_subspaces[i].stats[0] >= m_info_subspaces[center].stats[0])
	//			|| (CONTINUOUS_CAST->optMode(0) == optimization_mode::Maximize && m_info_subspaces[i].stats[0] <= m_info_subspaces[center].stats[0])) {
	//			if (cluster.find(i) == cluster.end()) {
	//				cluster.emplace(i);
	//				if (min_step[i] > step) {
	//					aff_clu[i] = id_clu;
	//					min_step[i] = step;
	//				}
	//				findWorseNeighbor(i, cluster, id_clu, aff_clu, step+1, min_step);
	//			}
	//		}
	//	}
	//}

	void HLC::findWorseNeighbor(size_t center, std::set<size_t> &cluster, std::vector<std::list<size_t>> &affiliated_clu, size_t idx_cluster) {
		for (auto i : m_info_subspaces[center].neighbors) {
			if ((m_opt_mode_bsn_atrct[0] == OptMode::Minimize && m_info_subspaces[i].stats[0] >= m_info_subspaces[center].stats[0])
				|| (m_opt_mode_bsn_atrct[0] == OptMode::Maximize && m_info_subspaces[i].stats[0] <= m_info_subspaces[center].stats[0])) {
				if (cluster.find(i) == cluster.end()) {
					cluster.emplace(i);
					affiliated_clu[i].push_back(idx_cluster);
					findWorseNeighbor(i, cluster, affiliated_clu, idx_cluster);
				}
			}
		}
	}

	int HLC::idxNearestCluster(size_t idx_region, std::vector<std::list<size_t>> &affiliated_clu) const {
		Real min_val = std::numeric_limits<Real>::max();
		int idx_cluster = -1;
		for (size_t cluster : affiliated_clu[idx_region]) {
			for (size_t i : m_info_bsns_atrct[cluster].subspaces) {
				if (m_dis_mat[i][idx_region] < min_val) {
					idx_cluster = m_info_subspaces.at(i).id_bsn_atr;
					min_val = m_dis_mat[i][idx_region];
				}
			}
		}
		return idx_cluster;
	}

	void HLC::updateDistance() {
		Real temp;
		m_dis_mat.assign(m_info_subspaces.size(), std::vector<Real>(m_info_subspaces.size()));
		for (size_t i = 0; i < m_info_subspaces.size(); ++i) {
			m_dis_mat[i][i] = 0;
			for (size_t j = i + 1; j < m_info_subspaces.size(); ++j) {
#ifdef OFEC_DEMO
				if (Demo::g_term_alg) return;
#endif
				temp = 0;
				auto &box1 = m_subspace_tree->getBox(i);
				auto &box2 = m_subspace_tree->getBox(j);
				for (size_t k = 0; k < m_size_var; ++k) {
					temp += pow((box1[k].first + box1[k].second - box2[k].first - box2[k].second) / 2, 2);
				}
				m_dis_mat[i][j] = m_dis_mat[j][i] = sqrt(temp);
			}
		}
	}


	//void HLC::get_less_fitter_atom(size_t center, std::set<size_t>& cluster, const std::vector<Real>& fitness, const std::vector<std::list<size_t>>& neighbors, std::vector<bool>& clustered) {
	//	for (auto i : neighbors[center]) {
	//		if ((CONTINUOUS_CAST->optMode(0) == optimization_mode::Minimize && fitness[i] >= fitness[center])
	//			|| (CONTINUOUS_CAST->optMode(0) == optimization_mode::Maximize && fitness[i] <= fitness[center])) {
	//			if (cluster.find(i) == cluster.end() && !clustered[i]) {
	//				cluster.emplace(i);
	//				clustered[i] = true;
	//				get_less_fitter_atom(i, cluster, fitness, neighbors, clustered);
	//			}
	//		}
	//	}
	//}
}

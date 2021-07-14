/* History Learning in Continuous Space */

#ifndef OFEC_HLC_H
#define OFEC_HLC_H

#include "../../../../../utility/kd-tree/kdtree_space.h"
#include "../../../../../core/algorithm/solution.h"
#include <memory>
#include <list>
#include <array>
#include <unordered_set>

namespace OFEC {
	class HLC {
	public:
		using SplitTree = KDTreeSpace::PartitioningKDTree<Real>;

		struct InfoSubspace {
			std::list<size_t> neighbors;
			std::array<Real, 2> stats = { 0,0 };  // [0]: objective;  [1]: num sample;
			std::list<std::shared_ptr<const Solution<>>> sols;
			std::shared_ptr<const Solution<>> best_sol, worst_sol;
			int id_bsn_atr = -1;
			std::unique_ptr<SplitTree> atomspace_tree;
		};

		struct InfoBsnAtrct {
			std::vector<size_t> subspaces;
			std::vector<Real> coverages;           // Coverages of iterations
			std::vector<Real> cuml_covs;           // Cumulative coverages of iterations
			std::array<Real, 2> log_param;         // Logarithmic parameters of the cumulative coverage line 
			Real potential;                        // Normalized potential 
			Real volume;
			Real ruggedness;
			
			std::map<size_t, std::map<int, size_t>> ssp_atp_to_div;
			std::vector<std::list<std::pair<size_t, int>>> div_to_ssp_atp;

			std::vector<Real> vols_divs;             // Volumes of subspaces						
		};

	private:
		size_t m_size_var;
		std::vector<OptMode> m_opt_mode_sol;
		std::vector<OptMode> m_opt_mode_bsn_atrct;
		std::unique_ptr<SplitTree> m_subspace_tree;
		std::list<std::shared_ptr<const Solution<>>> m_his_inds;
		std::vector<InfoSubspace> m_info_subspaces;
		std::vector<InfoBsnAtrct> m_info_bsns_atrct;
		std::shared_ptr<const Solution<>> m_worst_ind;
		std::unordered_set<size_t> m_interpolated;
		size_t m_num_divs;

#ifdef OFEC_DEMO  
		bool m_log_param_ready = false;
#endif

		std::vector<std::vector<Real>> m_dis_mat;

	public:
		HLC(size_t size_var, size_t num_divs);
		void initialize(const std::vector<std::pair<Real, Real>> &boundary, OptMode opt_mode, int init_num);
		void inputSample(const Solution<> &sample_point);
		void regressCovLines();
		void interpolateMissing();
		size_t clustering();
		void updatePotential();
		void splitBasin(size_t id_bsn_atrct, size_t num_subspaces);
		void calCoverage(size_t id_bsn_atrct, const std::vector<const Solution<>*> &sub_pop);
		void randomSmpl(size_t id_bsn_atrct, size_t id_div, size_t id_rnd, std::vector<Real>& smpl);
		void randomSmpl(size_t id_subspace, size_t id_rnd, std::vector<Real> &smpl);
		void calRuggedness(size_t id_bsn_atrct, int id_rnd, int id_pro);

		const std::list<std::shared_ptr<const Solution<>>> &hisInds() const { return m_his_inds; }
		const SplitTree &subspaceTree() const { return *m_subspace_tree; }
		size_t numSubspaces() const { return m_info_subspaces.size(); }
		const InfoSubspace &infoSubspace(size_t id_subspace) const { return m_info_subspaces.at(id_subspace); }
		size_t numBsnsAtrct() const { return m_info_bsns_atrct.size(); }
		const InfoBsnAtrct &infoBsnAtrct(size_t id_bsn_atrct) const { return m_info_bsns_atrct.at(id_bsn_atrct); }
		const std::unordered_set<size_t> interpolated() const { return m_interpolated; }

#ifdef OFEC_DEMO  
		bool isLogParamReady()const { return m_log_param_ready; }
		void setLogParamReady(bool flag) { m_log_param_ready = flag; }
#endif

	private:
		void divideBsnAtrct(size_t id_bsn_atrct);
		void updateSubspaceInfo(const std::shared_ptr<const Solution<>> &ptr_sol);
		void resetSubspaceInfo(size_t id_subspace);
		bool isAdjacent(const std::vector<std::pair<Real, Real>> &box1, const std::vector<std::pair<Real, Real>> &box2);
		int idxNearestCluster(size_t idx_region, std::vector<std::list<size_t>> &affiliated_clu) const;
		void updateDistance();
		void findWorseNeighbor(size_t center, std::set<size_t> &cluster, std::vector<std::list<size_t>> &affiliated_clu, size_t idx_cluster);
		void regressCovLine(size_t id_basin);

		//void findWorseNeighbor(size_t center, std::set<size_t>& cluster, size_t id_clu, std::vector<int>& aff_clu, size_t step, std::vector<size_t>& min_step);
		//void get_less_fitter_atom(size_t center, std::set<size_t>& cluster, const std::vector<Real> &fitness, const std::vector<std::list<size_t>>& neighbors, std::vector<bool>& clustered);
	};
}

#endif // !OFEC_HLC_H

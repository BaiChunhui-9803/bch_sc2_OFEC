//Register SPAE_GA "HLC-GA" ConOP,SOP,GOP,MMOP
/* DE-HL (Differential Evolution with History Learning) */

#ifndef OFEC_SPAE_GA_H
#define OFEC_SPAE_GA_H

#include "../../../../core/algorithm/algorithm.h"
#include "../SBX/SBX.h"
#include "../../HLC/HLC.h"
#include "../../../../core/algorithm/multi_population.h"

namespace OFEC {
	class SPAE_GA : public algorithm {
		using ind_type = individual<>;
		using pop_type = SBX_pop<>;
		using iter_subpop = typename multi_population<pop_type>::iterator_type;
	public:
		SPAE_GA(param_map& v);
		SPAE_GA(const std::string& algName, size_t subPopSize, size_t numPartition);
		void initialize() override;
		void record() override;
		HLC& get_hlc() { return m_hlc; }
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif
	protected:
		void run_() override;
	private:
		void update_accu_pro_clu();
		bool is_converged(pop_type& subpop);
		void exploit(pop_type& subpop);
		void explore(pop_type& subpop);
		void init_in_cluster(const std::vector<size_t>& cluster, ind_type& ind);
		void remove_outside_inds();
		void reallocate_explore_inds();
		void merge_overlapped_pops();

		multi_population<pop_type> m_subpops;
		size_t m_subpop_size, m_num_segs;
		HLC m_hlc;
		std::vector<Real> m_accu_pro_clu;
		std::list<ind_type> m_inds_explore;
	};
}


#endif // !OFEC_SPAE_GA_H


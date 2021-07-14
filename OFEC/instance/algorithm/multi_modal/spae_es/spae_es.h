//Register SPAE_ES "SPAE-ES" GOP,MMOP,ConOP,SOP

#ifndef OFEC_HLC_ES_H
#define OFEC_HLC_ES_H

#include "../CMA_ES/CMA_ES.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../HLC/HLC.h"

namespace OFEC {
	class SPAE_ES : public algorithm {
		using pop_type = cmaes_pop;
		using ind_type = individual<>;
		using iter_subpop = typename multi_population<pop_type>::iterator_type;
	public:
		SPAE_ES(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif
	protected:
		void run_() override;

	private:
		void update_accu_pro_clu();
		bool is_converged(pop_type& subpop);
		void exploit(pop_type& subpop);
		void explore(iter_subpop& iter_subpop);
		void init_in_cluster(const std::vector<size_t>& cluster, ind_type& ind);

		multi_population<pop_type> m_subpops;
		size_t m_subpop_size, m_num_segs;
		HLC m_hlc;
		std::vector<Real> m_accu_pro_clu;
	};
}

#endif // !OFEC_HLC_ES_H


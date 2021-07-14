//Register SPAE_DE "SPAE-DE" ConOP,SOP,GOP,MMOP

#ifndef OFEC_SPAE_DE_H
#define OFEC_SPAE_DE_H

#include "../../template/framework/spae/spae_cont.h"
#include "../../template/classic/de/de_pop.h"

namespace OFEC {
	class SPAE_DE : public ContSPAE<PopDE<>> {
	public:
		SPAE_DE() = default;
	protected:
		Real m_f, m_cr;
		MutationDE m_ms;

		void initialize_() override;
		void setPopType() override;
		//bool isPopConverged(PopDE<> &pop) override;
		void initSubPop(PopDE<> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg) override;
		void evolveSubPop(PopDE<> &pop) override;
	};
}

#endif //!OFEC_SPAE_DE_H
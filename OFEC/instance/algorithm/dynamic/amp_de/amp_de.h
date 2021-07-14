//Register AMP_DE "AMP-DE" DOP,ConOP,SOP,GOP,MMOP

#include "../../template/framework/amp/continuous/amp_cont_ind.h"
#include "../../template/framework/amp/continuous/amp_cont.h"
#include "../../../../core/algorithm/algorithm.h"
#include "../../template/classic/de/de_pop.h"

namespace OFEC {
	class AMP_DE : public Algorithm {
	protected:
		using PopType = PopDE<IndContAMP<IndDE>>;
		std::unique_ptr<ContAMP<PopType>> m_multi_pop;
		size_t m_pop_size;
		Real m_f, m_cr;
		MutationDE m_ms;
	public:
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif
	protected:
		void initialize_() override;
		void run_() override;
		void initMultiPop();
	};
}
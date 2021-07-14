#ifndef OFEC_MMOP_METRICS_H
#define OFEC_MMOP_METRICS_H

#include "../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class MetricsMMOP : virtual public Continuous {
	public:
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const override;
	protected:
		void updateOnlyByObj(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const;
		void updateByObjAndVar(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const;
		void updateBestFixedNumSols(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates, size_t num_sols) const;

		size_t numOptimaOnlyByObj(const std::list<std::unique_ptr<SolBase>> &candidates) const;
		size_t numOptimaByObjAndVar(const std::list<std::unique_ptr<SolBase>> &candidates) const;
	};
}

#endif // !OFEC_MMOP_METRICS_H

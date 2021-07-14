//Register simulated_annealing "SA" SOP,ConOP,GOP

#ifndef OFEC_SIMULATED_ANNEALING_H
#define OFEC_SIMULATED_ANNEALING_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/solution.h"

namespace OFEC {
	class simulated_annealing : public algorithm {
	public:
		simulated_annealing(param_map& v);
		simulated_annealing(size_t k_max);
		void initialize();
		void record();
#ifdef OFEC_DEMO
		void updateBuffer();
#endif
	protected:
		void run_();
		virtual Real temperature(Real val);
		virtual void neighbour(const Solution<>& s, Solution<>& s_new);
		virtual Real P(const Solution<>& s, const Solution<>& s_new, Real T);
	protected:
		size_t m_k_max;
		std::unique_ptr<Solution<>> m_s;
		std::unique_ptr<Solution<>> m_s_new;
	};
}

#endif // !OFEC_SIMULATED_ANNEALING_H


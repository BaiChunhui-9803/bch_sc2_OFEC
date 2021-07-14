//Register p_fGA "p-fGA" ConOP,SOP,GOP,MMOP

#ifndef OFEC_P_FGA_H
#define OFEC_P_FGA_H

#include "../SBX/SBX.h"
#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../DE/population.h"

namespace OFEC {
	class p_fGA : public algorithm {
		using type_pop = SBX_pop<>;
		//using type_pop = typename DE::population<>;
	public:
		p_fGA(param_map& v);
		p_fGA(const std::string& algName, size_t popSize);
		void initialize() override;
		void record() override;
		std::vector<Solution<>>& get_attractors() { return m_attractors; }
		std::vector<Real>& get_S() { return m_S; }
#ifdef OFEC_DEMO
		void updateBuffer() override;
		bool m_update_buffer = false;
#endif
	protected:
		void run_() override;
	private:
		EvalTag evolve_pop(type_pop& pop, bool is_main_pop);
		bool forking_condition(std::vector<size_t>& inds_to_fork);
		void fork_pop(const std::vector<size_t>& inds_to_fork);
		bool in_neighbor(const Solution<>& best, const Solution<>& ind);
		void init_in_neighbor(const Solution<>& best, Solution<>& ind);
		void block_ind(Solution<>& ind);
		bool in_blocks(const Solution<>& ind);
		void update_S();
		bool overlapped(const type_pop& pop);
		type_pop m_pop;
		multi_population<type_pop> m_child_pops;
		std::vector<Solution<>> m_attractors;
		size_t m_K_H;
		size_t m_num_stagnate;
		std::vector<Real> m_S;
		Real m_K_R;
		Real m_previous_best;
	};
}

#endif // !OFEC_P_FGA_H


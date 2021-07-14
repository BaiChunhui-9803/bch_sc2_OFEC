//Register DCNSGAII_DE "DCNSGAII-DE" ConOP,COP,MOP,SOP

#ifndef OFEC_DCNSGAII_DE_H
#define OFEC_DCNSGAII_DE_H

#include "../DCMOEA.h"
#include "../../DE/individual.h"

namespace OFEC {
	class DCNSGAII_DE_pop : public population<DCMOEA_ind<DE::individual>>, DCMOEA<DCMOEA_ind<DE::individual>> {
	public:
		using typename population<DCMOEA_ind<DE::individual>>::individual_type;
	public:
		DCNSGAII_DE_pop(size_t size_pop);
		EvalTag evolve();
		void initialize();
		void sort();
	protected:
		void select_next_parent_population(std::vector<individual_type*>& pop);
		void reduce_radius();
		void caculate_nichecount(std::vector<individual_type*>& pop);
		dominationship e_Pareto_compare(individual_type* const&s1, individual_type* const&s2); 
		dominationship Pareto_compare(individual_type* const&s1, individual_type* const&s2);
	protected:
		Real m_r = 0, m_R, m_CR, m_F;
		std::vector<std::unique_ptr<individual_type>> m_offspring;
		std::vector<std::unique_ptr<individual_type>> m_temp_pop;
		DE::recombine_strategy m_recombine_strategy;
		std::vector<int> m_rand_seq;
		bool m_flag = false;
	};

	class DCNSGAII_DE : public algorithm {
	protected:
		DCNSGAII_DE_pop m_pop;
	public:
		DCNSGAII_DE(param_map& v);
		void initialize();
		void run_();
		void record();
	};
}



#endif // !OFEC_DCNSGAII_DE_H

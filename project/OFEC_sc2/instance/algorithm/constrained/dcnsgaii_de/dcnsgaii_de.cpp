#include "DCNSGAII_DE.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../utility/nondominated_sorting/fast_sort.h"
#include <numeric>

using namespace std::placeholders;

namespace OFEC {
	DCNSGAII_DE_pop::DCNSGAII_DE_pop(size_t size_pop) : population<DCMOEA_ind<DE::individual>>(size_pop, CONTINUOUS_CAST->variable_size()), m_rand_seq(size_pop) {
		for (auto& i : m_inds) {
			i->resize_vio_obj(2);
			m_offspring.emplace_back(new individual_type(*i));
			m_temp_pop.emplace_back(new individual_type(*i));
		}
		std::iota(m_rand_seq.begin(), m_rand_seq.end(), 0);
		m_F = 0.5;
		m_CR = 0.6;
		m_recombine_strategy = DE::recombine_strategy::binomial;
		CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void DCNSGAII_DE_pop::initialize() {
		population::initialize();
		evaluate();
		calculate_initial_max_violation(m_inds);
		m_e = m_max_G;
		calculate_violation_objective(m_inds);
		mark_individual_efeasible(m_inds);
		Real dimension = CONTINUOUS_CAST->variable_size();
		m_R = 0.5 * pow(2 * dimension / (2 * m_inds.size()  * OFEC_PI), 1.0 / dimension);

	}

	void DCNSGAII_DE_pop::sort() {
		std::vector<int> ranks;
		std::function<OFEC::dominationship(individual_type* const&, individual_type* const&)> comp = std::bind(&DCNSGAII_DE_pop::Pareto_compare, this, _1, _2);
		std::vector<individual_type*> pop;
		for (auto& i : m_inds)
			pop.emplace_back(i.get());
		NS::fast_sort<individual_type*>(pop, ranks, comp);
		/*for (size_t i = 0; i < pop.size(); i++) {
			if (ranks[i] == 0) {
				std::cout << "pop[" << i << "]: ";
				for (size_t j = 0; j < CONTINUOUS_CAST->objective_size(); ++j)
					std::cout << pop[i]->objective(j) << "\t\t";
				std::cout << pop[i]->get_vio_obj()[0] << "\t\t" << pop[i]->get_vio_obj()[1] <<std::endl;
			}
		}*/
	}

	EvalTag DCNSGAII_DE_pop::evolve()
	{
		EvalTag tag = EvalTag::Normal;


		if (judge_population_efeasible(m_inds)) {
			m_k++;
			if (m_k > m_max_K + 1) {
				return EvalTag::Terminate;
				//m_flag = true;
			}	
			reduce_boundary();
			reduce_radius();
			mark_individual_efeasible(m_inds);
		}

		//generate offspring pop
		for (size_t i = 0; i < m_inds.size(); i++) {
			std::vector<int> ridx;
			global::ms_global->m_uniform[caller::Algorithm]->shuffle(m_rand_seq.begin(), m_rand_seq.end());
			for (int idx : m_rand_seq) {
				if (idx != i)					ridx.emplace_back(idx);
				if (ridx.size() == 3)			break;
			}
			m_inds[i]->mutate(m_F, m_inds[ridx[0]].get(), m_inds[ridx[1]].get(), m_inds[ridx[2]].get());
			m_inds[i]->recombine(m_CR, m_recombine_strategy);
			m_offspring[i]->solut() = m_inds[i]->trial();
		}

		//evaluate offspring pop
		for (auto& i : m_offspring)
			i->evaluate();

		calculate_violation_objective(m_offspring);
		mark_individual_efeasible(m_offspring);

		std::vector<individual_type*> pop;
		for (auto& i : m_inds)
			pop.emplace_back(i.get());
		for (auto& i : m_offspring)
			pop.emplace_back(i.get());
		caculate_nichecount(pop);	
		//select next pop
		select_next_parent_population(pop);

		if (tag == EvalTag::Normal)
			m_iter++;
	}

	void DCNSGAII_DE_pop::select_next_parent_population(std::vector<DCMOEA_ind<DE::individual>*>& pop)
	{
		/* Nondominated sorting based on e-Pareto domination  */
		std::vector<int> ranks;
		std::function<OFEC::dominationship(individual_type* const&, individual_type* const&)> comp = std::bind(&DCNSGAII_DE_pop::e_Pareto_compare, this, _1, _2);
		NS::fast_sort<individual_type*>(pop, ranks, comp);
		for (size_t i = 0; i < pop.size(); i++) {
			pop[i]->set_rank(ranks[i]);
		}

		size_t cur_rank = 0;
		size_t id_ind = 0;
		while (true) {
			int count = 0;
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank)
					count++;
			int size2 = id_ind + count;
			if (size2 > this->m_inds.size()) {
				break;
			}
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank)	{
					*m_temp_pop[id_ind] = *pop[i];
					++id_ind;
				}
			cur_rank++;
			if (id_ind >= this->m_inds.size()) break;
		}
		if (id_ind < pop.size()) {
			std::vector<int> list;	// save the individuals in the overflowed front
			for (size_t i = 0; i < pop.size(); i++)
				if (pop[i]->rank() == cur_rank)
					list.push_back(i);
			int s2 = list.size();
			std::vector<Real> density(s2);
			std::vector<Real> obj(s2);
			std::vector<int> idx(s2);
			std::vector<int> idd(s2);
			for (size_t i = 0; i < s2; i++) {
				idx[i] = i;
				density[i] = 0;
			}
			for (size_t j = 0; j < 2; j++)	{
				for (size_t i = 0; i < s2; i++) {
					idd[i] = i;
					obj[i] = pop[list[i]]->get_vio_obj()[0];
				}
				merge_sort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k < s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			for (size_t j = 0; j < CONTINUOUS_CAST->objective_size(); j++) {
				for (size_t i = 0; i < s2; i++) {
					idd[i] = i;
					obj[i] = pop[list[i]]->objective()[j];
				}
				merge_sort(obj, s2, idd, true, 0, s2 - 1, s2);
				density[idd[0]] += -1.0e+30;
				density[idd[s2 - 1]] += -1.0e+30;
				for (int k = 1; k < s2 - 1; k++)
					density[idd[k]] += -(obj[idd[k]] - obj[idd[k - 1]] + obj[idd[k + 1]] - obj[idd[k]]);
			}
			idd.clear();
			obj.clear();
			int s3 = this->m_inds.size() - id_ind;
			merge_sort(density, s2, idx, true, 0, s2 - 1, s3);
			for (size_t i = 0; i < s3; i++) {
				*m_temp_pop[id_ind] = *pop[list[idx[i]]];
				++id_ind;
			}
			density.clear();
			idx.clear();
			list.clear();
		}
		for (size_t i = 0; i < m_temp_pop.size(); i++) {
			*m_inds[i] = *m_temp_pop[i];
		}
	}





	void DCNSGAII_DE_pop::reduce_radius() {
		Real production = 1.0;
		for (int i = 0; i < CONTINUOUS_CAST->variable_size(); ++i) {
			production *= (CONTINUOUS_CAST->range(i).second - CONTINUOUS_CAST->range(i).first);
		}
		Real z1 = m_z / (pow(production, (1.0 / CONTINUOUS_CAST->variable_size())));
		Real C = m_R + z1;
		Real c = sqrt(abs(log2(C / z1) / log2(exp(1))));
		Real D = m_max_K / c;
		Real q = m_k / D;
		Real f = C * exp(-pow(q, 2)) - z1;
		if (abs(f) < m_Nearzero)
			f = 0.0;
		m_r = f;
	}

	void DCNSGAII_DE_pop::caculate_nichecount(std::vector<DCMOEA_ind<DE::individual>*>& pop)
	{
		size_t size_pop = pop.size();
		std::vector<Real> nicheCount(size_pop, 0.0);
		for (int i = 0; i < size_pop; ++i) {
			for (int j = i + 1; j < size_pop; ++j) {
				Real sum1 = 0.0;
				for (int k = 0; k < CONTINUOUS_CAST->variable_size(); ++k) {
					auto difference = pop[i]->variable()[k] - pop[j]->variable()[k];
					sum1 += (difference*difference);
				}
				Real aDist = sqrt(sum1);
				if (aDist < m_R) {
					nicheCount[i] += (1 - (aDist / m_r));
					nicheCount[j] += (1 - (aDist / m_r));
				}
			}
			pop[i]->get_vio_obj()[1] = nicheCount[i];
		}
	}

	dominationship DCNSGAII_DE_pop::e_Pareto_compare(individual_type* const&s1, individual_type* const&s2)
	{	/* One efeasible one in-efeasible */
		if (s1->get_efeasible() != s2->get_efeasible()) {
			if (s1->get_efeasible())
				return dominationship::Dominating;
			else
				return dominationship::Dominated;
		}

		/* Both efeasible */
		else if (s1->get_efeasible() && s2->get_efeasible()) {
			auto nor_obj_result = objective_compare<Real>(s1->objective(), s2->objective(), CONTINUOUS_CAST->opt_mode());
			auto vio_obj_result = objective_compare<Real>(s1->get_vio_obj(), s2->get_vio_obj(), optimization_mode::Minimization);

			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Equal)
				return dominationship::Dominating;
			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Dominating)
				return dominationship::Dominating;
			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Dominating)
				return dominationship::Dominating;
			
			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Equal)
				return dominationship::Dominated;
			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Dominated)
				return dominationship::Dominated;
			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Dominated)
				return dominationship::Dominated;

			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Dominating)
				return dominationship::Non_dominated;
			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Dominated)
				return dominationship::Non_dominated;
			if (nor_obj_result == dominationship::Non_dominated||vio_obj_result == dominationship::Non_dominated)
				return dominationship::Non_dominated;
			
			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Equal)
				return dominationship::Equal;
		}
			//return objective_compare<Real>(s1->objective(), s2->objective(), CONTINUOUS_CAST->opt_mode());

		/* Both in-efeasible */
		else {
			if (s1->get_vio_obj()[0] < s2->get_vio_obj()[0])
				return dominationship::Dominating;
			else if (s1->get_vio_obj()[0] > s2->get_vio_obj()[0])
				return dominationship::Dominated;
			else
				return dominationship::Equal;
		}
	}
	dominationship DCNSGAII_DE_pop::Pareto_compare(individual_type * const & s1, individual_type * const & s2)
	{
		/* Both efeasible */
		if (s1->get_efeasible() && s2->get_efeasible()) {
			auto nor_obj_result = objective_compare<Real>(s1->objective(), s2->objective(), CONTINUOUS_CAST->opt_mode());
			auto vio_obj_result = objective_compare<Real>(s1->get_vio_obj(), s2->get_vio_obj(), optimization_mode::Minimization);

			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Equal)
				return dominationship::Dominating;
			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Dominating)
				return dominationship::Dominating;
			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Dominating)
				return dominationship::Dominating;

			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Equal)
				return dominationship::Dominated;
			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Dominated)
				return dominationship::Dominated;
			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Dominated)
				return dominationship::Dominated;

			if (nor_obj_result == dominationship::Dominated&&vio_obj_result == dominationship::Dominating)
				return dominationship::Non_dominated;
			if (nor_obj_result == dominationship::Dominating&&vio_obj_result == dominationship::Dominated)
				return dominationship::Non_dominated;
			if (nor_obj_result == dominationship::Non_dominated || vio_obj_result == dominationship::Non_dominated)
				return dominationship::Non_dominated;

			if (nor_obj_result == dominationship::Equal&&vio_obj_result == dominationship::Equal)
				return dominationship::Equal;
		}
	}


	DCNSGAII_DE::DCNSGAII_DE(param_map & v) : algorithm(v.at("algorithm name")), m_pop(v.at("popSiz")) {
	}

	void DCNSGAII_DE::initialize() {
		m_pop.initialize();
	}

	void DCNSGAII_DE::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
		m_pop.sort();
	}

	void DCNSGAII_DE::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		Real err = std::fabs(problem::get_sofar_best<Solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
		Real cons = problem::get_sofar_best<Solution<>>(0)->constraint_value()[0];
		Real vio1 = m_pop[0].get_vio_obj()[0];
		Real vio2 = m_pop[0].get_vio_obj()[1];
		measure::get_measure()->record(global::ms_global.get(), evals, err, cons, vio1, vio2);
	}
}
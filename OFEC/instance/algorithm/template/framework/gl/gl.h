/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li 
* Email: changhe.lw@google.com  
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Framework of genetic learning (GL) algorithm
*
*********************************************************************************/

#ifndef OFEC_GL_H
#define OFEC_GL_H

#include "adaptor.h"
#include <deque>
#include <limits>
#include <cmath>
#include <algorithm>
#include "../../../core/algorithm/population.h"

namespace OFEC {
	template<typename Individual>
	class GL : public population<Individual>{
	public:
		// bsf : best so far of each individual
		// hb: all historical best solutions of each individual
		// ci: improved individuals in the best so far population
		// c: all individuals in the current population
		enum UpdateScheme { bsf, hb, ci, c };
	protected:
		UpdateScheme m_ms = UpdateScheme::bsf;			//memory scheme

		std::vector<Real> m_fitness;
		std::vector<Real> m_weight;
		std::vector<Real> m_obj;

		Real m_memoryMaxObj, m_memoryMinObj, m_preMemoryMaxObj, m_preMemoryMinObj;
		Real m_wt = 1.e-3;			// weight threshold, individuals with weight less than m_wt are removed from memory

		std::vector<std::deque<int>> m_exMemory; // explicit memory: the index of all personal best memory 

		std::vector<Individual> m_his;		//historical individuals
		std::vector<Individual> m_offspring;
		std::unique_ptr<adaptor<Individual>> m_adaptor;
	public:
		template<typename ... Args>
		GL(size_t size_pop, Args&& ... args);
		void initialize_memory();
		//bool terminating();
		void update_memory(bool excute = false);
		void update_memory_HB(const std::vector<int>& index);
		void update_memory_BSF(const std::vector<int>& index);
		void update_memory_CI(const std::vector<int>& index);
		void update_memory_C();
		virtual void initialize_curpop() = 0;
		int get_number_impr() {
			return m_num_improve;
		}
		void reset_flag() {
			for (int i = 0; i < this->size(); ++i)
				this->m_inds[i]->set_active_flag(false);
		}
		void set_alpha(Real alpha) { m_alpha = alpha; }
		void set_beta(Real beta) { m_beta = beta; }
		void set_gamma(Real gamma) { m_gamma = gamma; }
		void set_update_scheme(int uspl) { m_ms = (UpdateScheme)uspl; }
	protected:
		EvalTag update();
		int m_num_improve;
		Real m_alpha = 0.5, m_beta = 3, m_gamma = 6;

	};

	template<typename Individual>
	template<typename ...Args>
	GL<Individual>::GL(size_t size_pop, Args&& ...args) : population<Individual>(size_pop, std::forward<Args>(args)...),\
		m_fitness(size_pop), m_weight(size_pop), m_obj(size_pop), m_exMemory(size_pop), m_num_improve(0) { }

	//template<typename Individual>
	//void GL<Individual>::initialize_curpop() {
	//	m_offspring.resize(this->size());
	//	for (int i = 0; i < this->size(); i++) {
	//		m_offspring[i].initialize(this->m_pop[i]->id());
	//	}
	//}

	template<typename Individual>
	void GL<Individual>::initialize_memory() {
		m_memoryMaxObj = m_memoryMinObj = this->m_inds[0]->objective(0);
		for (int i = 0; i < this->size(); ++i) {
			Real obj = this->m_inds[i]->objective(0);
			if (obj > m_memoryMaxObj) m_memoryMaxObj = obj;
			if (obj < m_memoryMinObj) m_memoryMinObj = obj;
		}
		//std::vector<int> indiv(this->m_num_vars);
		Real gap = m_memoryMaxObj - m_memoryMinObj + 1;
		for (int i = 0; i < this->size(); ++i) {
			m_obj[i] = this->m_inds[i]->objective(0);
			if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization)	m_fitness[i] = (m_memoryMaxObj - m_obj[i] + 1) / gap;
			else m_fitness[i] = (m_obj[i] - m_memoryMinObj + 1) / gap;

			m_weight[i] = 1. / (1 + exp(-m_fitness[i]));
			m_exMemory[i].push_front(i);
			//for (int j = 0; j < this->m_num_vars; j++)
			//	indiv[j] = this->m_pop[i]->variable()[j];
			//m_hisIndi.push_back(indiv);
			m_his.push_back(*this->m_inds[i]);
		}
		m_adaptor->update_probability(this->m_inds, m_weight);
		m_preMemoryMaxObj = m_memoryMaxObj;
		m_preMemoryMinObj = m_memoryMinObj;
	}

	template<typename Individual>
	void GL<Individual>::update_memory(bool excute) {

		std::vector<int> index;  //the updated individual in the best so far

		if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization) {
			if (!excute) {
				m_memoryMinObj = m_preMemoryMinObj;
				for (int i = 0; i < this->size(); ++i)
				{
					if (this->m_inds[i]->is_improved() == true)
					{
						index.push_back(i);
						if (m_memoryMinObj > this->m_inds[i]->objective(0))
							m_memoryMinObj = this->m_inds[i]->objective(0);
					}
				}
			}
			else {
				m_memoryMinObj = std::numeric_limits<Real>::max();
				for (int i = 0; i < this->size(); ++i) {
					index.push_back(i);
					if (m_memoryMinObj > this->m_inds[i]->objective(0))
						m_memoryMinObj = this->m_inds[i]->objective(0);
				}
			}

		}
		else {
			if (!excute) {
				m_memoryMaxObj = m_preMemoryMaxObj;
				for (int i = 0; i < this->size(); ++i)
				{
					if (this->m_inds[i]->is_improved() == true)
					{
						index.push_back(i);
						if (m_memoryMaxObj < this->m_inds[i]->objective(0))
							m_memoryMaxObj = this->m_inds[i]->objective(0);
					}
				}
			}
			else {
				m_memoryMaxObj = -std::numeric_limits<Real>::max();
				for (int i = 0; i < this->size(); ++i)
				{
					index.push_back(i);
					if (m_memoryMaxObj < this->m_inds[i]->objective(0))
						m_memoryMaxObj = this->m_inds[i]->objective(0);
				}
			}
		}

		//if (m_ms != GL::c)
		//	if (index.empty())
		//		return;

		switch (m_ms)
		{
		case GL::bsf:
			update_memory_BSF(index);
			break;
		case GL::hb:
			update_memory_HB(index);
			break;
		case GL::ci:
			update_memory_CI(index);
			break;
		case GL::c:
			update_memory_C();
			break;
		default:
			break;
		}

	}

	template<typename Individual>
	void GL<Individual>::update_memory_HB(const std::vector<int> & index)
	{
		if (m_memoryMinObj < m_preMemoryMinObj)
			m_preMemoryMinObj = m_memoryMinObj;

		Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization) {
			if (m_memoryMinObj < m_preMemoryMinObj)
			{
				for (int i = 0; i < m_weight.size(); ++i)
				{
					m_fitness[i] = (m_preMemoryMaxObj - m_obj[i] + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
		}
		else {
			if (m_memoryMinObj < m_preMemoryMinObj)
			{
				for (int i = 0; i < m_weight.size(); ++i)
				{
					m_fitness[i] = (m_preMemoryMaxObj - m_obj[i] + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
		}




		std::vector<Real> weight(index.size()), fitness(index.size());
		for (int i = 0; i < weight.size(); ++i)
		{
			fitness[i] = (m_preMemoryMaxObj - this->m_inds[index[i]]->objective(0)) / gap;
			weight[i] = 1 / (1 + exp(-fitness[i]));
		}
		int z = 0; //改进个体按顺序1，2，...出现
		for (int i = 0; i < m_exMemory.size(); ++i)
		{
			int exMemorySize = m_exMemory[i].size();
			for (int j = 0; j < exMemorySize; ++j)
			{
				if (this->m_inds[i]->is_improved() == true)
				{
					if (m_memoryMinObj < m_preMemoryMinObj)
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / (pow((j + 2), 2));
					else
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] * pow((j + 1), 2) / pow((j + 2), 2);
					if (j == m_exMemory[i].size() - 1)
					{
						if (m_weight[m_exMemory[i][j]] / weight[z] < m_wt) {
							int pos = m_exMemory[i][j];
							m_weight[pos] = weight[z];
							m_obj[pos] = this->m_inds[i]->objective(0);
							m_fitness[pos] = fitness[z];
							m_exMemory[i].pop_back();
							m_exMemory[i].push_front(pos);
							for (int e1 = 0; e1 < this->m_inds[i]->variable().size(); e1++)
								m_his[pos].variable()[e1] = this->m_inds[i]->variable()[e1];
						}
						else {
							m_obj.push_back(this->m_inds[i]->objective(0));
							m_fitness.push_back(fitness[z]);
							m_weight.push_back(weight[z]);
							m_exMemory[i].push_front(m_weight.size() - 1);
							m_his.push_back(*this->m_inds[i]);
						}
						z++;
					}
				}
				else
				{
					if (m_memoryMinObj < m_preMemoryMinObj)
						m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / pow((j + 1), 2);
				}
			}
		}

		m_adaptor->update_probability(m_his, m_weight);

		std::vector<Real>::iterator maxObj = std::max_element(m_obj.begin(), m_obj.end());
		m_preMemoryMaxObj = *maxObj;
	}

	template<typename Individual>
	void GL<Individual>::update_memory_BSF(const std::vector<int> & index)
	{
		/*if (global::ms_global->m_problem->opt_mode(0)== optimization_mode::Minimization) {
			m_memoryMaxObj = this->m_pop[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMaxObj < this->m_pop[i]->objective(0))
					m_memoryMaxObj = this->m_pop[i]->objective(0);
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < this->size(); i++) {
					m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->objective(0) + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < index.size(); ++i)
				{
					m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->objective(0) + 1) / gap;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		else {
			m_memoryMinObj = this->m_pop[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMinObj > this->m_pop[i]->objective(0))
					m_memoryMinObj = this->m_pop[i]->objective(0);
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj )
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < this->size(); i++) {
					m_fitness[i] = (this->m_pop[i]->objective(0) - m_preMemoryMinObj + 1) / gap;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
				for (int i = 0; i < index.size(); ++i)
				{
					m_fitness[index[i]] = (this->m_pop[index[i]]->objective(0) - m_preMemoryMinObj + 1) / gap;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		*/
		/*if (global::ms_global->m_problem->opt_mode(0)== optimization_mode::Minimization) {
			m_memoryMaxObj = this->m_pop[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMaxObj < this->m_pop[i]->objective(0))
					m_memoryMaxObj = this->m_pop[i]->objective(0);
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < this->size(); i++) {
					if(gap>0)				m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->objective(0)) / gap;
					else m_fitness[i] = 0;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < index.size(); ++i)
				{
					if(gap>0)	m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->objective(0)) / gap;
					else m_fitness[index[i]] = 0;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}
		else {
			m_memoryMinObj = this->m_pop[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMinObj > this->m_pop[i]->objective(0))
					m_memoryMinObj = this->m_pop[i]->objective(0);
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj)
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < this->size(); i++) {
					if(gap>0)
					m_fitness[i] = (this->m_pop[i]->objective(0) - m_preMemoryMinObj) / gap;
					else m_fitness[i] = 0;
					m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
				}
			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				for (int i = 0; i < index.size(); ++i)
				{
					if(gap>0)				m_fitness[index[i]] = (this->m_pop[index[i]]->objective(0) - m_preMemoryMinObj) / gap;
					else m_fitness[index[i]] = 0;
					m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
				}
			}
		}*/
		if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization) {
			m_memoryMaxObj = this->m_inds[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMaxObj < this->m_inds[i]->objective(0))
					m_memoryMaxObj = this->m_inds[i]->objective(0);
			}
			if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < this->size(); i++) {
						m_fitness[i] = (m_preMemoryMaxObj - this->m_inds[i]->objective(0)) / gap;
						m_weight[i] = pow(m_fitness[i], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[i]));
					}
				}
				else {
					for (int i = 0; i < this->size(); i++) {
						m_fitness[i] = 0;
						m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
					}
				}

			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_inds[index[i]]->objective(0)) / gap;
						m_weight[index[i]] = pow(m_fitness[index[i]], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[index[i]]));
					}
				}
				else {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = 0;
						m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
					}
				}

			}
		}
		else {
			m_memoryMinObj = this->m_inds[0]->objective(0);
			for (int i = 1; i < this->size(); ++i) {
				if (m_memoryMinObj > this->m_inds[i]->objective(0))
					m_memoryMinObj = this->m_inds[i]->objective(0);
			}

			if (m_memoryMaxObj > m_preMemoryMaxObj || m_memoryMinObj != m_preMemoryMinObj)
			{
				m_preMemoryMinObj = m_memoryMinObj;
				m_preMemoryMaxObj = m_memoryMaxObj;

				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < this->size(); i++) {
						m_fitness[i] = (this->m_inds[i]->objective(0) - m_preMemoryMinObj) / gap;
						m_weight[i] = pow(m_fitness[i], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[i]));
					}
				}
				else {
					for (int i = 0; i < this->size(); i++) {
						m_fitness[i] = 0;
						m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
					}
				}

			}
			else {
				Real gap = m_preMemoryMaxObj - m_preMemoryMinObj;
				if (gap > 0) {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = (this->m_inds[index[i]]->objective(0) - m_preMemoryMinObj) / gap;
						m_weight[index[i]] = pow(m_fitness[index[i]], m_beta) / (1 + exp(m_gamma - 12 * m_fitness[index[i]]));
					}
				}
				else {
					for (int i = 0; i < index.size(); ++i)
					{
						m_fitness[index[i]] = 0;
						m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
					}
				}

			}
		}
		m_adaptor->update_probability(this->m_inds, m_weight);
	}

	template<typename Individual>
	void GL<Individual>::update_memory_CI(const std::vector<int> & index) {
		m_preMemoryMaxObj = this->m_inds[index[0]]->objective(0);
		m_preMemoryMinObj = this->m_inds[index[0]]->objective(0);
		for (int i = 1; i < index.size(); ++i)
		{
			if (m_preMemoryMaxObj < this->m_inds[index[i]]->objective(0))
				m_preMemoryMaxObj = this->m_inds[index[i]]->objective(0);
			if (m_preMemoryMinObj > this->m_inds[index[i]]->objective(0))
				m_preMemoryMinObj = this->m_inds[index[i]]->objective(0);
		}
		Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < index.size(); ++i)
		{
			if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization)	m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_inds[index[i]]->objective(0) + 1) / gap;
			else m_fitness[index[i]] = (this->m_inds[index[i]]->objective(0) - m_preMemoryMinObj + 1) / gap;

			m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
		}

		m_adaptor->update_probability(this->m_inds, m_weight, &index);
	}

	template<typename Individual>
	void GL<Individual>::update_memory_C() {
		m_preMemoryMaxObj = m_offspring[0].objective(0);
		m_preMemoryMinObj = m_offspring[0].objective(0);
		for (int i = 1; i < this->size(); ++i)
		{
			if (m_preMemoryMaxObj < m_offspring[i].objective(0))
				m_preMemoryMaxObj = m_offspring[i].objective(0);
			if (m_preMemoryMinObj > m_offspring[i].objective(0))
				m_preMemoryMinObj = m_offspring[i].objective(0);
		}
		Real gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < this->size(); ++i)
		{
			if (global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization)	m_fitness[i] = (m_preMemoryMaxObj - m_offspring[i].objective(0) + 1) / gap;
			else m_fitness[i] = (m_offspring[i].objective(0) - m_preMemoryMinObj + 1) / gap;
			m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
		}

		m_adaptor->update_probability(m_offspring, m_weight);
	}

	//template<typename Individual>
	//bool GL<Individual>::terminating() {
	//	//TermMean * term = dynamic_cast<TermMean*>(this->m_term.get());
	//	if (this->m_term->terminating()) {
	//		if (this->m_iter == 0) return false;
	//		return true;
	//	}
	//	return false;
	//}

	template<typename Individual>
	EvalTag GL<Individual>::update() {
		EvalTag rf = EvalTag::Normal;
		rf = m_adaptor->update_solution(this->m_inds, m_offspring, m_num_improve);
		return rf;
	}
}
#endif // !GENETIC_LEARNING_H


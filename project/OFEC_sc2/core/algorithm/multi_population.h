/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class MultiPopulation manages a set of populations with the same type of individuals.
*
*********************************************************************************/
#ifndef OFEC_MULTI_POPULATION_H
#define OFEC_MULTI_POPULATION_H

#include <vector>
#include <memory>
#include "individual.h"

namespace OFEC {
	template<typename T>
	class MultiPopulation {
	public:
		using PopType = T;
		using IterType = typename std::vector<std::unique_ptr<T>>::iterator;
		using CIterType = typename std::vector<std::unique_ptr<T>>::const_iterator;
	protected:
		std::vector<std::unique_ptr<T>> m_pops;
		int m_maxsize = -1;		// the maximum size of each sub-population
	public:
		MultiPopulation() = default;
		MultiPopulation(int n) : m_pops(n) {}
		template<typename ... Args>
		MultiPopulation(size_t n, size_t subsize, Args&& ...args) : m_pops(n) {
			for (auto& i : m_pops)
				i.reset(new T(subsize, std::forward<Args>(args)...));
		}
		virtual ~MultiPopulation() {}

		/* Member access methods */
		size_t size() const { return m_pops.size(); }
		T& operator[](size_t i) { return *m_pops[i]; }
		const T& operator[](size_t i) const { return *m_pops[i]; }
		T& at(size_t i) { return *m_pops[i]; }
		const T& at(size_t i) const { return *m_pops[i]; }
		IterType begin() { return m_pops.begin(); }
		IterType end() { return m_pops.end(); }
		CIterType begin() const { return m_pops.begin(); }
		CIterType end() const { return m_pops.end(); }

		bool isActive() const {
			for (size_t k = 0; k < m_pops.size(); k++) {
				if (m_pops[k]->isActive())
					return true;
			}
			return false;
		}

		void resizeObj(int n) {
			for (auto& i : m_pops)
				i->resizeObj(n);
		}

		void append(std::unique_ptr<T>& p) {
			m_pops.emplace_back(p.release());
			/* Set id */
			// TODO
		}

		void append(T& p) {
			m_pops.emplace_back(new T(p));
			/* Set id */
			// TODO
		}

		void append(T&& p) {
			m_pops.emplace_back(new T(std::forward<T>(p)));
			/* Set id */
			// TODO
		}

		IterType remove(IterType iter) {
			return m_pops.erase(iter);
		}

		void clear() {
			m_pops.clear();
		}

		virtual IterType merge(IterType iter1, IterType iter2) {
			size_t pre_size = (*iter1)->size();
			(*iter1)->m_inds.resize(pre_size + (*iter2)->size());
			for (size_t i = 0; i < (*iter2)->size(); i++) {
				std::swap((*iter1)->m_inds[pre_size + i], (*iter2)->m_inds[i]);
			}
			return m_pops.erase(iter2);
		}

		void resize(size_t N) {
			m_pops.resize(N);
			for (size_t k = 0; k < N; k++) {
				if (!m_pops[k])
					m_pops[k].reset(new T);
			}
		}

		virtual void handleEvalTag(EvalTag& tag) {}

		virtual EvalTag evolve(int id_pro, int id_alg, int id_rnd) {
			auto rf = EvalTag::Normal;
			for (auto& subpop : m_pops) {
				if (!subpop->isActive()) continue;
				rf = subpop->evolve(id_pro, id_alg, id_rnd);
				if (rf != EvalTag::Normal) {
					this->handleEvalTag(rf);
					return rf;
				}
			}
			return rf;
		}
	};
}
#endif // !OFEC_MULTI_POPULATION_H

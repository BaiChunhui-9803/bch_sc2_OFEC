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
* class Population manages a set of individuals.
*
*********************************************************************************/

#ifndef OFEC_POPULATION_H
#define OFEC_POPULATION_H

#include "../global.h"
#include "../../utility/functional.h"
#include "../../utility/nondominated_sorting/fast_sort.h"
#include "../../utility/nondominated_sorting/filter_sort.h"
#include "../instance_manager.h"

namespace OFEC {
	template<typename>  class MultiPopulation;

	template<typename T>
	class Population {
	public:
		virtual ~Population() = default;
		template<typename> friend class MultiPopulation;
		using IndType = T;
		using SolType = typename IndType::SolType;
		using IterType = typename std::vector<std::unique_ptr<T>>::iterator; 
		using CIterType = typename std::vector<std::unique_ptr<T>>::const_iterator;
		//element access
		size_t size() const noexcept { return m_inds.size(); }
		const T& operator[](size_t i) const { return *m_inds[i]; }
		T& operator[](size_t i) { return *m_inds[i]; }
		const T& at(size_t i) const { return *m_inds[i]; }
		T& at(size_t i) { return *m_inds[i]; }
		IterType begin() { return m_inds.begin(); }
		IterType end() { return m_inds.end(); }
		CIterType begin() const { return m_inds.begin(); }
		CIterType end() const { return m_inds.end(); }
		const std::list<std::unique_ptr<SolType>>& best() const { return m_best; }
		const std::list<std::unique_ptr<SolType>>& worst() const { return m_worst; }
		std::vector<int> order(int idx) const; // get individual(s) with order idx
		int iteration() const { return m_iter; }
		int timeBestUpdated() const { return m_time_best_updated; }
		int timeWorstUpdated() const { return m_time_worst_updated; }

		//update	
		const std::list<std::unique_ptr<SolType>> &best(int id_pro) {
			updateBest(id_pro);
			return m_best;
		}
		const std::list<std::unique_ptr<SolType>> &worst(int id_pro) {
			updateWorst(id_pro);
			return m_worst;
		}
		void updateBest(int id_pro);
		void updateWorst(int id_pro);

		void handleEvalTag(EvalTag tag) {}

		//constructors and members
		Population() = default;
		template<typename ... Args>
		Population(size_t n, int id_pro, Args&& ...args);
		Population(const Population &rhs);
		Population& operator=(const Population &rhs);
		Population(Population&&rhs);
		Population& operator=(Population &&rhs);

		//operations
		virtual void append(const T& ind);
		virtual void append(T&& ind);
		virtual void append(std::unique_ptr<T>& ind);
		virtual IterType remove(IterType iter_ind) { return m_inds.erase(iter_ind); }
		virtual void sort(int id_pro);
		void resizeObj(int n);
		virtual void initialize(int id_pro, int id_rnd); //a uniformly distributed initialization by default
		template<typename Fun, typename Problem, typename... Args>
		void initialize(Fun fun, const Problem* pro, Args && ... args);
		EvalTag evaluate(int id_pro, int id_alg); // evaluate each individual 
		virtual EvalTag evolve(int id_pro, int id_alg, int id_rnd) { return EvalTag::Normal; }
		void reset(); // delete all individuals
		std::map<Real, size_t> nearestNeighbour(size_t idx, int id_pro, int k = 1);
		void resetImproved();
		Real rank(const SolType &x, int id_pro);
		int numRanks() { return m_order.rbegin()->first; }
		int id() const { return m_id; }
		void setId(int id) { m_id = id; }
		bool isActive()const noexcept { return m_active; }
		void setActive(bool value) noexcept { m_active = value; }

	protected:		
		bool updateBest(const SolType&, int id_pro);
		bool updateWorst(const SolType&, int id_pro);

	protected:
		int m_id = -1;
		size_t m_iter = 0;			// the current number of iterations
		bool m_active = true;
		int m_time_best_updated = -1, m_time_worst_updated = -1;
		std::multimap<int, int> m_order;
		std::vector<std::unique_ptr<IndType>> m_inds;
		std::list<std::unique_ptr<SolType>> m_best, m_worst;   // best so far
	};

	template<typename T>
	std::vector<int> Population<T>::order(int idx) const {
		auto range_ = m_order.equal_range(idx);
		std::vector<int> inds;
		for (auto i = range_.first; i != range_.second; ++i)
			inds.push_back(i->second);
		return inds;
	}

	template<typename T>
	void Population<T>::updateBest(int id_pro) {
		for (auto &ptr_ind : m_inds) {
			if (updateBest(ptr_ind->phenotype(), id_pro))
				m_time_best_updated = m_iter;
		}
	}

	template<typename T>
	void Population<T>::updateWorst(int id_pro) {
		for (auto &ptr_ind : m_inds)
			if (updateWorst(ptr_ind->phenotype(), id_pro))
				m_time_worst_updated = m_iter;
	}

	template<typename T>
	bool Population<T>::updateBest(const SolType &x, int id_pro) {
		bool is_best = true;
		for (auto iter = m_best.begin(); iter != m_best.end();) {
			auto result = x.compare(**iter, GET_PRO(id_pro).optMode());
			if (result == Dominance::Dominated || result == Dominance::Equal) {
				is_best = false;
				break;
			}
			else if (result == Dominance::Dominant)
				iter = m_best.erase(iter);
			else
				iter++;
		}
		if (is_best) {
			m_best.emplace_back(new SolType(x));
			m_time_best_updated = m_iter;
		}
		return is_best;
	}

	template<typename T>
	bool Population<T>::updateWorst(const SolType&x, int id_pro) {
		bool is_worst = true;
		for (auto iter = m_worst.begin(); iter != m_worst.end();) {
			auto result = x.compare(**iter, GET_PRO(id_pro).optMode());
			if (result == Dominance::Dominant) {
				is_worst = false;
				break;
			}
			else if (result == Dominance::Dominated)
				iter = m_worst.erase(iter);
			else
				iter++;
		}
		if (is_worst)
			m_worst.emplace_back(new SolType(x));
		return is_worst;
	}

	template<typename T>
	template<typename ... Args>
	Population<T>::Population(size_t n, int id_pro, Args&& ... args) : m_inds(n) {
		size_t num_objs = GET_PRO(id_pro).numObjectives();
		size_t num_cons = GET_PRO(id_pro).numConstraints();
		for (auto& i : m_inds)
			i = std::make_unique<T>(num_objs, num_cons, std::forward<Args>(args)...);
	}

	template<typename T>
	Population<T>::Population(const Population &rhs) : 
		m_id(rhs.m_id), 
		m_iter(rhs.m_iter),
		m_active(rhs.m_active),
		m_time_best_updated(rhs.m_time_best_updated),
		m_time_worst_updated(rhs.m_time_worst_updated),
		m_order(rhs.m_order)
	{
		for (size_t i = 0; i < rhs.size(); i++)	
			m_inds.push_back(std::make_unique<IndType>(rhs[i]));
		for (auto &ptr_sol : rhs.m_best)
			m_best.emplace_back(new SolType(*ptr_sol));
		for (auto &ptr_sol : rhs.m_worst)
			m_worst.emplace_back(new SolType(*ptr_sol));
	}

	template<typename T>
	Population<T>& Population<T>::operator=(const Population &rhs) {
	    if (&rhs == this)
	        return *this;
		m_id = rhs.m_id;
		m_iter = rhs.m_iter;
		m_active = rhs.m_active;
		m_time_best_updated = rhs.m_time_best_updated;
		m_time_worst_updated = rhs.m_time_worst_updated;
		m_order = rhs.m_order;
		m_inds.clear();
		m_best.clear();
		m_worst.clear();
		for (size_t i = 0; i < rhs.size(); i++)
			m_inds.push_back(std::make_unique<T>(rhs[i]));
		for (auto& ptr_sol : rhs.m_best)
			m_best.emplace_back(new SolType(*ptr_sol));
		for (auto &ptr_sol : rhs.m_worst)
			m_worst.emplace_back(new SolType(*ptr_sol));
		return *this;
	}

	template<typename T>
	Population<T>::Population(Population&&rhs) : 
		m_id(rhs.m_id),
		m_iter(rhs.m_iter),
		m_active(rhs.m_active),
		m_time_best_updated(rhs.m_time_best_updated),
		m_time_worst_updated(rhs.m_time_worst_updated),
		m_order(rhs.m_order),
		m_inds(std::move(rhs.m_inds)),
		m_best(std::move(rhs.m_best)),
		m_worst(std::move(rhs.m_worst)) {}

	template<typename T>
	Population<T>& Population<T>::operator=(Population &&rhs) {
        if (&rhs == this)
            return *this;
		m_id = rhs.m_id;
		m_iter = rhs.m_iter;
		m_active = rhs.m_active;
		m_time_best_updated = rhs.m_time_best_updated;
		m_time_worst_updated = rhs.m_time_worst_updated;
		m_order = std::move(rhs.m_order);
		m_inds = std::move(rhs.m_inds);
		m_best = std::move(rhs.m_best);
		m_worst = std::move(rhs.m_worst);
		return *this;
	}

	template<typename T>
	void Population<T>::reset() {
		m_iter = 0;
		m_active = true;
		m_time_best_updated = -1;
		m_order.clear();
		m_inds.clear();
		m_best.clear();
		m_worst.clear();
	}

	template<typename T>
	void Population<T>::initialize(int id_pro, int id_rnd) {
		for (int i = 0; i < m_inds.size(); ++i) {
			m_inds[i]->initialize(i, id_pro, id_rnd);
		}
	}

	template<typename T>
	template<typename Fun, typename Problem, typename... Args>
	void  Population<T>::initialize(Fun fun, const Problem* pro, Args&& ... args) {
		fun(m_inds, pro, std::forward<Args>(args)...);
	}

	template<typename T>
	void Population<T>::sort(int id_pro) {
		std::vector<std::vector<Real>*> data(m_inds.size());
		for (size_t i = 0; i < m_inds.size(); ++i)
			data[i] = &(m_inds[i]->objective());
		std::vector<int> rank;
		//NS::fast_sort<Real>(data, rank, global::ms_global->m_problem->optMode());
		if (m_inds.size() > 1e3) {
			NS::filter_sort_p<Real>(data, rank, GET_PRO(id_pro).optMode());
		}
		else {
			NS::filter_sort<Real>(data, rank, GET_PRO(id_pro).optMode());
		}
		
		m_order.clear();
		for (size_t i = 0; i < m_inds.size(); i++) {
			m_order.insert(std::pair<int, int>(rank[i], m_inds[i]->id()));
			m_inds[i]->setRank(rank[i]);
		}
	}

	template<typename T>
	std::map<Real, size_t> Population<T>::nearestNeighbour(size_t idx, int id_pro, int k) {
		//TODO return k nearest neighbour of individual idx together with the distance
		Real Min_dis;
		size_t index;
		size_t count = 0;
		std::vector<Real> dis(size());
		for (size_t i = 0; i < size(); ++i) {
			if (idx == i) {
				dis[i] = 0;
				continue;
			}
			dis[i] = m_inds[idx]->variableDistance(*m_inds[i], id_pro);
		}

		for (size_t i = 0; i < size(); ++i) {
			if (i == idx) continue;
			++count;
			if (count == 1) {
				Min_dis = dis[i];
				index = i;
			}
			else if (dis[i]<Min_dis) {
				Min_dis = dis[i];
				index = i;
			}
		}
		
		std::map<Real, size_t> result;
		result[Min_dis] = index;

		return result;
	}

	template<typename T>
	EvalTag Population<T>::evaluate(int id_pro, int id_alg) {
		EvalTag tag = EvalTag::Normal;
		for (auto &i : m_inds) {
			tag = i->evaluate(id_pro, id_alg);
			if (tag != EvalTag::Normal) break;
		}
		return tag;
	}

	template<typename T>
	void Population<T>::resizeObj(int n) {
		for (auto &i : m_inds)
			i->resizeObj(n);
	}

	template<typename T>
	void Population<T>::resetImproved() {
		for (auto &i : m_inds) {
			i->setImproved(false);
		}
	}

	template<typename T>
	Real Population<T>::rank(const typename T::SolType &x, int id_pro) {
		int i = 0;
		while (1) {
			auto l = m_order.lower_bound(i), u = m_order.upper_bound(i);
			while (l != u) {
				Dominance r = objectiveCompare(x.objective(), m_inds[l->second]->objective(), GET_PRO(id_pro).optMode());
				if(r == Dominance::Dominant) return i - 0.5;
				if (r == Dominance::Dominated) break;
				++l;
			}
			if (l == u) return i;			
			if (u == m_order.end()) return i + 0.5;
			++i;
		}		 
	}

	template<typename T>
	void Population<T>::append(T&& ind) {
		m_inds.emplace_back(new T(std::forward<T>(ind)));
		/* Set id */ 
		// TODO
	}

	template<typename T>
	void Population<T>::append(std::unique_ptr<T>& ind) {
		m_inds.emplace_back(ind.release());
		/* Set id */
		// TODO
	}

	template<typename T>
	void Population<T>::append(const T& ind) {
		m_inds.emplace_back(new T(ind));
		/* Set id */
		// TODO
	}
}

#endif // !OFEC_POPULATION_H

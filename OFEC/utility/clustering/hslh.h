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
* heuristic single-linkage hierarchical clustering
*********************************************************************************/
#ifndef HSLHCLUSTERING_H
#define HSLHCLUSTERING_H

#include "group.h"

#include<utility>
#include<vector>

#include"../../core/algorithm/population.h"		//

namespace OFEC {
	template<typename Individual >
	class HSLH {
		std::vector<Group<Individual> > m_group;
		std::vector<std::vector<Real> > m_dis;	//distance between all objects

		Real m_inter_dis;			// inter distance of all group
		Real m_intra_dis;			// intra distance of all group
		int m_space;				//objective space or solution space

		typedef typename std::vector<Group<Individual>>::iterator mem_iter;

	public:
		HSLH(int space = 0) : m_space(space) {}

		HSLH(Population<Individual> &p, int id_pro, int space = 0) :
			m_space(space), 
			m_group(p.size()), 
			m_dis(p.size(), std::vector<Real>(p.size())) 
		{	//construct function	
			updateData(p, id_pro);
		}

		HSLH(std::vector<std::unique_ptr<Individual>>& p, int id_pro, int space = 0) :
			m_space(space), 
			m_group(p.size()), 
			m_dis(p.size(), std::vector<Real>(p.size())) 
		{	//construct function	
			updateData(p, id_pro);
		}

		void updateData(Population<Individual>& pop) {
			if (m_group.size() != pop.size())
				m_group.resize(pop.size());
			if (m_dis.size() != pop.size()) {
				m_dis.assign(pop.size(), std::vector<Real>(pop.size()));
			}
			for (int k = 0; k < pop.size(); ++k) {
				m_group[k].initialize(std::make_pair(&pop[k], k));
			}
			for (int i = 0; i < pop.size(); i++) {					//initialize m_dis
				m_dis[i][i] = 0;
				for (int j = 0; j < i; j++) {
					if (m_space == 0)
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->varDistance(*m_group[j].begin()->second);
					else
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->objDistance(*m_group[j].begin()->second);
				}
			}
			updateDistance();
		}

		void updateData(std::vector<std::unique_ptr<Individual>>& inds, int id_pro) {
			if (m_group.size() != inds.size())
				m_group.resize(inds.size());
			if (m_dis.size() != inds.size()) {
				m_dis.assign(inds.size(), std::vector<Real>(inds.size()));
			}
			for (int k = 0; k < inds.size(); ++k) {
				m_group[k].initialize(std::make_pair(inds[k].get(), k));
			}
			for (int i = 0; i < inds.size(); i++) {					//initialize m_dis
				m_dis[i][i] = 0;
				for (int j = 0; j < i; j++) {
					if (m_space == 0)
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->variableDistance(*m_group[j].begin()->second, id_pro);
					else
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->objectiveDistance(*m_group[j].begin()->second);
				}
			}
			updateDistance();
		}

		void updateData(std::vector<Individual*>& inds) {
			if (m_group.size() != inds.size())
				m_group.resize(inds.size());
			if (m_dis.size() != inds.size()) {
				m_dis.assign(inds.size(), std::vector<Real>(inds.size()));
			}
			for (int k = 0; k < inds.size(); ++k) {
				m_group[k].initialize(std::make_pair(inds[k], k));
			}
			for (int i = 0; i < inds.size(); i++) {					//initialize m_dis
				m_dis[i][i] = 0;
				for (int j = 0; j < i; j++) {
					if (m_space == 0)
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->varDistance(*m_group[j].begin()->second);
					else
						m_dis[i][j] = m_dis[j][i] = m_group[i].begin()->second->objDistance(*m_group[j].begin()->second);
				}
			}
			updateDistance();
		}

		void updateDistance() {
			m_inter_dis = m_intra_dis = 0;
			//calculate inter-distance
			for (unsigned i = 0; i < m_group.size(); i++) {
				for (unsigned j = 0; j < i; j++) {
					m_inter_dis += m_dis[m_group[i].best()][m_group[j].best()];
				}
			}
		
			m_inter_dis /= m_group.size()*(m_group.size() - 1) / 2;		//average inter distance

			// calulate intra-distance
			for (unsigned i = 0; i < m_group.size(); i++) {
				m_intra_dis += m_group[i].intraDis();
			}

			m_intra_dis /= m_group.size();				//average intra distance

		}
		
		void clustering(int minsize, int id_pro) {			
			while (1) {
				mem_iter i = m_group.begin();
				while (i != m_group.end() && i->size() > 1) i++;
				if (i == m_group.end()) 
					break;

				auto p = nearest_group(-1);		//find nearest two group

				if ((p.first == m_group.end())) 
					break;

				p.first->merge(*p.second, m_dis, id_pro);
				m_group.erase(p.second);

				updateDistance();				//update distance

				if (m_inter_dis <= m_intra_dis)
					break;

			}

		}

		void clustering(int maxsize, int minsize, int id_pro) {			
			while (1) {
				mem_iter i = m_group.begin();
				while (i != m_group.end() && i->size() > 1) 
					i++;
				if (i == m_group.end()) 
					break;

				auto &&p = nearest_group(maxsize);		//find nearest two group

				if ((p.first == m_group.end()) ) 
					break;
				p.first->merge(*p.second, m_dis, id_pro);
				m_group.erase(p.second);

				updateDistance();				//update distance

				if (m_inter_dis <= m_intra_dis)
					break;

			}
		}

		Group<Individual>& operator[](const int i) {
			return m_group[i];
		}
		int size() {					//size of group
			return m_group.size();
		}
		int size_above(int n) {
			int count = 0;
			for (auto i = m_group.begin(); i != m_group.end(); ++i) {
				if (i->size() > n) ++count;
			}
			return count;
		}

		std::pair<mem_iter, mem_iter> nearest_group(int maxsize) {
			
			Real Min_dis = std::numeric_limits<Real>::max(), dist;
			auto g1 = m_group.end(), g2 = m_group.end();

			for (auto i = m_group.begin(); i != m_group.end(); ++i) {
				// can't merge two mp_groups whose total m_number are greater than m_maxsize
				for (auto j = i+1; j != m_group.end(); ++j) {		
					if (maxsize > 0 && (i->size() + j->size()) > maxsize) continue;

					dist = m_dis[i->best()][j->best()];
					if (Min_dis > dist) {
						Min_dis = dist;
						g1 = i;
						g2 = j;
					}
				}
			}

			return std::make_pair(g1, g2);
		}
	};



}

#endif // CLUSTERING_H

//Register TravellingSalesman "TSP" TSP,ComOP,SOP

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Oct 2014
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef TSP_H
#define TSP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"

namespace OFEC {
#define GET_TSP(id_pro) dynamic_cast<TravellingSalesman&>(InstanceManager::ms_instance_manager->getProblem(id_pro))
	class TravellingSalesman : public Problem {
	protected:
		std::string m_file_name;
		std::vector<std::vector<std::vector<Real>>> mvvv_cost;  // the cost between cities
		size_t m_num_cities;
		Domain<int> m_domain; // boundary of each variable
		Optima<VarVec<int>> m_optima; // the optimal solution of problem
		std::vector<std::vector<int>> mvv_nearby; // nearby cities of each city
		bool m_if_valid_check = true;
		std::vector<std::vector<Real>> m_coordinate;

	public:
		void initSolution(SolBase &s, int id_rnd) const override;
		bool same(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const SolBase &s1, const SolBase &s2) const override;
		Real variableDistance(const VarBase &s1, const VarBase &s2) const override;

		bool isValid(SolBase& s) const;
		std::pair<int, int> adjacentCities(const SolBase &s, int city) const; //return the next and previous cities of city in solution s
		const std::vector<std::vector<Real>>& coordinate() const { return m_coordinate; }
		void nearbyCities(std::vector<std::vector<int>> &nearby,int num_near=0, int obj=0);     //find some percent of nearby city
		void prim(std::vector<std::vector<int>>& mst_edge, int n = 0); // find MST edges
		void calculateEdgeWeight(char* edge_type);
		const std::vector<std::vector<Real>>& cost(int i = 0) const { return mvvv_cost[i]; }
		const std::vector<std::vector<int>>& nearby() const { return mvv_nearby; }
		std::string fileName() const { return m_file_name; }
		const Optima<VarVec<int>>& getOptima() const { return m_optima; }

		bool isOptimaGiven() const override;
		void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const override;
		size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const override;

	protected:
		void initialize_() override;
		void evaluate_(SolBase& s, bool effective) override;
		virtual void evaluateObjective(int* x, std::vector<Real>& obj);
		virtual void readProblem();    //read source data from file
	};

	int selectCityRandom(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row, int id_rnd);
	int selectCityRandom(std::vector<int> visited, int dim, int id_rnd);
	int selectCityGreedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row);
}

#endif // !TSP_H


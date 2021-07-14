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
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Operator adaptator of genetic learning algorithm
*
*********************************************************************************/


#ifndef OFEC_ADAPTER_H
#define OFEC_ADAPTER_H
#include <vector>
#include <memory>
#include "../../../core/definition.h"
#include <list>

namespace OFEC {
	template<typename Individual>
	class adaptor {
	protected:
	public:
		adaptor(Real alpha) : m_alpha(alpha) {};
		virtual ~adaptor() {}
		virtual void update_probability(std::vector<std::unique_ptr<Individual>>& pop, const std::vector<Real>& weight, const std::vector<int>* index = nullptr) = 0;
		virtual void update_probability(std::vector<Individual>& pop, const std::vector<Real>& weight, const std::vector<int>* index = nullptr) {};
		virtual void create_solution(std::vector<std::unique_ptr<Individual>>& pop, std::vector<Individual>& offspring) = 0;
		virtual EvalTag update_solution(std::vector<std::unique_ptr<Individual>>& pop, std::vector<Individual>& offspring, int& num_improve) = 0;
		virtual void print_infor() {}
		Real get_alpha() const { return m_alpha; }
		void set_alpha(Real alpha) { m_alpha = alpha; }
	protected:
		Real m_alpha;
	};
}

#endif //OPERATOR_ADAPTOR_H
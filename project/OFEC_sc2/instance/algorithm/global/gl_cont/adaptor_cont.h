#ifndef OFEC_ADAPTOR_CONT_H
#define OFEC_ADAPTOR_CONT_H

#include "../adaptor.h"
#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/algorithm/individual.h"
#include <algorithm>

namespace OFEC {
	class adaptor_cont :public adaptor<individual<>> {
	public:
		adaptor_cont(Real alpha, size_t num_dim, size_t size_pop);
		void update_probability(std::vector<std::unique_ptr<individual<>>>& pop, const std::vector<Real>& weight, const std::vector<int>* index = nullptr);
		void create_solution(std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring);
		EvalTag update_solution(std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring, int& num_improve);
		void update_step(std::vector<std::unique_ptr<individual<>>>& pop);
	protected:
		void accumlate_probability();
		void local_search(size_t i, std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring);
		void global_search(size_t i, std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring);
	protected:
		int m_num;
		struct Infor {
			Real val;
			std::vector<int> idx;
		};
		std::vector<std::vector<Infor> > m_proba;
		std::vector<std::vector<Real> >m_acc_proba;
		struct Limit {
			std::pair<Real, Real>boundary;
			Real step, range;
			std::vector<Real> as;
		};
		std::vector<Limit> m_limit/*, m_initialRange*/;
		std::vector<std::vector<int>> m_pos;
	};
}

#endif // !OFEC_ADAPTOR_CONT_H


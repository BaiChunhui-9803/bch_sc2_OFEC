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
* class solution is designed for any kind of user-defined solution representation.
* It takes a variable encoding and an objective value type as parameters to generate
* a solution.
*
*********************************************************************************/

#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H

#include <utility>
#include "../instance_manager.h"

namespace  OFEC {

	template<typename T = VarVec<Real>>
	class Solution : public SolBase {
	public:
		using VarEnc = T;
		virtual ~Solution() {}
		template<typename ... Args>
		Solution(size_t num_objs, size_t num_cons, Args&& ... args) :
			SolBase(num_objs, num_cons), m_var(std::forward<Args>(args)...) {}
		Solution(const Solution &rhs) = default;
		Solution(Solution &&rhs) = default;

		Solution &operator =(const Solution &rhs) = default;
		Solution &operator =(Solution &&rhs) = default;
		const Solution& solut() const noexcept { return *this; }
		Solution& solut() noexcept { return *this; }
		const VarEnc& variable() const noexcept { return m_var; }
		VarEnc& variable() noexcept { return m_var; }

		EvalTag evaluate(int id_pro, int id_alg, bool effective_eval = true) {
			GET_PRO(id_pro).evaluate(*this, effective_eval);
			if (ID_ALG_VALID(id_alg) && effective_eval)
				GET_ALG(id_alg).increaseEffectiveEvals();
			if (ID_ALG_VALID(id_alg) && ID_RND_VALID(GET_ALG(id_alg).idRecord())) {
				size_t num_evals = GET_ALG(id_alg).numEffectiveEvals();
				size_t record_freq = GET_ALG(id_alg).recordFrequency();
				if (num_evals && num_evals % record_freq == 0)
					GET_ALG(id_alg).record();
			}
			if (ID_ALG_VALID(id_alg) && GET_ALG(id_alg).isObjMinMaxMonitored())
				GET_ALG(id_alg).updateObjMinMax(m_obj);
			if (ID_ALG_VALID(id_alg) && GET_ALG(id_alg).keepCandidatesUpdated())
				GET_ALG(id_alg).updateCandidates(*this);
			return  GET_PRO(id_pro).updateEvalTag(*this, id_alg, effective_eval);
		}

		Real variableDistance(const SolBase &x, int id_pro) const override {
			return GET_PRO(id_pro).variableDistance(*this, x);
		}

		Real variableDistance(const VarBase &x, int id_pro) const override {
			return GET_PRO(id_pro).variableDistance(m_var, x);
		}

	protected:
		VarEnc m_var;
	};
}

#endif // !OFEC_SOLUTION_H

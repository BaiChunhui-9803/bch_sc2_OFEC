//Register CMA_ES "CMA-ES" ConOP,GOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created by Junchen Wang on Oct. 29, 2018.

/*Hansen, N. and A.Ostermeier(2001).
Completely Derandomized Self - Adaptation in Evolution Strategies.
Evolutionary Computation, 9(2), pp. 159 - 195;
https://github.com/CMA-ES/c-cmaes */

#ifndef OFEC_CMAES_H
#define OFEC_CMAES_H

#include "../../../../core/algorithm/algorithm.h"
#include "../../../../core/algorithm/population.h"
#include "../../../../core/algorithm/individual.h"
#include "cmaes.h"
#include <vector>

namespace OFEC {
	class cmaes_pop : public population<individual<>> {
	public:
		cmaes_pop(size_t size_pop);
		cmaes_pop(size_t size_pop, const std::vector<Real>& start_ind, Real step_size);
		void reinit_cmaest();
		void resizepop_cmaest();
		void initialize() override;
		EvalTag evolve() override;
		~cmaes_pop();
	protected:
		cmaes_t evo;
		double* arFunvals = nullptr, * const* pop = nullptr, * xfinal = nullptr;
		std::string signalsFilePathName;
		std::string initialsFilePathName;
	protected:
		void copy(double* x, std::vector<Real>& vx);
		void reverse_x(const std::vector<Real>& vx, double* x);
		void reverse_size(const std::vector<Real>& vstddev, double* stddev);
		double fit_map(Solution<>& s);
	};

	class CMA_ES : public algorithm {
	public:
		CMA_ES(param_map& v);
		void initialize() override;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		void run_() override;
	private:
		cmaes_pop  m_pop;
	};
}

#endif // !OFEC_CMAES_H


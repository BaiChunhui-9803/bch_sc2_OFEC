/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Apr 4, 2018 by Li Zhou

#ifndef EPANET_INDIVIDUAL_H
#define EPANET_INDIVIDUAL_H

#include "../../../../../core/algorithm/individual.h"

#include "../../../../problem/realworld/EPANET/epa_encoding.h"
#include "../../../../problem/realworld/EPANET/epanet.h"

#include <cmath>
#include <fstream>
#include <cstring>

namespace OFEC {

	class epanet_individual : public individual<variable_epanet, Real>
	{
	protected:
		solution_type m_pv, m_pu;
	public:
	    epanet_individual(size_t num_obj, size_t num_con) : individual(num_obj,num_con) {}
        epanet_individual(const Solution<variable_epanet,Real>& sol) : individual(sol), m_pv(sol), m_pu(sol) {}
		void mutate_first_part(const std::vector<Real> & pro);
		void mutate_second_part(Real F, const std::vector<std::vector<Real>> & pro,
			Solution<variable_epanet, Real> *r1,
			Solution<variable_epanet, Real> *r2,
			Solution<variable_epanet, Real> *r3,
			Solution<variable_epanet, Real> *r4 = 0,
			Solution<variable_epanet, Real> *r5 = 0);

		void recombine(Real CR);
		EvalTag select(bool is_stable);
		void cover_first_part(const epanet_individual & indi);
		void cover_second_part(const epanet_individual & indi);
		bool same_location(epanet_individual & indi);
		void initialize(int id);
		
		solution_type& trial();
	protected:
		void mutate_mass(Real F, Solution<variable_epanet, Real> *r1,
			Solution<variable_epanet, Real> *r2,
			Solution<variable_epanet, Real> *r3,
			Solution<variable_epanet, Real> *r4 = 0,
			Solution<variable_epanet, Real> *r5 = 0);
		void mutate_location(const std::vector<Real> & pro);
		void mutate_ST(const std::vector<Real> & pro);
		void mutate_duration(const std::vector<Real> & pro);

	public:
		Real m_distance_fitness;
		Real m_pu_distance_fitness;
		
	};


}

#endif // !EPANET_INDIVIDUAL_H



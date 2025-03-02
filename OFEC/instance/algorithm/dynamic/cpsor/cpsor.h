//Register CPSOR "CPSOR" ConOP,SOP,GOP,MMOP,DOP

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
// Created by Changhe Li
// Updated by Mai Peng at 26 September 2021 

/* ---------------------------------------------------------------------------------------
CPSOR: [1]
-----------------------------------------------------------------------------------------*/

#ifndef OFEC_CPSOR_H
#define OFEC_CPSOR_H
#include "../cpso/cpso_subswarm.h"
#include "../../../../core/algorithm/multi_population.h"
#include "../../../../core/algorithm/algorithm.h"
#include "../../../problem/continuous/dynamic/uncertianty_continuous.h"
#include "../../../record/dynamic/rcr_vec_real_dynamic.h"
#include "../../../../utility/clustering/hslh.h"
#include "../metrics_dynamic.h"
#ifdef OFEC_DEMO
#include "../dynamic_pso.h"
#endif

namespace ofec {
	class CPSOR : public MetricsDynamicConOEA
#ifdef OFEC_DEMO
		, public DynamicPSO 
#endif
	{
	protected:
		void initialize_() override;
		int initializeOriginalPop(int num);
		int createSubswarms(int num);
		int checkOverlapping();
		void checkConverging();

		void run_() override;

		void informChange(int rf);
		void measureMultiPop(bool flag);

	public:
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer();
		std::vector<bool> getPopHiberState() const override;
#endif
	protected:
		ofec::MultiPopulation<CPSOSwarm> m_sub_pop;							//slst
		int m_init_popsize;
		int m_min_subpopSize = 1;
		std::vector<std::unique_ptr<templateParticle>> m_all_indis;			//initial cradle C
		int m_max_subpopSize;
		Real m_overlapDegree;							//��
		Real m_alpha;									//��
		Real m_rconv;
	};


}
#endif //!OFEC_CPSOR_H

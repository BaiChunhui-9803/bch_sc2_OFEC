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
*
*********************************************************************************/



// Created: 9 June 2021
// Last modified:
#ifndef NOISY_H
#define NOISY_H
#include "../problem.h"

namespace OFEC {
#define CAST_UNCERTAINTY_NOISY dynamic_cast<Noisy*>(global::ms_global->m_problem.get())

	class Noisy : virtual public Problem
	{
	public:
	protected:

		// there are three sources of noisy

		bool m_flag_noisy_from_objective = false;
		Real m_obj_noisy_severity = 0.0;
		bool m_flag_noisy_from_variable = false;
		Real m_var_noisy_severity = 0.0;
		bool m_flag_noisy_from_environment = false;
		Real m_environment_noisy_severity = 0.0;
	public:

		template<typename ... Args>
		Noisy(Args&& ... args) : Problem(std::forward<Args>(args)...) {};
		virtual ~Noisy() = 0;
		Noisy& operator=(const Noisy& rhs) = default;


		void setFlagNoisyFromObjective(bool flag_noisy_obj) {
			m_flag_noisy_from_objective = flag_noisy_obj;
		}

		bool getFlagNoisyFromObjective()const {
			return m_flag_noisy_from_objective;
		}
		void setFlagNoisyFromVariable(bool flag_noisy_var) {
			m_flag_noisy_from_variable = flag_noisy_var;
		}

		bool getFlagNoisyFromVariable()const {
			return m_flag_noisy_from_variable;
		}
		void setVarNoisyServerity(Real val) {
			m_var_noisy_severity = val;
		}

		void setFlagNoisyFromEnvironment(bool flag_noisy_env) {
			m_flag_noisy_from_environment = flag_noisy_env;
		}

		bool getFlagNoisyFromEnvironment()const {
			return m_flag_noisy_from_environment;
		}

	protected:

		//void copy(const Problem& rP) {
		//	auto& dcp = dynamic_cast<const Noisy&>(rP);
		//	*this = dcp;

		//}
		void updateParameters() { 
			//if (m_flag_noise)
			//	m_params["Noise severity"] = m_noise_severity;
		//	m_params["Flag of obj noise"] = m_flag_noisy_from_objective;
		}

	};
}

#endif
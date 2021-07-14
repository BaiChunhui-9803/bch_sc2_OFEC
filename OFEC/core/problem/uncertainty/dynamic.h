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
#ifndef DYNAMIC_H
#define DYNAMIC_H
#include"../problem.h"

namespace OFEC {
#define CAST_DYN_PRO dynamic_cast<Dynamic*>(global::ms_global->m_problem.get())

	class Dynamic : virtual public Problem
	{
	public:
	protected:
		int m_frequency = 0;
		int m_period = 0;					// definite period for values repeating
		int m_counter = 0;			// counter of number of changes
		// flag=true, the number of dimensions of the problem change, otherwise no change,  default value is false
		bool m_flag_variable_memory_change = false;
		// dir=1 means increasing the memory, dir=-1 decrease it, dir=0 random dir
		int m_flag_variable_memory_dir = 0;
		bool m_flag_objective_memory_change = false;
		bool m_flag_objective_memory_dir = 0;

		bool m_flag_time_linkage = false;
		// severity of noise and time-linkage added in time-linkage enviroment, June 05 2014
		Real m_time_linkage_severity = 0.0;
		bool m_flag_trigger_time_linkage = false;

		bool m_flag_change_during_execution = false;
		
	
		//void setDimensionChange(bool rFlag) {
		//	m_flag_dimension_change = rFlag;
		//}
		//void setChangeDirction(bool rFlag) {
		//	m_direction_dimension_change = rFlag;
		//}


	public:

		template<typename ... Args>
		Dynamic(Args&& ... args) : Problem(std::forward<Args>(args)...) {};
		virtual ~Dynamic() = 0;
		Dynamic& operator=(const Dynamic& rhs) = default;




		void setFrequency(int rChangeFre) {
			m_frequency = rChangeFre;
		//	m_params["Change frequency:"] = m_frequency;
		}
		int getFrequency()const {
			return m_frequency;
		}
		void setPeriod(int rPeriod) {
		//	m_params["Period"] = m_period;
			m_period = rPeriod;
		}
		int getPeriod() const {
			return m_period;
		}
		void setCount(int counter) {
			m_counter = counter;
		}
		int getCount()const {
			return m_counter;
		}
		void setFlagVarMemChange(bool flag) {
			m_flag_variable_memory_change = flag;
		}

		bool getFlagVarMemChange()const {
			return m_flag_variable_memory_change;
		}
		void setFlagTimeLinkage(bool flag) {
			m_flag_time_linkage = flag;
		}
		bool getFlagTimeLinkage() const {
			return m_flag_time_linkage;
		}
		void setTimeLinkageSeverity(Real value) {
			m_time_linkage_severity = value;
		}
		bool& getTriggerFlagTimeLinkage() {
			return m_flag_trigger_time_linkage;
		}


		virtual void changeVarMemory() {};


		virtual void change() {
			++m_counter;
		}

	protected:

		void copy(const Problem& rP) {};
		void updateParameters();

	};
}

#endif
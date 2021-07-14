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
* class Algorithm is an abstract for all algorithms.
*
*********************************************************************************/
#ifndef OFEC_ALGORITHM_H
#define OFEC_ALGORITHM_H

#include <array>
#include <memory>

#include "termination.h"
#include "encoding.h"
#include "../../utility/typevar/typevar.h"

namespace OFEC {
	class InstanceManager;

	class Algorithm {
		friend class InstanceManager;
	public:
		Algorithm& operator=(Algorithm &&) = default;
		virtual ~Algorithm() {}

		std::string name() const { return m_name; }
		const ParamMap& getParameters()const { return m_params; }
		size_t recordFrequency() const { return m_record_frequency; }
		int idRecord() const { return m_id_rcr; }
		int idParamMap() const { return m_id_param; }
		bool solved() const { return m_solved; }
		size_t numEffectiveEvals() const { return m_effective_eval; }

		void initialize();
		void run();
		bool terminated();
		virtual bool terminating();
		virtual void record() = 0;
		Real duration();
		Termination *getTermination() { return m_term.get(); }
		void setTermination(Termination *term);

		void increaseEffectiveEvals() { m_effective_eval++; }

		bool keepCandidatesUpdated() const { return m_keep_candidates_updated; }
		void setKeepCandidatesUpdated(bool flag) { m_keep_candidates_updated = flag; }
		void updateCandidates(const SolBase &sol);
		const std::list<std::unique_ptr<SolBase>>&candidates() const { return m_candidates; }

		bool isObjMinMaxMonitored() const { return m_obj_minmax_monitored; }
		void setObjMinMaxMonitored(bool flag) { m_obj_minmax_monitored = flag; }
		void updateObjMinMax(const std::vector<Real> &obj);
		Real minObjFound(size_t i) const { return m_minmax_objective[i][0]; }
		Real maxObjFound(size_t i) const { return m_minmax_objective[i][1]; }	

	protected:
		virtual void initialize_();
		virtual void run_() = 0;
		
		std::string m_name;
		int m_id_pro;
		int m_id_rnd;
		int m_id_alg;
		int m_id_rcr;
		int m_id_param;
		std::unique_ptr<Termination> m_term;
		ParamMap m_params;
		size_t m_record_frequency;
		size_t m_effective_eval;
		bool m_solved;
		bool m_obj_minmax_monitored;
		std::vector<std::array<Real, 2>> m_minmax_objective; // the best and worst so far solutions of each objective 
		bool m_keep_candidates_updated;                    
		std::list<std::unique_ptr<SolBase>> m_candidates;    // candidate optimal solutions

	private:
		bool m_initialized = false;

#ifdef OFEC_DEMO
	public:
		virtual void updateBuffer() = 0;
		const std::vector<std::vector<const SolBase*>> &getSolution() const { return m_solution; }
	protected:
		std::vector<std::vector<const SolBase*>> m_solution;
#endif
	};

}
#endif // !OFEC_ALGORITHM_H

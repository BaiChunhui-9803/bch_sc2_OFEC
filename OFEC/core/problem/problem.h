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
* the abstract of different types of problems
*
*
*********************************************************************************/
#ifndef OFEC_PROBLEM_H
#define OFEC_PROBLEM_H
#include <string>
#include <map>
#include <memory>
#include <set>
#include <array>

#include "../algorithm/encoding.h"
#include "../../utility/typevar/typevar.h"

namespace OFEC {
	class InstanceManager;

	class Problem {
		friend class InstanceManager;
	public:
		virtual ~Problem() = default;
		void initialize();
		void evaluate(SolBase &s, bool effective_eval = true);

		/* Ready-only methods */
		const std::string &name() const { return m_name; }
		size_t numObjectives() const { return m_num_objs; }
		size_t numConstraints() const { return m_num_cons; }
		bool isInitialized() const { return m_initialized; }
		const std::set<ProTag> &tags() const { return m_tag; }
		Real objectiveAccuracy() const { return m_objective_accuracy; }
		Real variableAccuracy() const { return m_variable_accuracy; }
		Real variableNicheRadius() const { return m_variable_niche_radius; }
		const ParamMap &parameters()const { return m_params; }
		bool hasTag(ProTag tag) const { return m_tag.count(tag) > 0; }
		OptMode optMode(size_t idx) const { return m_opt_mode[idx]; }
		const std::vector<OptMode> &optMode() const { return m_opt_mode; }
		bool isEqualityConstraint(size_t i) const { return m_constraint[i] == Constraint::Equality; }
		int idParamMap() const { return m_id_param; }

		/* Write methods */
		void setTags(const std::set<ProTag> &tags) { m_tag = tags; }
		void addTag(ProTag tag) { m_tag.insert(tag); }
		void setOptMode(OptMode m, size_t idx = 0) { m_opt_mode[idx] = m; }
		void setIdRnd(int id_rnd) { m_id_rnd = id_rnd; }
		void setIdParam(int id_param) { m_id_param = id_param; }

		/* Virtual methods */
		virtual void initSolution(SolBase &s, int id_rnd) const = 0;
		virtual bool same(const SolBase &s1, const SolBase &s2) const = 0;
		virtual Real variableDistance(const SolBase &s1, const SolBase &s2) const = 0;
		virtual Real variableDistance(const VarBase &s1, const VarBase &s2) const = 0;
		virtual bool boundaryViolated(const SolBase &) const { return false; }
		virtual bool constraintViolated(const SolBase &) const { return false; }
		virtual void validateSolution(SolBase &, Validation, int) {}
		virtual void resizeObjective(size_t num_objs);
		virtual void resizeConstraint(size_t num_cons);
		virtual Real feasibleRatio() { return 1.0; }
		virtual void updateParameters();
		virtual EvalTag updateEvalTag(SolBase &s, int id_alg, bool effective_eval) { return EvalTag::Normal; }

		virtual bool isOptimaGiven() const { return false; }
		virtual void updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const = 0;
		virtual size_t numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const = 0;

	protected:
		virtual void initialize_();
		virtual void evaluate_(SolBase &s, bool effective) = 0;
		//void copy(const Problem &); // copy non-memory related parameters

	protected:
		std::string m_name;
		int m_id_pro;
		int m_id_rnd;
		int m_id_param;
		size_t m_num_objs;
		size_t m_num_cons;
		std::vector<OptMode> m_opt_mode;
		std::vector<Constraint> m_constraint;
		std::set<ProTag> m_tag;
		ParamMap m_params;		// To record the parameters of the current problem
		size_t m_total_eval;
		Real m_objective_accuracy;
		Real m_variable_accuracy;
		Real m_variable_niche_radius;  // for selecting candidate solutions

	private:
		bool m_initialized = false;
	};
}


#endif // !OFEC_PROBLEM_H

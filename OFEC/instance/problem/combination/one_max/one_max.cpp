#include "one_max.h"

namespace OFEC {
	one_max::one_max(param_map & v) : one_max(v.at("problem name"), v.at("number of variables")) {}

	one_max::one_max(const std::string & name, int size_var) : problem(name, size_var, 1) {}

	void one_max::initialize() {
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Maximization;
		m_optima.append(VarVec<int>(std::vector<int>(m_num_vars, 1)));
		m_optima.append(m_num_vars);
		m_initialized = true;
	}

	EvalTag one_max::evaluate_(solution_base & s, caller call, bool effective, bool initialized) {
		VarVec<int> &x = dynamic_cast< Solution<VarVec<int>, Real> &>(s).variable();
		std::vector<Real> &obj = dynamic_cast<Solution<VarVec<int>, Real> &>(s).objective();

		for (int i = 0; i < m_num_objs; i++)
			obj[i] = 0;
		for (int n = 0; n < m_num_objs; n++)
			for (size_t i = 0; i < m_num_vars; i++)
				obj[n] += x[i];

		if (initialized && call == caller::Algorithm) {
			if (effective)		m_effective_eval++;
			if (global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return EvalTag::Terminate;
		}
		return EvalTag::Normal;
	}

	bool one_max::is_valid(const solution_base & s) {
		if (!m_if_valid_check) return true;
        const auto &x = dynamic_cast<const Solution<VarVec<int>, Real> &>(s).variable();
		for (int i = 0; i < m_num_vars; i++) {
			if (x[i] != 0 && x[i] != 1)
				return false;
		}
		return true;
	}

	void one_max::initialize_solution(solution_base & s) const
	{
		VarVec<int> &x = dynamic_cast< Solution<VarVec<int>, Real> &>(s).variable();
		for (size_t i = 0; i < m_num_vars; i++)
			if (global::ms_global->m_uniform[caller::Problem]->next() < 0.5)
				x[i] = 0;
			else x[i] = 1;
	}
	bool one_max::same(const solution_base & s1, const solution_base & s2) const
	{
        const auto &x1 = dynamic_cast<const Solution<VarVec<int>, Real> &>(s1).variable();
        const auto &x2 = dynamic_cast<const Solution<VarVec<int>, Real> &>(s2).variable();
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	Real one_max::variable_distance(const solution_base & s1, const solution_base & s2) const
	{
        const auto &x1 = dynamic_cast<const Solution<VarVec<int>, Real> &>(s1).variable();
        const auto &x2 = dynamic_cast<const Solution<VarVec<int>, Real> &>(s2).variable();
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	Real one_max::variable_distance(const variable_base & s1, const variable_base & s2) const
	{
		const auto &x1 = dynamic_cast<const VarVec<int>&>(s1);
		const auto &x2 = dynamic_cast<const VarVec<int>&>(s2);
		Real dis = 0;
		for (int i = 0; i < m_num_vars; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

    bool one_max::is_optima_given() {
        return true;
    }
}
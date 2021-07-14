#include "MOEA_F6.h"

namespace OFEC {
	MOEA_F6::MOEA_F6(param_map & v) :
		MOEA_F6(v.at("problem name"), v.at("number of variables")) {
		
	}
	MOEA_F6::MOEA_F6(const std::string & name, size_t size_var) : problem(name, size_var, 3), \
		MOEA_FBase(name, size_var, 3) {
		
	}
	void MOEA_F6::initialize() {
		std::vector<std::pair<Real, Real>> range(m_num_vars, std::make_pair(-2, 2));
		range[0].first = 0;
		range[0].second = 1;
		range[1].first = 0;
		range[1].second = 1;
		setDomain(range);
		setInitialDomain(range);
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Minimization;
		m_dtype = 1;
		m_ptype = 31;
		m_ltype = 32;
		LoadPF();
	}
}
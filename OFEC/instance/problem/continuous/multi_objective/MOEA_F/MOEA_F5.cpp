#include "MOEA_F5.h"

namespace OFEC {
	MOEA_F5::MOEA_F5(param_map & v) : 
		MOEA_F5(v.at("problem name"), v.at("number of variables")) {
		
	}
	MOEA_F5::MOEA_F5(const std::string & name, size_t size_var) : problem(name, size_var, 2), \
		MOEA_FBase(name, size_var, 2) {
		
	}
	void MOEA_F5::initialize() {
		std::vector<std::pair<Real, Real>> range(m_num_vars, std::make_pair(-1, 1));
		range[0].first = 0;
		setDomain(range);
		setInitialDomain(range);
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Minimization;
		m_dtype = 1;
		m_ptype = 21;
		m_ltype = 26;
		LoadPF();
	}
}
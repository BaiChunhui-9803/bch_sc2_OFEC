#include "WFG5.h"

namespace OFEC {
	WFG5::WFG5(param_map & v) : WFG5(v.at("problem name"), v.at("number of variables"), v.at("numObj")){
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG5::WFG5(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG5::initialize() {
		WFG::initialize();
	}

	std::vector<Real> WFG5::t1(const std::vector<Real> &y) {
		int n = y.size();
		std::vector<Real> t;
		for (int i = 0; i < n; i++)	{
			t.push_back(s_decept(y[i], 0.35, 0.001, 0.05));
		}
		return std::move(t);
	}

	std::vector<Real> WFG5::t2(const std::vector<Real> &y) {	//as t2 from WFG4
		int n = y.size();
		std::vector<Real> w(n, 1.0);
		std::vector<Real> t;
		for (int i = 1; i <= m_num_objs - 1; i++) {
			const int head = (i - 1)*m_k / (m_num_objs - 1);
			const int tail = i * m_k / (m_num_objs - 1);
			const std::vector<Real>& y_sub = subvector(y, head, tail);
			const std::vector<Real>& w_sub = subvector(w, head, tail);
			t.push_back(r_sum(y_sub, w_sub));
		}
		const std::vector<Real>& y_sub = subvector(y, m_k, n);
		const std::vector<Real>& w_sub = subvector(w, m_k, n);
		t.push_back(r_sum(y_sub, w_sub));
		return std::move(t);
	}

	

	std::vector<Real> WFG5::shape(const std::vector<Real> &t_p) {
		int M = t_p.size();
		std::vector<Real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = concave(x, m);
		}	
		return std::move(x);
	}


}
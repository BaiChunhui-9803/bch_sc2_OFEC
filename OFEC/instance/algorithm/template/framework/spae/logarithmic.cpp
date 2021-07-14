#include "logarithmic.h"

namespace OFEC {
	RegressLog::RegressLog(const std::vector<Real> &x, const std::vector<Real> &y) : m_x(x), m_y(y) {}

	void RegressLog::initialize_() {
		Continuous::initialize_();
		resizeVariable(2);
		resizeObjective(1);
		m_opt_mode[0] = OptMode::Minimize;
		m_domain.setRange(0, 10000, 0);
		m_domain.setRange(1, 100, 1);
	}
	
	void RegressLog::evaluateObjective(Real* x, std::vector<Real>& obj) {
		obj[0] = 0;
		Real a = x[0], b = x[1], c = 1 - x[0] * x[1];
		for (size_t i = 0; i < m_x.size(); ++i) {
			obj[0] += pow(a * pow(b, m_y[i]) + c - m_x[i], 2);
		}
	}
}
#include "UF04.h"

namespace OFEC {
	UF04::UF04(param_map & v) : UF04(v.at("problem name"), v.at("number of variables")) { //param_numDim = 30 is suggested

	}
	UF04::UF04(const std::string & name, size_t size_var) : problem(name, size_var, 2), UF(name, size_var, 2) {

	}
	void UF04::initialize() {
		std::vector<std::pair<Real, Real>> r;
		r.push_back(std::make_pair(0., 1.));
		for (int i = 1; i < m_num_vars; ++i) {
			r.push_back(std::make_pair(-2., 2.));
		}
		setInitialDomain(r);
		setDomain(r);
		load_PF();
	}

	EvalTag UF04::evaluateObjective(Real *x, std::vector<Real> &obj) {
		int count1, count2;
		Real sum1, sum2, yj, hj;
		sum1 = sum2 = 0.0;
		count1 = count2 = 0;
		for (int j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - sin(6.0*OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			hj = fabs(yj) / (1.0 + exp(2.0*fabs(yj)));
			if (j % 2 == 0) {
				sum2 += hj;
				count2++;
			}
			else {
				sum1 += hj;
				count1++;
			}
		}
		if (count1 == 0)
			obj[0] = x[0];
		else
			obj[0] = x[0] + 2.0 * sum1 / (Real)count1;

		obj[1] = 1.0 - x[0] * x[0] + 2.0*sum2 / (Real)count2;
		return EvalTag::Normal;
	}

}
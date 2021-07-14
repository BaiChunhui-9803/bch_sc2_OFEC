#include "UF10.h"

namespace OFEC {
	UF10::UF10(param_map & v) : UF10(v.at("problem name"), v.at("number of variables")) { //param_numDim = 30 is suggested

	}
	UF10::UF10(const std::string & name, size_t size_var) : problem(name, size_var, 3), UF(name, size_var, 3) {

	}
	void UF10::initialize() {
		std::vector<std::pair<Real, Real>> r;
		r.push_back(std::make_pair(0., 1.));
		r.push_back(std::make_pair(0., 1.));
		for (int i = 2; i < m_num_vars; ++i) {
			r.push_back(std::make_pair(-2., 2.));
		}
		setInitialDomain(r);
		setDomain(r);
		load_PF();
	}

	EvalTag UF10::evaluateObjective(Real *x, std::vector<Real> &obj) {
		int count1, count2, count3;
		Real sum1, sum2, sum3, yj, hj;
		sum1 = sum2 = sum3 = 0.0;
		count1 = count2 = count3 = 0;
		for (int j = 2; j <= m_num_vars; j++) {
			yj = x[j - 1] - 2.0*x[1] * sin(2.0*OFEC_PI*x[0] + j * OFEC_PI / m_num_vars);
			hj = 4.0*yj*yj - cos(8.0*OFEC_PI*yj) + 1.0;
			if (j % 3 == 1)	{
				sum1 += hj;
				count1++;
			}
			else if (j % 3 == 2) {
				sum2 += hj;
				count2++;
			}
			else {
				sum3 += hj;
				count3++;
			}
		}
		if (count1 == 0)
			obj[0] = cos(0.5*OFEC_PI*x[0])*cos(0.5*OFEC_PI*x[1]);
		else
			obj[0] = cos(0.5*OFEC_PI*x[0])*cos(0.5*OFEC_PI*x[1]) + 2.0*sum1 / (Real)count1;

		if (count2 == 0)
			obj[1] = cos(0.5*OFEC_PI*x[0])*sin(0.5*OFEC_PI*x[1]);
		else
			obj[1] = cos(0.5*OFEC_PI*x[0])*sin(0.5*OFEC_PI*x[1]) + 2.0*sum2 / (Real)count2;

		if (count3 == 0)
			obj[2] = sin(0.5*OFEC_PI*x[0]);
		else
			obj[2] = sin(0.5*OFEC_PI*x[0]) + 2.0*sum3 / (Real)count3;
		return EvalTag::Normal;

	}

}
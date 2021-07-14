//Register Schwefel_2_6 "Classic_schwefel_2_6" GOP,ConOP,SOP

#ifndef OFEC_SCHWEFEL_2_6_H
#define OFEC_SCHWEFEL_2_6_H

#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {
	
	class Schwefel_2_6 : public Function {
	public:
		Schwefel_2_6(const ParamMap &v);
		Schwefel_2_6(const std::string &name, size_t num_vars);
		void initialize();
	protected:
		void load_data();
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;

		std::vector<Real> m_b;
		std::vector<std::vector<int>> m_a;
	};
	
}
#endif // !OFEC_SCHWEFEL_2_6_H

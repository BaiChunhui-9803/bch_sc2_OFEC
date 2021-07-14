//Register CEC2005_GOP_F12 "GOP_CEC2005_F12" GOP,ConOP,SOP

#ifndef OFEC_F12_SCHWEFEL_2_13_H
#define OFEC_F12_SCHWEFEL_2_13_H

#include "../../../../../core/problem/continuous/function.h"
#include <cstring>

namespace OFEC {
	namespace CEC2005 {
		class Schwefel_2_13 final: public Function {
		public:
			Schwefel_2_13(const ParamMap &v);
			Schwefel_2_13(const std::string &name, size_t num_vars);
			void initialize();
		protected:
			void load_data(const std::string & path);
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
		private:
			std::vector<std::vector<Real>> m_a;
			std::vector<std::vector<Real>> m_b;
			std::vector<Real> m_alpha;
		};
	}
	using CEC2005_GOP_F12 = CEC2005::Schwefel_2_13;
}
#endif // !OFEC_F12_SCHWEFEL_2_13_H

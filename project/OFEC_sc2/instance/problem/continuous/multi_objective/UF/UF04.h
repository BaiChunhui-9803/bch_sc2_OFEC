//Register UF04 "MOP_UF04" MOP,ConOP

#ifndef UF4_H
#define UF4_H


#include "UF.h"

namespace OFEC {
	class UF04 final : public UF {
	public:
		UF04(const ParamMap &v);
		UF04(const std::string &name, size_t size_var);
		~UF04() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif


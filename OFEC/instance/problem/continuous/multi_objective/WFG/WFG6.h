//Register WFG6 "WFG6" MOP,ConOP

#ifndef WFG6_H
#define WFG6_H

#include "WFG.h"

namespace OFEC {
	class WFG6 final : public WFG {
	public:
		WFG6(const ParamMap &v);
		WFG6(const std::string &name, size_t size_var, size_t size_obj);
		~WFG6() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif

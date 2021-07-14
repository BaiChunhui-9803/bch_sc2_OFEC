//Register WFG7 "WFG7" MOP,ConOP

#ifndef WFG7_H
#define WFG7_H

#include "WFG.h"

namespace OFEC {
	class WFG7 final : public WFG {
	public:
		WFG7(const ParamMap &v);
		WFG7(const std::string &name, size_t size_var, size_t size_obj);
		~WFG7() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif

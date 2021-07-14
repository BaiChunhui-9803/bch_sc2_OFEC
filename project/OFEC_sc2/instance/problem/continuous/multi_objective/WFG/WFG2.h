//Register WFG2 "WFG2" MOP,ConOP

#ifndef WFG2_H
#define WFG2_H

#include "WFG.h"

namespace OFEC {
	class WFG2 final : public WFG {
	public:
		WFG2(const ParamMap &v);
		WFG2(const std::string &name, size_t size_var, size_t size_obj);
		~WFG2() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif

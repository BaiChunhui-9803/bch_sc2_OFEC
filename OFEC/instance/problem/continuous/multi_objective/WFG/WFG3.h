//Register WFG3 "WFG3" MOP,ConOP

#ifndef WFG3_H
#define WFG3_H

#include "WFG.h"

namespace OFEC {
	class WFG3 final : public WFG {
	public:
		WFG3(const ParamMap &v);
		WFG3(const std::string &name, size_t size_var, size_t size_obj);
		~WFG3() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif

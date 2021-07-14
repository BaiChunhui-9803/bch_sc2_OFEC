//Register WFG1 "WFG1" MOP,ConOP

#ifndef WFG1_H
#define WFG1_H

#include "WFG.h"

namespace OFEC {
	class WFG1 final : public WFG {
	public:
		WFG1(const ParamMap &v);
		WFG1(const std::string &name, size_t size_var, size_t size_obj);
		~WFG1() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> t4(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif
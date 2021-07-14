//Register GLT1 "MOP_GLT1" MOP,ConOP

#ifndef GLT1_H
#define GLT1_H


#include "GLT.h"

namespace OFEC {
	class GLT1 final : public GLT {
	public:
		GLT1(const ParamMap &v);
		GLT1(const std::string &name, size_t size_var);
		~GLT1() {}
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
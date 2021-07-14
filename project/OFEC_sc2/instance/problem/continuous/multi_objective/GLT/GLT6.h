//Register GLT6 "MOP_GLT6" MOP,ConOP

#ifndef GLT6_H
#define GLT6_H


#include "GLT.h"

namespace OFEC {
	class GLT6 final : public GLT {
	public:
		GLT6(const ParamMap &v);
		GLT6(const std::string &name, size_t size_var);
		~GLT6() {}
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

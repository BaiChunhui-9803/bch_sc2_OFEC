//Register GLT2 "MOP_GLT2" MOP,ConOP

#ifndef GLT2_H
#define GLT2_H


#include "GLT.h"

namespace OFEC {
	class GLT2 final : public GLT {
	public:
		GLT2(const ParamMap &v);
		GLT2(const std::string &name, size_t size_var);
		~GLT2() {}
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

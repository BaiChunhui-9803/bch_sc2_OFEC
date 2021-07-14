//Register GLT3 "MOP_GLT3" MOP,ConOP

#ifndef GLT3_H
#define GLT3_H


#include "GLT.h"

namespace OFEC {
	class GLT3 final : public GLT {
	public:
		GLT3(const ParamMap &v);
		GLT3(const std::string &name, size_t size_var);
		~GLT3() {}
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
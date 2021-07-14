//Register GLT4 "MOP_GLT4" MOP,ConOP

#ifndef GLT4_H
#define GLT4_H


#include "GLT.h"

namespace OFEC {
	class GLT4 final : public GLT {
	public:
		GLT4(const ParamMap &v);
		GLT4(const std::string &name, size_t size_var);
		~GLT4() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

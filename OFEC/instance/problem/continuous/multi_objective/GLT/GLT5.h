//Register GLT5 "MOP_GLT5" MOP,ConOP

#ifndef GLT5_H
#define GLT5_H


#include "GLT.h"

namespace OFEC {
	class GLT5 final : public GLT {
	public:
		GLT5(const ParamMap &v);
		GLT5(const std::string &name, size_t size_var);
		~GLT5() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

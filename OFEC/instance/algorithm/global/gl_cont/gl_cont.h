//Register GL_cont "GLC" ConOP,GOP,MMOP,SOP

#ifndef OFEC_GL_CONT_H
#define OFEC_GL_CONT_H
#include "../GL.h"
#include "../../../../core/algorithm/individual.h"

namespace OFEC {
	class GL_cont_pop : public GL<individual<>> {
	public:
		GL_cont_pop(size_t size_pop, size_t size_var);
		void initialize();
		EvalTag evolve();
	protected:
		void initialize_curpop();
	};

	class GL_cont : public algorithm {
	public:
		GL_cont(param_map& v);
		void initialize();
		void run_();
		void record();
#ifdef OFEC_DEMO
		void updateBuffer() override {}
#endif
	protected:
		GL_cont_pop m_pop;
	};
}


#endif // !OFEC_GL_CONT_H


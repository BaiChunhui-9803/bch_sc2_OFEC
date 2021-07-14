#include "de_ind.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	IndDE::IndDE(size_t num_objs, size_t num_cons, size_t num_vars) :
		Individual<>(num_objs, num_cons, num_vars),
		m_pv(num_objs, num_cons, num_vars),
		m_pu(num_objs, num_cons, num_vars) {}

	IndDE::IndDE(const Solution<>& sol) :
		Individual<>(sol),
		m_pu(sol),
		m_pv(sol) {}

	IndDE& IndDE::operator=(const Solution<>& other) {
		Solution<>::operator=(other);
		return *this;
	}

	void IndDE::mutate(Real F,
		const Solution<>* r1,
		const Solution<>* r2,
		const Solution<>* r3,
		int id_pro,
		const Solution<>* r4,
		const Solution<>* r5) {
		Real l, u;
		for (size_t i = 0; i < m_pv.variable().size(); ++i) {
			l = GET_CONOP(id_pro).range(i).first;
			u = GET_CONOP(id_pro).range(i).second;
			m_pv.variable()[i] = (r1->variable()[i]) + F * ((r2->variable()[i]) - (r3->variable()[i]));
			if (r4 && r5) m_pv.variable()[i] += F * ((r4->variable()[i]) - (r5->variable()[i]));

			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndDE::recombine(Real CR, RecombineDE rs, int id_rnd) {
		size_t dim = m_var.size();
		if (rs == RecombineDE::Binomial) {
			size_t I = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, dim);
			for (size_t i = 0; i < dim; ++i) {
				Real p = GET_RND(id_rnd).uniform.next();
				if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
				else m_pu.variable()[i] = variable()[i];
			}
		}
		else if (rs == RecombineDE::exponential) {
			int n = GET_RND(id_rnd).uniform.nextNonStd<int>(0, dim);
			size_t L = 0;
			do {
				L = L + 1;
			} while (GET_RND(id_rnd).uniform.next() < CR && L < dim);
			for (size_t i = 0; i < dim; i++) {
				if (i < L)
					m_pu.variable()[(n + i) % dim] = m_pv.variable()[(n + i) % dim];
				else
					m_pu.variable()[(n + i) % dim] = m_var[(n + i) % dim];
			}
		}
	}

	void IndDE::mutate(Real F, const std::vector<int>& var,
		Solution<>* r1,
		Solution<>* r2,
		Solution<>* r3,
		int id_pro,
		Solution<>* r4,
		Solution<>* r5) {
		Real l, u;
		for (auto& i : var) {
			l = GET_CONOP(id_pro).range(i).first;
			u = GET_CONOP(id_pro).range(i).second;
			m_pv.variable()[i] = (r1->variable()[i]) + F * ((r2->variable()[i]) - (r3->variable()[i]));
			if (r4 && r5) m_pv.variable()[i] += F * ((r4->variable()[i]) - (r5->variable()[i]));
			if ((m_pv.variable()[i]) > u) {
				m_pv.variable()[i] = ((r1->variable()[i]) + u) / 2;
			}
			else if ((m_pv.variable()[i]) < l) {
				m_pv.variable()[i] = ((r1->variable()[i]) + l) / 2;
			}
		}
	}

	void IndDE::recombine(Real CR, const std::vector<int>& var, int I, int id_rnd) {
		for (auto& i : var) {
			Real p = GET_RND(id_rnd).uniform.next();
			if (p <= CR || i == I)     m_pu.variable()[i] = m_pv.variable()[i];
			else m_pu.variable()[i] = variable()[i];
		}
	}

	EvalTag IndDE::select(int id_pro, int id_alg) {
		EvalTag tag = m_pu.evaluate(id_pro, id_alg);
		if (m_pu.dominate(*this, GET_PRO(id_pro).optMode())) {
			m_var = m_pu.variable();
			m_obj = m_pu.objective();
			m_con = m_pu.constraint();
			m_improved = true;
		}
		else {
			m_improved = false;
		}
		return tag;
	}

	Solution<>& IndDE::trial() {
		return m_pu;
	}
}

#include "NSGAII_SBX.h"
#include "../../../../../core/problem/continuous/continuous.h"

#ifdef OFEC_DEMO
#include "../../../../../../ui/buffer/continuous/buffer_cont_MOP.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace OFEC {
	NSGAII_SBX_pop::NSGAII_SBX_pop(size_t size_pop) : SBX_pop<>(size_pop) {}

	void NSGAII_SBX_pop::initialize()	{
		SBX_pop::initialize();
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
	}

	EvalTag NSGAII_SBX_pop::evolve() {
		if (this->m_inds.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAII_SBXRealMu::evolve()");
		EvalTag tag = EvalTag::Normal;
		for (size_t i = 0; i < this->m_inds.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = tournament_selection();
			do { p[1] = tournament_selection(); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_offspring[i], m_offspring[i + 1]);
			mutate(m_offspring[i]);
			mutate(m_offspring[i + 1]);
		}
		for (auto& i : m_offspring) {
			tag = i.evaluate();
			if (tag != EvalTag::Normal) break;
		}  
		for (size_t i = 0; i < this->m_inds.size(); ++i)  m_offspring[i+ this->m_inds.size()] = *this->m_inds[i];
		survivor_selection(this->m_inds, m_offspring);
		m_iter++;
		return tag;
	}

	NSGAII_SBX::NSGAII_SBX(param_map & v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {}

	void NSGAII_SBX::initialize() {
		m_pop.set_cr(0.9);
		m_pop.set_mr(1.0 / CONTINUOUS_CAST->variable_size());
		m_pop.set_eta(20, 20);
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont_MOP*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	void NSGAII_SBX::run_() {
		while (!terminating()) {
			m_pop.evolve();
#ifdef OFEC_DEMO
			vector<vector<Solution<>*>> pops(1);
			for (size_t i = 0; i < m_pop.size(); ++i)
				pops[0].emplace_back(&m_pop[i].solut());
			dynamic_cast<Demo::buffer_cont_MOP*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
		}
	}

	void NSGAII_SBX::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		Real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}
}
#include "RingPSO.h"

namespace OFEC {
	void ring_particle::next_velocity(const Solution<> *lbest, Real w, Real c1, Real c2) {
		for (size_t j = 0; j < m_var.size(); j++) {
			m_vel[j] = w * (m_vel[j] + 
				c1 * global::ms_global->m_uniform[caller::Algorithm]->next() * (m_pbest.variable()[j] - m_var[j]) + 
				c2 * global::ms_global->m_uniform[caller::Algorithm]->next() * (lbest->variable()[j] - m_var[j]));
		}
	}

	ring_swarm::ring_swarm(size_t size_pop) : swarm(size_pop) {}

	R2_swarm::R2_swarm(size_t size_pop) : ring_swarm(size_pop) {}

	R3_swarm::R3_swarm(size_t size_pop) : ring_swarm(size_pop) {}

	LHC_R2_swarm::LHC_R2_swarm(size_t size_pop) : ring_swarm(size_pop) {}

	LHC_R3_swarm::LHC_R3_swarm(size_t size_pop) : ring_swarm(size_pop) {}

	void R2_swarm::set_neighborhood() {
		if (m_is_neighbor_set) return;
		size_t size_pop = m_inds.size();
		for (size_t i = 0; i < size_pop; ++i) {
			m_link[i][i] = true;
			m_link[i][(i + 1) % size_pop] = true;
		}
		m_is_neighbor_set = true;
	}

	void R3_swarm::set_neighborhood() {
		if (m_is_neighbor_set) return;
		size_t size_pop = m_inds.size();
		for (size_t i = 0; i < size_pop; ++i) {
			m_link[i][(i - 1) % size_pop] = true;
			m_link[i][i] = true;
			m_link[i][(i + 1) % size_pop] = true;
		}
		m_is_neighbor_set = true;
	}

	void LHC_R2_swarm::set_neighborhood() {
		if (m_is_neighbor_set) return;
		size_t size_pop = m_inds.size();
		for (size_t i = 0; i < size_pop; ++i) {
			for (size_t j = 0; j < 2 && (i / 2 * 2 + j) < size_pop; ++j)
				m_link[i][(i / 2 * 2 + j)] = true;
		}
		m_is_neighbor_set = true;
	}

	void LHC_R3_swarm::set_neighborhood() {
		if (m_is_neighbor_set) return;
		size_t size_pop = m_inds.size();
		for (size_t i = 0; i < size_pop; ++i) {
			for (size_t j = 0; j < 3 && (i / 3 * 3 + j) < size_pop; ++j)
				m_link[i][(i / 3 * 3 + j)] = true;
		}
		m_is_neighbor_set = true;
	}
}
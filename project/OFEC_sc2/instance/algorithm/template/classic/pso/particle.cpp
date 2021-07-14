#include "particle.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/instance_manager.h"

namespace OFEC {
	Particle::Particle(size_t num_obj, size_t num_con, size_t size_var) :
	        Individual(num_obj, num_con, size_var),
	        m_pbest(num_obj, num_con, size_var),
	        m_vel(size_var) {}

	Particle::Particle(const Solution<>& rhs) : 
		Individual(rhs), 
		m_pbest(rhs), 
		m_vel(rhs.variable().size()) {}

	Particle& Particle::operator=(const Solution<>& other) {
		Solution<>::operator=(other);
		m_pbest = other;
		return *this;
	}

	Real Particle::speed() const {
		Real ve = 0;
		for (size_t i = 0; i<m_var.size(); i++)
			ve += m_vel[i] * m_vel[i];
		return sqrt(ve);
	}

	void Particle::initialize(int id, int id_pro, int id_rnd) {
		Individual::initialize(id, id_pro, id_rnd);
		m_pbest = solut();
		initVelocity(id_pro, id_rnd);
	}

	void Particle::initVelocity(int id_pro, int id_rnd) {
		for (size_t i = 0; i<m_var.size(); i++) {
			auto& range = GET_CONOP(id_pro).range(i);
			m_vel[i] = GET_RND(id_rnd).uniform.nextNonStd(range.first, range.second);
		}
	}

	void Particle::initVelocity(const std::vector<std::pair<Real, Real>> &vrange, int id_rnd) {
		for (size_t i = 0; i<m_var.size(); i++) {	
			m_vel[i] = GET_RND(id_rnd).uniform.nextNonStd(vrange[i].first, vrange[i].second);
		}
	}

	void Particle::initVelocity(Real min, Real max, int id_rnd) {
		for (int i = 0; i < m_var.size(); i++) {
			m_vel[i] = GET_RND(id_rnd).uniform.nextNonStd(min, max);
		}
	}

	void Particle::nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd) {
		for (size_t j = 0; j<m_var.size(); j++) {
			m_vel[j] = w * m_vel[j]
				+ c1 * GET_RND(id_rnd).uniform.next() * (m_pbest.variable()[j] - m_var[j])
				+ c2 * GET_RND(id_rnd).uniform.next() * (lbest->variable()[j] - m_var[j]);
		}
	}

	void Particle::clampVelocity(int id_pro) {
		for (size_t j = 0; j<m_var.size(); j++) {	
			auto& range = GET_CONOP(id_pro).range(j);
			if (m_var[j]>range.second) {
				m_var[j] = range.second;
				m_vel[j] = 0;
			}
			else if (m_var[j]<range.first) {
				m_var[j] = range.first;
				m_vel[j] = 0;
			}
		}
	}
	
	void Particle::move() {
		for (size_t j = 0; j<m_var.size(); j++) {			
			m_var[j] += m_vel[j];
		}
	}
	
	void Particle::resizeVar(size_t n) {
		m_var.resize(n);
		m_pbest.variable().resize(n);
		m_vel.resize(n);
	}
}

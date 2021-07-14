#include "encoding.h"
#include "../instance_manager.h"
#include "../../utility/functional.h"

namespace OFEC {
	SolBase::SolBase(size_t num_objs, size_t num_cons) : 
		m_obj(num_objs), m_con(num_cons) {}

	void SolBase::initialize(int id_pro, int id_rnd) {
		GET_PRO(id_pro).initSolution(*this, id_rnd);
	}

	Dominance SolBase::compare(const SolBase &s, int id_pro) const {
		return objectiveCompare(m_obj, s.m_obj, GET_PRO(id_pro).optMode());
	}

	bool SolBase::dominate(const std::vector<Real> &o, int id_pro) const {
		return  Dominance::Dominant == objectiveCompare(m_obj, o, GET_PRO(id_pro).optMode());
	}

	bool SolBase::dominate(const SolBase &s, int id_pro) const {
		return Dominance::Dominant == objectiveCompare(m_obj, s.m_obj, GET_PRO(id_pro).optMode());
	}

	Dominance SolBase::compare(const SolBase &s, const std::vector<OptMode> &mode) const {
		return objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::dominate(const std::vector<Real> &o, const std::vector<OptMode> &mode) const {
		return  Dominance::Dominant == objectiveCompare(m_obj, o, mode);
	}

	bool SolBase::dominate(const SolBase &s, const std::vector<OptMode> &mode) const {
		return Dominance::Dominant == objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::dominEqual(const std::vector<Real> &o, const std::vector<OptMode> &mode) const { //this soluton weakly donimates objective o
		Dominance r = objectiveCompare(m_obj, o, mode);
		return r == Dominance::Dominant || r == Dominance::Equal;
	}

	bool SolBase::dominEqual(const SolBase &s, const std::vector<OptMode> &mode) const {//this solution weakly donimates solution s
		Dominance r = objectiveCompare(m_obj, s.m_obj, mode);
		return r == Dominance::Dominant || r == Dominance::Equal;
	}

	bool SolBase::nonDominated(const std::vector<Real> &o, const std::vector<OptMode> &mode) const { //two solutions non-donimate with each other
		return  Dominance::NonDominated == objectiveCompare(m_obj, o, mode);
	}

	bool SolBase::nonDominated(const SolBase &s, const std::vector<OptMode> &mode) const {//two solutions non-donimate with each other
		return  Dominance::NonDominated == objectiveCompare(m_obj, s.m_obj, mode);
	}

	bool SolBase::equal(const SolBase &rhs, const std::vector<OptMode> &mode) const {
		return Dominance::Equal == objectiveCompare(m_obj, rhs.m_obj, mode);
	}

	bool SolBase::same(const SolBase &x, int id_pro) const {//two solutions in decision space
		return GET_PRO(id_pro).same(*this, x);
	}

	Real SolBase::objectiveDistance(const std::vector<Real> &rhs) const {
		return euclidean_distance(m_obj.begin(), m_obj.end(), rhs.begin());
	}

	Real SolBase::objectiveDistance(const SolBase &rhs) const {
		return euclidean_distance(m_obj.begin(), m_obj.end(), rhs.m_obj.begin());
	}

	bool SolBase::boundaryViolated(int id_pro) const {
		return GET_PRO(id_pro).boundaryViolated(*this);
	}

	bool SolBase::constraintViolated(int id_pro) const {
		return GET_PRO(id_pro).constraintViolated(*this);
	}

	size_t SolBase::numViolation() const {
		size_t count = 0;
		for (Real i : m_con)
			if (i > 0) ++count;
		return count;
	}

	void SolBase::setObjInfinite(int id_pro) {
		for (int i = 0; i < m_obj.size(); ++i)
			m_obj[i] = (GET_PRO(id_pro).optMode(i) == OptMode::Minimize) ? std::numeric_limits<Real>::max() : std::numeric_limits<Real>::min();

	}
}
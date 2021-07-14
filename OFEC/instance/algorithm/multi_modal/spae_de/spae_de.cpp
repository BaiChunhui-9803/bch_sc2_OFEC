#include "spae_de.h"

namespace OFEC {
	void SPAE_DE::initialize_() {
		ContSPAE::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_f = v.count("scaling factor") > 0 ? std::get<Real>(v.at("scaling factor")) : 0.5;
		m_cr = v.count("crossover rate") > 0 ? std::get<Real>(v.at("crossover rate")) : 0.6;
		m_ms = v.count("mutation strategy") > 0 ?
			static_cast<MutationDE>(std::get<int>(v.at("mutation strategy"))) : MutationDE::rand_1;
	}

	void SPAE_DE::initSubPop(PopDE<> &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg) {
		ContSPAE::initSubPop(pop, id_atr_bsn, hlc, id_pro, id_alg);
		pop.F() = m_f;
		pop.CR() = m_cr;
		pop.mutationStrategy() = m_ms;
	}

	void SPAE_DE::evolveSubPop(PopDE<> &pop) {
		pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t i = 0; i < pop.size(); ++i)
			m_hlc->inputSample(pop[i].trial());
	}

	void SPAE_DE::setPopType() {
		switch (m_ms)
		{
		case OFEC::MutationDE::rand_1:
			m_pop_type = "DE-rand-1";
			break;
		case OFEC::MutationDE::best_1:
			m_pop_type = "DE-best-1";
			break;
		case OFEC::MutationDE::target_to_best_1:
			m_pop_type = "DE-target-to-best-1";
			break;
		case OFEC::MutationDE::best_2:
			m_pop_type = "DE-best-2";
			break;
		case OFEC::MutationDE::rand_2:
			m_pop_type = "DE-rand-2";
			break;
		case OFEC::MutationDE::rand_to_best_1:
			m_pop_type = "DE-rand-to-best-1";
			break;
		case OFEC::MutationDE::target_to_rand_1:
			m_pop_type = "DE-target-to-rand-1";
			break;
		case OFEC::MutationDE::nrand_1:
			m_pop_type = "DE-nrand-1";
			break;
		default:
			break;
		}
	}

	//bool SPAE_DE::isPopConverged(PopDE<> &pop) {
	//	pop.updateBest(m_id_pro);
	//	if (pop.iteration() - pop.timeBestUpdated() > 20)
	//		return true;
	//	else
	//		return false;
	//	//pop.updateWorst(m_id_pro);
	//	//if (pop.best().front()->objectiveDistance(*pop.worst().front()) < 1e-6)
	//	//	return true;
	//	//else
	//	//	return false;
	//}
}
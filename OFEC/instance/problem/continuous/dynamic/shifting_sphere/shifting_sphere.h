//Register ShfitingSphere "Shifting-Sphere" ConOP,DOP,SOP

#include "../../global/classical/sphere.h"

namespace OFEC {
	class ShfitingSphere : public Sphere {
	public:
		void initialize_() override;
	protected:
		EvalTag updateEvalTag(SolBase &s, int id_alg, bool effective_eval) override;
		size_t m_change_freq;
	};
}

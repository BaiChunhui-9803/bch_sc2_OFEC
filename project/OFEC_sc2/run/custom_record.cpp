#include "custom_method.h"
#include "../core/global.h"
#include "../instance/record/rcr_vec_real.h"

namespace OFEC {
	Record* getNewRecord(const ParamMap& param) {
		auto record = new RecordVecReal(param);
		return record;
	}
}
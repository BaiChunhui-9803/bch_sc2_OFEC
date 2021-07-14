#include "typevar.h"

namespace OFEC {
	std::ostream &operator<<(std::ostream &os, const TypeVar &v) {
		std::visit([&os](auto &&arg) { os << arg; }, v);
		return os;
	}
}
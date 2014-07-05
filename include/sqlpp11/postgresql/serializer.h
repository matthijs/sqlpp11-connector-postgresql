#ifndef SQLPP_POSTGRESQL_INTERPRETER_H
#define SQLPP_POSTGRESQL_INTERPRETER_H

#include <sqlpp11/interpreter.h>
#include <sqlpp11/wrap_operand.h>
#include <sqlpp11/parameter.h>

namespace sqlpp {

	template<typename ValueType, typename NameType>
		struct serializer_t<postgresql::context_t, parameter_t<ValueType, NameType>> {
			using T = parameter_t<ValueType, NameType>;

			static postgresql::context_t &_(const T &t, postgresql::context_t &context) {
				context << "$" << context.count();
				context.pop_count();
				return context;
			}
		};
}

#endif

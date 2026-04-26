#ifndef REPL_TYPES_HPP
#define REPL_TYPES_HPP

#include <string_view>

namespace Types
{
	constexpr uint8_t InvalidTypeIDX = 0;

	enum class TypeKind
	{
		Invalid,
		Opaque,
		Uint08, Uint16, Uint32, Uint64,
		Sint08, Sint16, Sint32, Sint64,
		Real08, Real16, Real32, Real64,
		String,
		Char08,
		Binary,
		NilPtr,

		Count
	};

	bool GetTypeKind(std::string_view aView, uint8_t& arOutResult);
}

#endif // REPL_TYPES_HPP
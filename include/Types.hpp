#ifndef REPL_TYPES_HPP
#define REPL_TYPES_HPP

#include <string_view>
#include <string>

typedef std::monostate	opaque;
typedef uint8_t 		uint08;
typedef uint16_t 		uint16;
typedef uint32_t 		uint32;
typedef uint64_t 		uint64;
typedef int8_t 			sint08;
typedef int16_t 		sint16;
typedef int32_t 		sint32;
typedef int64_t 		sint64;
//typedef float 		real08;
//typedef float 		real16;
typedef float 			real32;
typedef double 			real64;
typedef std::string		string;
typedef char			char08;
typedef bool			binary;
typedef nullptr_t		nilptr;

namespace Types
{
	constexpr uint8_t UnspecifiedTypeIDX = 0;

	enum class TypeKind : uint8_t
	{
		Unspecified = 0,
		Opaque,
		Uint08, Uint16, Uint32, Uint64,
		Sint08, Sint16, Sint32, Sint64,
		Real08, Real16, Real32, Real64,
		String, Char08,
		Binary,
		NilPtr,

		Count
	};

	bool GetTypeKind(std::string_view aView, uint8_t& arOutResult);
}

#endif // REPL_TYPES_HPP
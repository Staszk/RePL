#include "Types.hpp"
#include <string_view>
#include <algorithm>
#include <array>
#include <utility>
#include <iostream>

namespace Types
{
	using TypeEntry = std::pair<std::string_view, TypeKind>;
	using namespace std::literals::string_view_literals;

	constexpr std::array<TypeEntry, static_cast<size_t>(TypeKind::Count)> TypeLiterals =
	{{
		{ "opaque"sv, TypeKind::Opaque },
		{ "uint08"sv, TypeKind::Uint08 },
		{ "uint16"sv, TypeKind::Uint16 },
		{ "uint32"sv, TypeKind::Uint32 },
		{ "uint64"sv, TypeKind::Uint64 },
		{ "sint08"sv, TypeKind::Sint08 },
		{ "sint16"sv, TypeKind::Sint16 },
		{ "sint32"sv, TypeKind::Sint32 },
		{ "sint64"sv, TypeKind::Sint64 },
		{ "real08"sv, TypeKind::Real08 },
		{ "real16"sv, TypeKind::Real16 },
		{ "real32"sv, TypeKind::Real32 },
		{ "real64"sv, TypeKind::Real64 },
		{ "string"sv, TypeKind::String },
		{ "char08"sv, TypeKind::Char08 },
		{ "binary"sv, TypeKind::Binary },
		{ "nilptr"sv, TypeKind::NilPtr },
	}};

	/**
	 * @brief Check if a string value is a reserved type.
	 *
	 * @param aValue The string value to check.
	 * @param arOutResult OUT: The kind of Type, if valid. Otherwise untouched.
	 * @return True if the value is a type, false otherwise.
	 */
	bool GetTypeKind(std::string_view aView, uint8_t &arOutResult)
	{
		bool found = false;
		auto it = std::ranges::find(TypeLiterals, aView, &TypeEntry::first);
		if (it != TypeLiterals.end())
		{
			arOutResult = std::to_underlying(it->second);
			found = true;
		}
		return found;
	}
}
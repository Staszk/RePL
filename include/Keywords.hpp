#ifndef REPL_KEYWORDS_HPP
#define REPL_KEYWORDS_HPP

#include <cstdint>
#include <string_view>

namespace Keywords
{
	enum class KeywordKind : uint8_t
	{
		Invalid,
		// Special Case,
		LastResult,
		// Control Flow
		If, Else, ElseIf, While, Until, For, Continue, Break,
		// Truth-y
		True, False,
		// Built-Ins
		Print,

		Count,
	};

	bool GetKeywordKind(std::string_view aValue, uint8_t& arOutResult);
}

#endif
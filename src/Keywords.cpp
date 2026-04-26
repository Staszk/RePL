#include "Keywords.hpp"
#include <string_view>
#include <algorithm>
#include <array>
#include <utility>
#include <iostream>

namespace Keywords
{
	using KeywordEntry = std::pair<std::string_view, KeywordKind>;
	using namespace std::literals::string_view_literals;

	constexpr std::array<KeywordEntry, static_cast<size_t>(KeywordKind::Count)> KeywordLiterals =
	{{
		{ "_"sv, KeywordKind::LastResult },
		{ "if"sv, KeywordKind::If },
		{ "else"sv, KeywordKind::Else },
		{ "else if"sv, KeywordKind::ElseIf },
		{ "while"sv, KeywordKind::While },
		{ "until"sv, KeywordKind::Until },
		{ "for"sv, KeywordKind::For },
		{ "continue"sv, KeywordKind::Continue },
		{ "break"sv, KeywordKind::Break },
		{ "true"sv, KeywordKind::True },
		{ "false"sv, KeywordKind::False },
		{ "print"sv, KeywordKind::Print },
	}};

	/**
	 * @brief Check if a string value is a reserved keyword.
	 *
	 * @param aValue The string value to check.
	 * @param arOutResult OUT: The type of Keyword, if valid. Otherwise untouched.
	 * @return True if the value is a keyword, false otherwise.
	 */
	bool GetKeywordKind(std::string_view aValue, uint8_t& arOutResult)
	{
		bool found = false;
		auto it = std::ranges::find(KeywordLiterals, aValue, &KeywordEntry::first);
		if (it != KeywordLiterals.end())
		{
			arOutResult = std::to_underlying(it->second);
			found = true;
		}
		return found;
	}
}
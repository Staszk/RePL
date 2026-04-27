#include "Environment.hpp"
#include "Token.hpp"
#include "Types.hpp"
#include <iostream>

namespace
{
	template<typename T>
	concept Numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

	template<typename T>
	constexpr T type_max = std::numeric_limits<T>::max();

	template<typename T>
	constexpr T type_min = std::numeric_limits<T>::min();

	struct TypeEnforcer
	{
		Types::TypeKind Kind;

		InterpreterValue operator()(opaque) const { return InterpreterValue(opaque{}); }

		// Convert Integer Numerics
		template <Numeric T>
		InterpreterValue operator()(T aNumericValue) const 
		{
			switch (Kind)
			{
				using enum Types::TypeKind;

			case Unspecified:
				return InterpreterValue(aNumericValue);

			case Opaque:
				return InterpreterValue(opaque{});

			case Uint08:
				if (aNumericValue < type_min<uint08> || aNumericValue > type_max<uint08>) 
					throw std::runtime_error("Value out of range for uint08");
				return InterpreterValue(static_cast<uint08>(aNumericValue));
			case Uint16:
				if (aNumericValue < type_min<uint16> || aNumericValue > type_max<uint16>)
					throw std::runtime_error("Value out of range for uint16");
				return InterpreterValue(static_cast<uint16>(aNumericValue));
			case Uint32:
				if (aNumericValue < type_min<uint32> || aNumericValue > type_max<uint32>)
					throw std::runtime_error("Value out of range for uint32");
				return InterpreterValue(static_cast<uint32>(aNumericValue));
			case Uint64:
				if (aNumericValue < type_min<uint64> || aNumericValue > type_max<uint64>)
					throw std::runtime_error("Value out of range for uint64");
				return InterpreterValue(static_cast<uint64_t>(aNumericValue));

			case Sint08:
				if (aNumericValue < std::numeric_limits<int8_t>::min() || aNumericValue > std::numeric_limits<int8_t>::max()) 
					throw std::runtime_error("Value out of range for sint08");
				return InterpreterValue(static_cast<int8_t>(aNumericValue));
			case Sint16:
				if (aNumericValue < std::numeric_limits<uint16_t>::min() || aNumericValue > std::numeric_limits<int16_t>::max())
					throw std::runtime_error("Value out of range for sint16");
				return InterpreterValue(static_cast<int16_t>(aNumericValue));
			case Sint32:
				if (aNumericValue < std::numeric_limits<int32_t>::min() || aNumericValue > std::numeric_limits<int32_t>::max()) 
					throw std::runtime_error("Value out of range for sint32");
				return InterpreterValue(static_cast<int32_t>(aNumericValue));
			case Sint64:
				if (aNumericValue < std::numeric_limits<int64_t>::min() || aNumericValue > std::numeric_limits<int64_t>::max())
					throw std::runtime_error("Value out of range for sint64");
				return InterpreterValue(static_cast<int64_t>(aNumericValue));

			case Real08:
			case Real16:
			case Real32:
				if (aNumericValue < std::numeric_limits<float>::min() || aNumericValue > std::numeric_limits<float>::max()) 
					throw std::runtime_error("Value out of range for real32");
				return InterpreterValue(static_cast<float>(aNumericValue));
			case Real64:
				if (aNumericValue < std::numeric_limits<double>::min() || aNumericValue > std::numeric_limits<double>::max()) 
					throw std::runtime_error("Value out of range for real64");
				return InterpreterValue(static_cast<double>(aNumericValue));

			case String:
				return InterpreterValue(std::to_string(aNumericValue));
			case Char08:
				if (aNumericValue < std::numeric_limits<char>::min() || aNumericValue > std::numeric_limits<char>::max()) 
					throw std::runtime_error("Value out of range for char08");
				return InterpreterValue(static_cast<char>(aNumericValue));

			case Binary:
				return InterpreterValue(aNumericValue > 0 ? true : false);

			case NilPtr:
				InterpreterValue(nullptr_t{});
			};

			return InterpreterValue(std::monostate{});
		}

		InterpreterValue operator()(bool aBool) const
		{
			return this->operator()(static_cast<uint32_t>(aBool));
		}

		InterpreterValue operator()(const std::string& aString) const
		{
			switch (Kind)
			{
				using enum Types::TypeKind;
			case String:
				return InterpreterValue(aString);
			case Char08:
				if (aString.empty()) return InterpreterValue('\0');
				else return InterpreterValue(aString[0]);
			default:
				real64 value;
				std::errc ec{};
				if (aString == "true") value = real64{1};
				else if (aString == "false") value = real64{0};
				else
				{
					auto [ptr, ec] = std::from_chars(aString.data(), aString.data() + aString.size(), value);
				}
				return this->operator()(value);
			}
		}

		InterpreterValue operator()(auto autoVal) const
		{
			return InterpreterValue(autoVal);
		}
	};
}

InterpreterValue Environment::Get(const Token& arIdentifier)
{
	std::string key(arIdentifier.Value);
	if (EnvironmentSymbols.contains(key))
	{
		return EnvironmentSymbols.at(key).Value;
	}

	return Symbol{}.Value;
}

void Environment::Define(std::string_view aView, Types::TypeKind aType, bool abIsConst, InterpreterValue aValue)
{
	try
	{
		InterpreterValue enforcedValue = std::visit(TypeEnforcer{aType}, aValue);
		EnvironmentSymbols[std::string(aView)] = { aType, enforcedValue, abIsConst };
	}
	catch (const std::runtime_error& e) 
	{
        throw std::runtime_error("Declaration Error: " + std::string(e.what()));
    }
}

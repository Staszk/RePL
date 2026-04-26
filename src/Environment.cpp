#include "Environment.hpp"
#include "Token.hpp"
#include "Types.hpp"
#include <iostream>

namespace
{
	template<typename T>
	concept Numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

	struct TypeEnforcer
	{
		Types::TypeKind Kind;

		// Convert Integer Numerics
		template <Numeric T>
		InterpreterValue operator()(T val) const 
		{
			switch (Kind)
			{
				using enum Types::TypeKind;
			case Opaque:
				return InterpreterValue(std::monostate{});

			case Uint08:
				if (val < std::numeric_limits<uint8_t>::min() || val > std::numeric_limits<uint8_t>::max()) 
					throw std::runtime_error("Value out of range for uint08");
				return InterpreterValue(static_cast<uint8_t>(val));
			case Uint16:
				if (val < std::numeric_limits<uint16_t>::min() || val > std::numeric_limits<uint16_t>::max())
					throw std::runtime_error("Value out of range for uint16");
				return InterpreterValue(static_cast<uint16_t>(val));
			case Uint32:
				if (val < std::numeric_limits<uint32_t>::min() || val > std::numeric_limits<uint32_t>::max()) 
					throw std::runtime_error("Value out of range for uint32");
				return InterpreterValue(static_cast<uint32_t>(val));
			case Uint64:
				if (val < std::numeric_limits<uint64_t>::min() || val > std::numeric_limits<uint64_t>::max())
					throw std::runtime_error("Value out of range for uint64");
				return InterpreterValue(static_cast<uint64_t>(val));

			case Sint08:
				if (val < std::numeric_limits<int8_t>::min() || val > std::numeric_limits<int8_t>::max()) 
					throw std::runtime_error("Value out of range for sint08");
				return InterpreterValue(static_cast<int8_t>(val));
			case Sint16:
				if (val < std::numeric_limits<uint16_t>::min() || val > std::numeric_limits<int16_t>::max())
					throw std::runtime_error("Value out of range for sint16");
				return InterpreterValue(static_cast<int16_t>(val));
			case Sint32:
				if (val < std::numeric_limits<int32_t>::min() || val > std::numeric_limits<int32_t>::max()) 
					throw std::runtime_error("Value out of range for sint32");
				return InterpreterValue(static_cast<int32_t>(val));
			case Sint64:
				if (val < std::numeric_limits<int64_t>::min() || val > std::numeric_limits<int64_t>::max())
					throw std::runtime_error("Value out of range for sint64");
				return InterpreterValue(static_cast<int64_t>(val));

			case Real08:
			case Real16:
			case Real32:
				if (val < std::numeric_limits<float>::min() || val > std::numeric_limits<float>::max()) 
					throw std::runtime_error("Value out of range for real32");
				return InterpreterValue(static_cast<float>(val));
			case Real64:
				if (val < std::numeric_limits<double>::min() || val > std::numeric_limits<double>::max()) 
					throw std::runtime_error("Value out of range for real64");
				return InterpreterValue(static_cast<double>(val));

			case String:
				return InterpreterValue(std::to_string(val));
			case Char08:
				if (val < std::numeric_limits<char>::min() || val > std::numeric_limits<char>::max()) 
					throw std::runtime_error("Value out of range for char08");
				return InterpreterValue(static_cast<char>(val));

			case Binary:
				return InterpreterValue(val > 0 ? true : false);

			case NilPtr:
				InterpreterValue(nullptr_t{});
			};


			return InterpreterValue(std::monostate{});
		}

		InterpreterValue operator()(auto val) const
		{
			return InterpreterValue(val);
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

void Environment::Define(std::string_view aView, Types::TypeKind aType, InterpreterValue aValue)
{
	try
	{
		InterpreterValue enforcedValue = std::visit(TypeEnforcer{aType}, aValue);
		EnvironmentSymbols[std::string(aView)] = { aType, enforcedValue, false };
	}
	catch (const std::runtime_error& e) 
	{
        throw std::runtime_error("Declaration Error: " + std::string(e.what()));
    }
}

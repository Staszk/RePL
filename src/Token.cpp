#include "Token.hpp"
#include <format>
#include <iostream>
#include <assert.h>

namespace TokenHelpers
{
    /**
    * @brief Format a TokenLocation struct into a human-readable text representation.
    *
    * @param loc The location to format.
    * @return The formatted location string.
    */
    std::string LocationToText(const TokenLocation& loc)
    {
        return std::format("Line {}, Column {}", loc.Line + 1, loc.Col + 1);
    }

    /**
     * @brief Convert a TokenKind enum value into a human-readable text label.
     *
     * @param aKind The token kind to convert.
     * @return The token kind text label.
     */
    constexpr std::string_view TokenKindToText(TokenKind aKind)
    {
        switch (aKind)
        {
        using enum TokenKind;
        // Special Cases
        case Invalid:           return "Invalid";
        case End:               return "End";

        // Whitespaces
        case Whitespace:        return "Whitespace";
        case NewLine:           return "NewLine";
        case Tab:               return "Tab";

        // Comments
        case LineComment:       return "LineComment";
        case BlockComment:      return "BlockComment";

        // Identifiers
        case Identifier:        return "Identifier";
        
        // Literals
        case KeywordLiteral:    return "KeywordLiteral";
        case IntLiteral:        return "IntLiteral";
        case FloatLiteral:      return "FloatLiteral";
        case HalfFloatLiteral:  return "HalfFloatLiteral";
        case StringLiteral:     return "StringLiteral";
        case CharLiteral:       return "CharLiteral";

        // Preprocessor
        case Preprocessor:      return "Preprocessor";

        // Multi-Character Literals
        // Operators
        case PlusEqual:         return "PlusEqual";
        case MinusEqual:        return "MinusEqual";
        case Increment:         return "Increment";
        case Decrement:         return "Decrement";
        case AsteriskEqual:     return "AsteriskEqual";
        case SlashEqual:        return "SlashEqual";
        case PercentEqual:      return "PercentEqual";
        case EqualEqual:      return "EqualsEquals";
        case NotEqual:         return "NotEquals";
        case LogicalAnd:        return "LogicalAnd";
        case LogicalOr:         return "LogicalOr";
        case LesserEqual:         return "LessEqual";
        case GreaterEqual:      return "GreaterEqual";

        // Single Character Literals
        // Operators
        case Plus:              return "Plus";
        case Minus:             return "Minus";
        case Asterisk:          return "Asterisk";
        case Slash:             return "Slash";
        case Percent:           return "Percent";
        case Ampersand:         return "Ampersand";
        case Pipe:              return "Pipe";
        case Caret:             return "Caret";
        case Tilde:             return "Tilde";
        case Bang:              return "Bang";
        case Equal:             return "Equal";
        case Question:          return "Question";
        case Lesser:              return "Less";
        case Greater:           return "Greater";
        /// Delimiters
        case Semicolon:         return "Semicolon";
        case Colon:             return "Colon";
        case Comma:             return "Comma";
        case Period:            return "Period";
        /// Scopes and Grouping
        case OpenParen:         return "OpenParen";
        case CloseParen:        return "CloseParen";
        case OpenCurly:         return "OpenCurly";
        case CloseCurly:        return "CloseCurly";
        case OpenBracket:       return "OpenBracket";
        case CloseBracket:      return "CloseBracket";
        }

        assert(false && "Invalid TokenKind");
        return "";
    }

    /**
     * @brief Format a token value for display, escaping whitespace characters.
     *
     * @param arView The token value to format.
     * @return The escaped display string view.
     */
    std::string_view TokenValueToText(const std::string_view arView)
    {
        std::string_view v{ arView };

        if (arView.size() == 1)
        {
            if (arView.at(0) == '\n')
            {
                v = "\\n";
            }
            else if (arView.at(0) == '\t')
            {
                v = "\\t";
            }
            else if (arView.at(0) == ' ')
            {
                v = "' '";
            }
        }

        return v;
    }

    /**
     * @brief Serialize a Token to a text representation for debugging output.
     *
     * @param aToken The token to serialize.
     * @return The formatted token string.
     */
    std::string TokenToText(const Token& aToken)
    {
        return std::format("{} : {} : {{{}, {}}}", TokenKindToText(aToken.Kind),
            TokenValueToText(aToken.Value), aToken.Loc.Line + 1, aToken.Loc.Col + 1);
    }
}
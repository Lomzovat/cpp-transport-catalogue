#include "json.h"

#include <iterator>

using namespace std::literals;

namespace json {

    Node LoadNode(std::istream& input);
    Node LoadString(std::istream& input);

    std::string LoadLiteral(std::istream& input) {
        std::string str;
        while (std::isalpha(input.peek())) {
            str.push_back(static_cast<char>(input.get()));
        }
        return str;
    }

    Node LoadArray(std::istream& input) {
        std::vector<Node> result;

        for (char ch; input >> ch && ch != ']';) {
            if (ch != ',') {
                input.putback(ch);
            }
            result.push_back(LoadNode(input));
        }
        if (!input) {
            throw ParsingError("Array parsing error"s);
        }
        return Node(result);
    }

    Node LoadDict(std::istream& input) {
        Dict dict;

        for (char c; input >> c && c != '}';) {
            if (c == '"') {
                std::string key = LoadString(input).AsString();
                if (input >> c && c == ':') {
                    if (dict.find(key) != dict.end()) {
                        throw ParsingError("Duplicate key '"s + key + "' have been found");
                    }
                    dict.emplace(std::move(key), LoadNode(input));
                }
                else {
                    throw ParsingError(": is expected but '"s + c + "' has been found"s);
                }
            }
            else if (c != ',') {
                throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
            }
        }
        if (!input) {
            throw ParsingError("Dictionary parsing error"s);
        }
        return Node(std::move(dict));
    }

    Node LoadString(std::istream& input) {
        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char ch = *it;
            if (ch == '"') {
                ++it;
                break;
            }
            else if (ch == '\\') {
                ++it;
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char escaped_char = *(it);
                switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            }
            else if (ch == '\n' || ch == '\r') {
                throw ParsingError("Unexpected end of line"s);
            }
            else {
                s.push_back(ch);
            }
            ++it;
        }

        return Node(std::move(s));
    }

    Node LoadBool(std::istream& input) {
        const auto s = LoadLiteral(input);
        if (s == "true"sv) {
            return Node{ true };
        }
        else if (s == "false"sv) {
            return Node{ false };
        }
        else {
            throw ParsingError("Failed to parse '"s + s + "' as bool"s);
        }
    }

    Node LoadNull(std::istream& input) {
        if (auto literal = LoadLiteral(input); literal == "null"sv) {
            return Node{ nullptr };
        }
        else {
            throw ParsingError("Failed to parse '"s + literal + "' as null"s);
        }
    }

    Node LoadNumber(std::istream& input) {
        std::string parsed_num;

        // ��������� � parsed_num ��������� ������ �� input
        auto read_char = [&parsed_num, &input] {
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError("Failed to read number from stream"s);
            }
        };

        // ��������� ���� ��� ����� ���� � parsed_num �� input
        auto read_digits = [&input, read_char] {
            if (!std::isdigit(input.peek())) {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
        };

        if (input.peek() == '-') {
            read_char();
        }
        // ������ ����� ����� �����
        if (input.peek() == '0') {
            read_char();
            // ����� 0 � JSON �� ����� ���� ������ �����
        }
        else {
            read_digits();
        }

        bool is_int = true;
        // ������ ������� ����� �����
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        // ������ ���������������� ����� �����
        if (int ch = input.peek(); ch == 'e' || ch == 'E') {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {
                // ������� ������� ������������� ������ � int
                try {
                    return std::stoi(parsed_num);
                }
                catch (...) {
                    // � ������ �������, ��������, ��� ������������
                    // ��� ���� ��������� ������������� ������ � double
                }
            }
            return std::stod(parsed_num);
        }
        catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadNode(std::istream& input) {
        char c;
        if (!(input >> c)) {
            throw ParsingError("Unexpected EOF"s);
        }
        switch (c) {
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case '"':
            return LoadString(input);
        case 't':
            // ������� [[fallthrough]] (�����������) ������ �� ������, � ��������
            // ���������� ����������� � ��������, ��� ����� ����������� ���� ���������
            // ��������� ������� � ���������� ��������� ����� case, � �� �������� �����
            // �������� break, return ��� throw.
            // � ������ ������, �������� t ��� f, ��������� � ������� ��������
            // ��������� true ���� false
            [[fallthrough]];
        case 'f':
            input.putback(c);
            return LoadBool(input);
        case 'n':
            input.putback(c);
            return LoadNull(input);
        default:
            input.putback(c);
            return LoadNumber(input);
        }
    }

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    void PrintNode(const Node& value, const PrintContext& ctx);

    template <typename Value>
    void PrintValue(const Value& value, const PrintContext& ctx) {
        ctx.out << value;
    }

    void PrintString(const std::string& value, std::ostream& out) {
        out.put('"');
        for (const char c : value) {
            switch (c) {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '"':
                // ������� " � \ ��������� ��� \" ��� \\, ��������������
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(c);
                break;
            }
        }
        out.put('"');
    }

    template <>
    void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
        PrintString(value, ctx.out);
    }

    template <>
    void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
        ctx.out << "null"sv;
    }

    // � ������������ ������� PrintValue ��� ���� bool �������� value ���������
    // �� ����������� ������, ��� � � �������� �������.
    // � �������� ������������ ����� ������������ ����������:
    // void PrintValue(bool value, const PrintContext& ctx);
    template <>
    void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
        ctx.out << (value ? "true"sv : "false"sv);
    }

    template <>
    void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
        std::ostream& out = ctx.out;
        out << "[\n"sv;
        bool first = true;
        auto inner_ctx = ctx.Indented();
        for (const Node& node : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }
            inner_ctx.PrintIndent();
            PrintNode(node, inner_ctx);
        }
        out.put('\n');
        ctx.PrintIndent();
        out.put(']');
    }

    template <>
    void PrintValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
        std::ostream& out = ctx.out;
        out << "{\n"sv;
        bool first = true;
        auto inner_ctx = ctx.Indented();
        for (const auto& [key, node] : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }
            inner_ctx.PrintIndent();
            PrintString(key, ctx.out);
            out << ": "sv;
            PrintNode(node, inner_ctx);
        }
        out.put('\n');
        ctx.PrintIndent();
        out.put('}');
    }

    void PrintNode(const Node& node, const PrintContext& ctx) {
        std::visit(
            [&ctx](const auto& value) {
                PrintValue(value, ctx);
            },
            node.GetValue());
    }



    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), PrintContext{ output });
    }

}  // namespace json
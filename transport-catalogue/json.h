#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

using namespace std::string_view_literals;


namespace json {

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;
        Node(std::nullptr_t);
        Node(Array array);
        Node(Dict dict);
        Node(bool value);
        Node(int value);
        Node(double value);
        Node(std::string value);

        const Array& AsArray() const;
        const Dict& AsDict() const;
        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;

        bool IsNull() const;
        bool IsArray() const;
        bool IsDict() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsDouble() const; // Возвращает true, если в Node хранится int либо double.
        bool IsPureDouble() const; // Возвращает true, если в Node хранится double.
        bool IsString() const;

        const Value& GetValue() const;

    private:
        Value value_;
    };

    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);
        const Node& GetRoot() const;

    private:
        Node root_;
    };

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
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
            return { out,
                    indent_step,
                    indent_step + indent };
        }
    };

    Document Load(std::istream& input);
    void Print(const Document& document, std::ostream& output);

}  // end namespace json

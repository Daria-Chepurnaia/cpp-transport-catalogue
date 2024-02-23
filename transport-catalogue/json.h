#pragma once

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace json {

using Number = std::variant<int, double>; 
    
class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка выбрасывается при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node : private std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict> {
public:
    using variant::variant;   
 
    Node(Number value);
    
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const; 
    bool IsNull() const;
   
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    
    bool operator==(const Node& rhs) const {
        return GetValue() == rhs.GetValue();
    }
    
    bool operator!=(const Node& rhs) const {
        return GetValue() != rhs.GetValue();
    }    
    
    const std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>& GetValue() const { return *this; }
};

class Document {
public:
    Document(Node root);

    const Node& GetRoot() const;
    
    bool operator == (const Document& doc_other) const
    {
        return root_ == doc_other.root_;
    }
    
    bool operator != (const Document& doc_other) const
    {
        return root_ != doc_other.root_;
    }

private:
    Node root_;
};

Document Load(std::istream& input);
    
void PrintValue(const int& value, std::ostream& out);  
void PrintValue(const double& value, std::ostream& out);
void PrintValue(const std::nullptr_t, std::ostream& out);
void PrintValue(const bool value, std::ostream& out);
void PrintValue(const std::string value, std::ostream& out);
void PrintValue(Dict map, std::ostream& out);
void PrintValue(const Array array, std::ostream& out);
    
void PrintNode(const Node& node, std::ostream& out);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
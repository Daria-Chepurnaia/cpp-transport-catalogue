#include "json.h"

using namespace std;

namespace json {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("Array parsing error");
        }
        const char ch = *it;
        
        if (ch == ' ' || ch =='\t' || ch == '\n' || ch == '\r') {
            ++it;
            continue;
        } else if (ch == ']') {
            // Встретили закрывающую скобку
            ++it;
            break;
        }  else if (ch == ',')  {
            ++it;
            if (it == end) {            
                throw ParsingError("Array parsing error");
            } else {                
                result.push_back(LoadNode(input));
                
            }
        } else {            
            result.push_back(LoadNode(input));
            
        }   
    }
    return Node(move(result));    
}    

Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
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
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
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
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
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
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node (move(s));
}

Node LoadDict(istream& input) {
    Dict result;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    string key;
    
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("Dict parsing error");
        }         
        const char ch = *it;
        if (ch == ' ' || ch =='\t' || ch == '\n' || ch == '\r') {
            ++it;
            continue;
        } else if (ch == '}') {
            // Встретили закрывающую скобку
            ++it;
            break;
        }  else if (ch =='"') {
            // Встретили кавычку ключа
            ++it;
            if (it == end) {
            // Поток закончился сразу после кавычки
                throw ParsingError("Dict parsing error");
            }
            key = LoadString(input).AsString();
            while (*it != ':') {
                if (it == end) {
                    throw ParsingError("Dict parsing error");
                }
                ++it;
            }
            //дошли до двоеточия
            ++it; 
            if (it == end) {
                throw ParsingError("Dict parsing error");
            }
            result.insert({move(key), LoadNode(input)});
        } else {
           ++it; 
        }        
    }
    return Node(move(result));
}
    
Node LoadNull(istream& input) {
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    
    string result;
    
    for (int i = 0; i < 3; ++i) {        
        if (it == end) {           
            throw ParsingError("Null parsing error1");
        }        
        result.push_back(*it);
        ++it;
    }
    if ((it == end || *it == ']' || *it == '}' || *it == ' ' || *it == '\r' || *it == '\n' || *it == '\t' || *it == ',')
        && result == "ull"s) {
            return Node(nullptr);  
    } else {
        throw ParsingError("Unexpected value");
    }    
}
    
Node LoadBool(istream& input, char first_letter) {
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    
    string result;
    result += first_letter;   
    int count_of_cycles = first_letter == 'f' ? 4 : 3;  
    
    for (int i = 0; i < count_of_cycles; ++i) {
        if (it == end) {           
            throw ParsingError("Bool parsing error");
        }
        result.push_back(*it);
        ++it;
    }
    if (it == end || *it == ']' || *it == '}' || *it == ' ' || *it == '\r' || *it == '\n' || *it == '\t' || *it == ',') {
        if (result == "false"s) {
            return Node(false);
        }
        if (result == "true"s){
            return Node(true);
        }
    } else {
        throw ParsingError("Unexpected value");
    }
    return Node(true);
}

Node LoadNode(istream& input) {
    char c;
    input.get(c);
    while (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
        input.get(c);
        continue;
    }
    
    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 'n') {
        return LoadNull(input);
    } else if (c == 't') {
        return LoadBool(input, 't'); 
    } else if (c == 'f') {
        return LoadBool(input, 'f');    
    } else {
        input.putback(c);
        Number num = LoadNumber(input);
        return Node(num);
    }
}

Node::Node(Array array)
    : node(move(array)) {
}

Node::Node(Dict map)
    : node(move(map)) {
}

Node::Node(Number value) {
    if (holds_alternative<double>(value)) {
        node = std::get<double>(value);
    } else {
        node = std::get<int>(value);
    }   
}

Node::Node(string value)
    : node(move(value)) {
}
    
Node::Node(int value)
    : node(value) {
}
    
Node::Node(double value)
    : node(value) {
}
    
Node::Node(nullptr_t)
    : node(nullptr) {
}

Node::Node(bool value)
    :node(value) {
}
    
    bool Node::IsInt() const {
        if (holds_alternative<int>(node)) {        
            return true;
        }
        return false;
    }
    bool Node::IsDouble() const {
        if (holds_alternative<int>(node) || holds_alternative<double>(node)) {        
            return true;
        }
        return false;
    }
    bool Node::IsPureDouble() const {
        if (holds_alternative<double>(node)) {        
            return true;
        }
        return false;
    }
    bool Node::IsBool() const {
        if (holds_alternative<bool>(node)) {        
            return true;
        }
        return false;    
    }
    bool Node::IsString() const {
        if (holds_alternative<std::string>(node)) {        
            return true;
        }
        return false;    
    }
    bool Node::IsArray() const {
        if (holds_alternative<Array>(node)) {        
            return true;
        }
        return false;    
    }
    bool Node::IsMap() const {
        if (holds_alternative<Dict>(node)) {        
            return true;
        }
        return false;    
    }
    
    bool Node::IsNull() const {
        if (holds_alternative<std::nullptr_t>(node)) {        
            return true;
        }
        return false;    
    }
    
    int Node::AsInt() const{
        if (IsInt()) {
            return std::get<int>(node);
        } else {
            throw std::logic_error(""s);
        }
    }
    bool Node::AsBool() const {
        if (IsBool()) {
            return std::get<bool>(node);
        } else {
            throw std::logic_error(""s);
        }
    }
    double Node::AsDouble() const {
        if (IsInt()) {
            return (double)std::get<int>(node);
        } else if (IsPureDouble()) {
            return std::get<double>(node);
        } else {
            throw std::logic_error(""s);
        }
    }
    const std::string& Node::AsString() const {
        if (IsString()) {
            return std::get<string>(node);
        } else {
            throw std::logic_error(""s);
        }
    }
    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(node);
        } else {
            throw std::logic_error(""s);
        }
    }
    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return std::get<Dict>(node);
        } else {
            throw std::logic_error(""s);
        }
    }

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}    

void PrintValue(const int& value, std::ostream& out) {
    out << value;
}
    
void PrintValue(const double& value, std::ostream& out) {
    out << value;
}

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(const std::nullptr_t, std::ostream& out) {
    out << "null"sv;
}

void PrintValue(const bool value, std::ostream& out) {
    if (value == true) {
        out << "true"sv;
    } else {
        out << "false"sv;
    }    
}

void PrintValue(const std::string value, std::ostream& out) {
    out << "\""s;
    size_t size = value.size();
    for (size_t i = 0; i < size; ++i) {
        switch (value[i]) {
            case '\r':
                out << '\\';                
                out << 'r';                
                break;
            case '\t':
                out << '\\';                
                out << 't';               
                break;
            case '\n':
                out << '\\';                
                out << 'n';                
                break;
            case '"':
                out << '\\';
                out << '"';                
                break;
            case '\\':
                out << '\\';
                out << '\\';
                break;  
            default:
                out << value[i];  
        }  
    }    
    out << "\""s;
}

void PrintValue(Dict map, std::ostream& out) {
    out << "{"s;
    bool is_first = true;
    for (const auto& [key, value] : map) {
        if (is_first) {
            is_first = false;
        } else {
            out << ", "s;
        }
        PrintValue(key, out);
        out << ": "s; 
        PrintNode(value, out);        
    }
    out << "}"s;
}

void PrintValue(const Array array, std::ostream& out) {
    out << "["s;
    bool is_first = true;
    for (const auto& node : array) {
        if (!is_first) {
            out << ", "s;
        } else {
            is_first = false;
        }
        PrintNode(node, out);
    }
    out << "]"s;
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}

void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), output);    
}

}  // namespace json
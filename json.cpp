#include "json.h"
namespace json {
    using namespace std;
         ////////////////////////////////////////////////////////////////////////////////////////////
         //                                                                                        //
         //                                                                                        //
         //                         ПСЕВДОНИМЫ ТИПОВ  И ОБЬЯВЛЕНИЯ                                 //
         //                                                                                        //
         //                                                                                        //
         ////////////////////////////////////////////////////////////////////////////////////////////
   
    using Number = std::variant<int, double>;
    Number DigitParser(std::istream& input);
    Node LoadNode(istream& input);
    std::string StringParser(std::istream& input);
    #define GAP3 "   "
    #define GAP4 "    "
    #define GAP5 "     "
    #define GAP6 "      "
    #define GAP7 "       "
      ////////////////////////////////////////////////////////////////////////////////////////////
      //                                                                                        //
      //                                                                                        //
      //                                     NODE                                               //
      //                                                                                        //
      //                                                                                        //
      ////////////////////////////////////////////////////////////////////////////////////////////

     ////////////////////////////////////////////////////////////////////////////////////////////
     //                          СРАВНЕНИЕ ТИПОВ                                               //
     ////////////////////////////////////////////////////////////////////////////////////////////

    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }
    bool Node::IsDouble() const {
        if (std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this))return true;
        else return 0;
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);

    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }
    bool Node::IsNull() const {
        return std::holds_alternative<nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }
 
    ////////////////////////////////////////////////////////////////////////////////////////////
    //                          ВЕРНУТЬ В КАЧ-ВЕ..                                            //
    ////////////////////////////////////////////////////////////////////////////////////////////

    int Node::AsInt() const {
        if (!IsInt())throw std::logic_error("non-valid type");
        return std::get<int>(*this);
    }
    double Node::AsDouble() const {
        using namespace std::literals;
        if (!IsDouble()) {
            throw std::logic_error("Not a double"s);
        }
        return IsPureDouble() ? std::get<double>(*this) : AsInt();
    }
    double Node::AsPureDouble() const {
        if (!IsPureDouble())throw std::logic_error("non-valid type");
        return  std::get<double>(*this);
    }
    bool Node::AsBool() const {
        if (!IsBool())throw std::logic_error("non-valid type");
        return  std::get<bool>(*this);
    }

    const std::string& Node::AsString() const {
        if (!IsString())throw std::logic_error("non-valid type");
        return   std::get<std::string>(*this);
    }

    const Array& Node::AsArray() const {
        if (!IsArray())throw std::logic_error("non-valid type");
        return   std::get<Array>(*this);
    }
    const Dict& Node::AsMap() const {
        if (!IsMap())throw std::logic_error("non-valid type");
        return  std::get<Dict>(*this);
    }

    const std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict>& Node::GetValue() const {
        return *this;
    }

   

    
    ///////////////////////////////////////////////////////////
     //неконстантые версии для move

    
    std::string& Node::AsString()  {
        if (!IsString())throw std::logic_error("non-valid type");
        return   std::get<std::string>(*this);
     }

     Array& Node::AsArray() {
        if (!IsArray())throw std::logic_error("non-valid type");
        return   std::get<Array>(*this);
     }
     Dict& Node::AsMap() {
        if (!IsMap())throw std::logic_error("non-valid type");
        return  std::get<Dict>(*this);
     }

     ValueObject& Node::GetValue(){
        return *this;
     }
   

    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                                                                                        //
    //                         DOCUMENT                                                       //
    //                                                                                        //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////

    Document::Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Node& Document::GetRoot() { return root_;}
    
    bool Document::operator ==(const Document& rhs) {
        return this->GetRoot() == rhs.GetRoot();
    }
    bool Document::operator!=(const Document& rhs) {
        return !(*this == rhs);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                                                                                        //
    //                                     NODE_VISITOR                                       //
    //                                                                                        //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    void NodeVisiter::operator()(std::nullptr_t) const {
        out << "null";
    }
    void NodeVisiter::operator()(bool v)const {
        out << std::boolalpha << v;
    }
    void NodeVisiter::operator()(double v)const {
        out << v;
    }
    void NodeVisiter::operator()(int v) const {
        out << v;
    }

    void NodeVisiter::operator()(const std::string& v) const {
        out << "\""sv;
        for (char c : v) {
            switch (c) {
            case'\\':
                out << "\\\\"sv;
                break;

            case'"':
                out << "\\\""sv;
                break;

            case'\n':
                out << "\\n"sv;
                break;

            case'\r':
                out << "\\r"sv;
                break;

            case'\t':
                out << "\\t"sv;
                break;

            default:
                out << c;
                break;
            }
        }
        out << "\""sv;
    }

    void NodeVisiter::operator()(const Array& v)const {
        out <<std::endl<<GAP5<< "[" << std::endl;
        for (size_t i = 0; i < v.size(); ++i) {
            out << GAP7;
            std::visit(NodeVisiter{ out }, v[i].GetValue());
            if (i + 1 == v.size()) { break; }
            out << "," << std::endl;
        }
        out << "\n" << GAP5<<"]" << std::endl;
    }
    void NodeVisiter::operator()(const Dict& v)const {

        size_t lim = v.size();
        out <<endl<< "{"sv << std::endl;
        for (const auto& i : v) {
            out <<GAP4<< "\"" << i.first << "\": "; std::visit(NodeVisiter{ out }, i.second.GetValue());
            --lim; if (lim == 0)break;
            out << "," << std::endl;

        }
        out <<std::endl <<"}" << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
  
     ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    //                                                                                        //
    //                         LOAD                                                           //
    //                                                                                        //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////

   Node LoadArray(istream& input) {
        Array result;
        for (char c; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }
        if (!input)throw ParsingError("Bad stream"s);
        return Node(move(result));
   }

    Node LoadDigit(istream& input) {
        auto result = DigitParser(input);
        if (std::holds_alternative<double>(result)) {
            return Node(get<double>(result));
        }
        return Node(get<int>(result));
    }

    Node LoadString(istream& input) {
        string line = StringParser(input);
        return Node(move(line));
    }

    Node LoadDict(istream& input) {
        Dict result;
        for (char c; input >> c && c != '}';) {
            if (c == ',') {
                input >> c;
            }
            string key = LoadString(input).AsString();
            input >> c;
            result.insert({ move(key), LoadNode(input) });
            if (!input)throw ParsingError("Bad stream"s);
        }
     //   if (result.empty())throw ParsingError("Bad stream"s);
        return Node(move(result));
    }

    Node  LoadBoolorNull(std::istream& input) {
        string line;
        while (std::isalpha(input.peek())) {
            line.push_back(static_cast<char>(input.get()));
        }
        if (line == "null") { 
            return Node(nullptr); 
        }
        if (line == "false") { 
            return Node(false); 
        }
        if (line == "true") {
            return Node(true); 
        }
        throw ParsingError("Incorrect symb"s);
    }

    Node LoadNode(istream& input) {
        char c;
        input >> c;

        switch (c) {

        case '[':
            return LoadArray(input);
            break;

        case '{':
            return LoadDict(input);
            break;

        case '"':
            return LoadString(input);
            break;

        case 't':
            input.putback(c);
            return  LoadBoolorNull(input);
        case 'f':
            input.putback(c);
            return  LoadBoolorNull(input);
        case 'n':
            input.putback(c);
            return LoadBoolorNull(input);
            break;

        default:
            input.putback(c);
            return LoadDigit(input);
            break;
        }
    }
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                                        //
  //                                                                                        //
  //                         HELP FOO                                                       //
  //                                                                                        //
  //                                                                                        //
  ////////////////////////////////////////////////////////////////////////////////////////////
    ostream& operator<<(ostream& os, const Node& nod) {
        std::visit(NodeVisiter{ os }, nod.GetValue());
        return os;
    }

    ostream& operator<<(ostream& os, const Document& nod) {
        std::visit(NodeVisiter{ os }, nod.GetRoot().GetValue());
        return os;
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(NodeVisiter{ output }, doc.GetRoot().GetValue());
    }
    
    bool operator==(const Document& doc, const Node& nod) {
        return doc.GetRoot().GetValue() == nod.GetValue();
    }
    bool operator==(const Node& nod, const Document& doc) {
        return doc.GetRoot().GetValue() == nod.GetValue();
    }
    

    Document Load(std::istream& input) {
        
        return Document{ LoadNode(input) };
    }
    
    Number DigitParser(std::istream& input) {
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
        }
        else {
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
                }
                catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return std::stod(parsed_num);
        }
        catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    // Считывает содержимое строкового литерала JSON-документа
    // Функцию следует использовать после считывания открывающего символа ":
    std::string StringParser(std::istream& input) {
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
            }
            else if (ch == '\\') {
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
            }
            else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            }
            else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }
        return s;
    }
}







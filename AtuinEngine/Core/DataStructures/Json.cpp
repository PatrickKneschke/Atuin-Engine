
#include "Json.h"
#include "Core/Util/StringFormat.h"

#include <cmath>


namespace Atuin {


Json::Json() {}


Json::Json(const Json &other) {
    
    switch (other.mData.index())
    {
        case (Size)JsonType::STRING:
            mData = new std::string( *(get< std::string* >(other.mData)) );
            break;
        case (Size)JsonType::LIST:
            mData = new JsonList( *(get< JsonList* >(other.mData)) );
            break;
        case (Size)JsonType::DICT:
            mData = new JsonDict( *(get< JsonDict* >(other.mData)) );
            break;

        default:
            mData = other.mData;
            break;
    }
}


Json::Json(Json &&other) : mData(std::move(other.mData)) { other.mData = std::monostate{}; }


Json& Json::operator= (const Json &rhs) {

    Clear();
    switch (rhs.mData.index())
    {
        case (Size)JsonType::STRING:
            mData = new std::string( *(get< std::string* >(rhs.mData)) );
            break;
        case (Size)JsonType::LIST:
            mData = new JsonList( *(get< JsonList* >(rhs.mData)) );
            break;
        case (Size)JsonType::DICT:
            mData = new JsonDict( *(get< JsonDict* >(rhs.mData)) );
            break;

        default:
            mData = rhs.mData;
            break;
    }

    return *this;
}


Json& Json::operator= (Json &&rhs) {

    Clear();
    mData = std::move(rhs.mData);
    rhs.mData = std::monostate{};

    return *this;
}


Json::~Json() {

    Clear();
}


void Json::Clear() {

    switch (mData.index())
    {
        case (Size)JsonType::STRING:
            delete std::get< std::string* >(mData);
            break;
        case (Size)JsonType::LIST:
            delete std::get< JsonList* >(mData);
            break;
        case (Size)JsonType::DICT:
            delete std::get< JsonDict* >(mData);
            break;

        default:
            break;
    }

    mData = std::monostate{};
}


Size Json::GetSize() const {

    if (mData.index() == (Size)JsonType::LIST)
    {
        return std::get< (Size)JsonType::LIST >(mData)->GetSize();
    }

    if (mData.index() == (Size)JsonType::DICT)
    {
        return std::get< (Size)JsonType::DICT >(mData)->GetSize();
    }

    return 0;
}
    
    
bool Json::HasKey(const std::string &key) const {

    if (mData.index() == (Size)JsonType::DICT)
    {
        auto mp = std::get< (Size)JsonType::DICT >(mData);
        return mp->Find(key) != mp->End();
    }

    return false;
}

    
Json& Json::operator[] (const std::string &key) {

    if (mData.index() != (Size)JsonType::DICT)
    {
        throw std::runtime_error("Cannot access key in non-dictionary type json object.");
    }

    return std::get< (Size)JsonType::DICT >(mData)->operator[](key.data());
}


Json& Json::At(const std::string &key) {

    if (mData.index() != (Size)JsonType::DICT)
    {
        throw std::runtime_error("Cannot access key in non-dictionary type json object.");
    }

    return std::get< (Size)JsonType::DICT >(mData)->At(key.data());
}


const Json& Json::At(const std::string &key) const {

    if (mData.index() != (Size)JsonType::DICT)
    {
        throw std::runtime_error("Cannot access key in non-dictionary type json object.");
    }

    return std::get< (Size)JsonType::DICT >(mData)->At(key.data());
}

    
Json& Json::operator[] (Size idx) {

    if (mData.index() != (Size)JsonType::LIST)
    {
        throw std::runtime_error("Cannot access index in non-list type json object.");
    }

    return std::get< (Size)JsonType::LIST >(mData)->operator[](idx);
}

    
const Json& Json::operator[] (Size idx) const {

    if (mData.index() != (Size)JsonType::LIST)
    {
        throw std::runtime_error("Cannot access index in non-list type json object.");
    }

    return std::get< (Size)JsonType::LIST >(mData)->operator[](idx);
}


bool Json::ToBool() const {

    if (mData.index() != (Size)JsonType::BOOL )
    {
        throw std::runtime_error("Tried to extract bool value from non-bool json object.");
    }

    return std::get< (Size)JsonType::BOOL >(mData);
}


I64 Json::ToInt() const {

    if (mData.index() != (Size)JsonType::INT )
    {
        throw std::runtime_error("Tried to extract int value from non-int json object.");
    }

    return std::get< (Size)JsonType::INT >(mData);
}


double Json::ToFloat() const {

    if (mData.index() != (Size)JsonType::FLOAT )
    {
        throw std::runtime_error("Tried to extract floating point value from non-floating point json object.");
    }

    return std::get< (Size)JsonType::FLOAT >(mData);
}


std::string Json::ToString() const {

    if (mData.index() != (Size)JsonType::STRING )
    {
        throw std::runtime_error("Tried to extract string value from non-string json object");
    }

    return *(std::get< (Size)JsonType::STRING >(mData));
}


const Json::JsonList& Json::GetList() const {

    if (mData.index() != (Size)JsonType::LIST )
    {
        throw std::runtime_error("Tried to get json list from non-list json object");
    }

    return *std::get< (Size)JsonType::LIST >(mData);
}


const Json::JsonDict& Json::GetDict() const {

    if (mData.index() != (Size)JsonType::DICT )
    {
        throw std::runtime_error("Tried to get dictionary from non-dictionary json object");
    }

    return *std::get< (Size)JsonType::DICT >(mData);
}


std::string Json::Print(Size depth, std::string tab) const {

    std::string pad = "";

    for (Size i = 0; i < depth; ++i)
    {
        pad += tab;
    }
    switch( mData.index() ) 
    {
        case (Size)JsonType::EMPTY:
            return "null";

        case (Size)JsonType::DICT: 
        {
            std::string s = "{\n";
            bool first = true;
            const JsonDict &mp = *std::get< (Size)JsonType::DICT >(mData);
            for (auto &[key, val] : mp ) 
            {
                if( !first ) {

                    s += ",\n";
                }
                s += ( pad + "\"" + key + "\" : " + val.Print(depth + 1, tab) );
                first = false;
            }
            s += ( "\n" + pad.erase(0, 4) + "}" ) ;
            return s;
        }
        case (Size)JsonType::LIST: 
        {
            std::string s = "[";
            bool first = true;
            const JsonList &list = *std::get< (Size)JsonType::LIST >(mData);
            for( auto &val : list ) 
            {
                if( !first ) 
                {                    
                    s += ", ";
                }
                s += val.Print(depth + 1, tab);
                first = false;
            }
            s += "]";
            return s;
        }
        case (Size)JsonType::STRING:
            return "\"" + JsonEscape( *std::get< (Size)JsonType::STRING >(mData) ) + "\"";

        case (Size)JsonType::FLOAT:
            return std::to_string( std::get< (Size)JsonType::FLOAT >(mData) );

        case (Size)JsonType::INT:
            return std::to_string( std::get< (Size)JsonType::INT >(mData) );

        case (Size)JsonType::BOOL:
            return std::get< (Size)JsonType::BOOL >(mData) ? "true" : "false";

        default:
            return "";
    }

    return "";
}


std::ostream& operator<<( std::ostream &os, const Json &json ) {

    os << json.Print();

    return os;
}


Json Json::Load(std::string_view str) {

    Size offset = 0;
    return Parse(str, offset);
}


void Json::SkipWhiteSpace(std::string_view str, Size &offset) {

    while ( isspace(str[offset]) )
    {
        ++offset;
    }
}


std::string Json::JsonEscape(const std::string &in) {
    
    std::string out;
    for( auto c : in )
    {
        switch( c ) 
        {
            case '\"': 
                out += "\\\""; 
                break;
            case '\\': 
                out += "\\\\"; 
                break;
            case '\b': 
                out += "\\b"; 
                break;
            case '\f':
                out += "\\f";
                break;
            case '\n': 
                out += "\\n"; 
                break;
            case '\r': 
                out += "\\r";
                break;
            case '\t': 
                out += "\\t";
                break;

            default  : 
                out += c; 
                break;
        }
    }

    return out;
}


Json Json::Parse(std::string_view str, Size &offset) {

    char value;
    SkipWhiteSpace( str, offset );
    value = str[offset];
    switch (value) 
    {
        case '{' : 
            return ParseDict( str, offset );
        case '[' : 
            return ParseList( str, offset );
        case '\"': 
            return ParseString( str, offset );
        case 't' :
        case 'f' : 
            return ParseBool( str, offset );
        case 'n' : 
            return ParseNull( str, offset );
        default  : 
            if( ( value >= '0' && value <= '9' ) || value == '-' )
            {
                return ParseNumber( str, offset );
            }
    }
    
    throw std::runtime_error( FormatStr("Json : Parse unknown starting character %c", value) );

    return Json();
}


Json Json::ParseDict(std::string_view str, Size &offset) {

    Json out = MakeDict();

    ++offset;
    SkipWhiteSpace(str, offset);
    if (str[offset] == '}') 
    {
        ++offset; 
        return out;
    }
    while (true) 
    {
        Json key = Parse(str, offset);
        SkipWhiteSpace(str, offset);
        if( str[offset] != ':' ) 
        {
            throw std::runtime_error( FormatStr("Json :  Expected ':' , found '%c'",  str[offset]) );
            break;
        }
        Json value = Parse(str, ++offset);
        out[key.ToString()] = value;

        SkipWhiteSpace(str, offset);
        if (str[offset] == ',') 
        {
            ++offset; 
            continue;
        }
        else if (str[offset] == '}') 
        {
            ++offset; 
            break;
        }
        else 
        {
            throw std::runtime_error( FormatStr("Json :  Expected ',' , found '%c'",  str[offset]) );
            break;
        }
    }

    return out;  
}


Json Json::ParseList(std::string_view str, Size &offset) {

    Json out = MakeList();
  
    ++offset;
    SkipWhiteSpace(str, offset);
    if (str[offset] == ']') 
    {
        ++offset; 
        return out;
    }
    while (true) 
    {
        out.Append( Parse(str, offset) );
        SkipWhiteSpace(str, offset);
        if( str[offset] == ',' ) 
        {
            ++offset; 
            continue;
        }
        else if(str[offset] == ']') 
        {
            ++offset; 
            break;
        }
        else 
        {
            throw std::runtime_error( FormatStr("Json :  Expected ',' or ']' in List, found '%c'",  str[offset]) );
        }
    }

    return out; 
}


Json Json::ParseString(std::string_view str, Size &offset) {

    Json out;

    std::string val;
    for (char c = str[++offset]; c != '\"' ; c = str[++offset])
    {
        if (c == '\\') 
        {
            switch(str[++offset]) 
            {
                case '\"': 
                    val += '\"'; 
                    break;
                case '\\': 
                    val += '\\'; 
                    break;
                case '/' : 
                    val += '/'; 
                    break;
                case 'b' : 
                    val += '\b';
                    break;
                case 'f' : 
                    val += '\f'; 
                    break;
                case 'n' : 
                    val += '\n'; 
                    break;
                case 'r' : 
                    val += '\r'; 
                    break;
                case 't' : 
                    val += '\t'; 
                    break;
                case 'u' : 
                    val += "\\u" ;
                    for( unsigned i = 1; i <= 4; ++i ) 
                    {
                        c = str[offset + i];
                        if( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )
                        {
                            val += c;
                        }
                        else 
                        {
                            throw std::runtime_error( FormatStr("Json : Expected hex character in unicode escape, found '%c'",  c) );
                        }
                    }
                    offset += 4;
                    break;

                default  : 
                    val += '\\'; 
                    break;
            }
        }
        else
        {
            val += c;
        }
    }
    ++offset;
    out = val;

    return out;   
}


Json Json::ParseNumber(std::string_view str, Size &offset) {

    Json out;

    std::string valStr, expStr;
    bool isDouble = false;
    char c;

    // get value
    while (true) 
    {
        c = str[offset++];
        if ( (c == '-') || (c >= '0' && c <= '9') )
        {
            valStr += c;
        }
        else if( c == '.' )
        {
            valStr += c; 
            isDouble = true;
        }
        else
        {
            break;
        }
    }

    // get exponent
    if( c == 'E' || c == 'e' ) 
    {
        c = str[ offset++ ];
        if( c == '-' )
        { 
            ++offset; 
            expStr += '-';
        }
        while( true )
        {
            c = str[ offset++ ];
            if ( c >= '0' && c <= '9' )
            {
                expStr += c;
            }
            else if ( !isspace(c) && c != ',' && c != ']' && c != '}' ) 
            {
                throw std::runtime_error( FormatStr("Json : Expected a number for exponent, found '%c'",  c) );
            }
            else
            {
                break;
            }
        }
    }
    else if ( !isspace( c ) && c != ',' && c != ']' && c != '}' ) 
    {
        throw std::runtime_error( FormatStr("Json : Unexpected character found '%c'",  c) );
    }

    --offset;
    I64 exp = (expStr.empty() ? 1 : stol(expStr));
    if (isDouble)
    {
        out = stod(valStr) * (double)exp;
    }
    else
    {
        out = stol(valStr) * exp;
    }
    
    return out; 
}


Json Json::ParseBool(std::string_view str, Size &offset) {

    Json out;
    if (str.substr(offset, 4) == "true") 
    {
        offset += 4;
        out = true;
    }
    else if (str.substr(offset, 5) == "false")
    {
        offset += 5;
        out = false;
    }
    else 
    {
        throw std::runtime_error( FormatStr("Json : Expected 'true' or 'false', found '%s'",  str.substr(offset, 5)) );
    }
    
    return out; 
}


Json Json::ParseNull(std::string_view str, Size &offset) {

    Json out;
    if ( str.substr(offset, 4) != "null" ) 
    {
        throw std::runtime_error( FormatStr("Json : Expected 'null', found '%s'",  str.substr( offset, 4 )) );
    }    
    offset += 4;

    return out; 
}

    
} // Atuin
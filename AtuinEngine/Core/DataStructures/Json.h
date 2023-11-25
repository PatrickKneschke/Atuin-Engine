
#pragma once 


#include "Core/Util/Types.h"
#include "Core/Debug/Log.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include <initializer_list>
#include <string>
#include <variant>


namespace Atuin {


class Json {

    using JsonList = Array<Json>;
    using JsonDict = Map<std::string, Json>;
    using Internal = std::variant<std::monostate, bool, I64, double, std::string, JsonList, JsonDict>;

public:

    enum class JsonType {

        EMPTY, 
        BOOL,
        INT,
        FLOAT,
        STRING,
        LIST,
        DICT
    };

public:

    static Json MakeList() {

        Json out;
        out.mData = JsonList();

        return out;
    }

    static Json MakeDict() {

        Json out;
        out.mData = JsonDict();

        return out;
    }

    static Json Load(std::string_view file);


    Json();
    Json(const Json &other);
    Json(Json &&other);

    Json(bool b) : mData(b) {}
    Json(I64 i) : mData(i) {}
    Json(double d) : mData(d) {}
    Json(const std::string &s) { mData = s; }
    Json(std::string &&s) { mData = std::move(s); }
    Json(const char* s) : Json(std::string(s)) {}
    
    Json& operator= (const Json &rhs);
    Json& operator= (Json &&rhs);

    Json& operator= (bool b) { mData = b; return *this; }
    Json& operator= (I64 i) { mData = i; return *this; }
    Json& operator= (double d) { mData = d; return *this; }
    Json& operator= (const std::string &s) { mData = s; return *this; }
    Json& operator= (std::string &&s) { mData = std::move(s); return *this; }
    Json& operator= (const char* s) { return operator=(std::string(s)); }

    ~Json();

    void Clear();

    Size GetSize() const;
    bool HasKey(const std::string &key) const;
    bool IsNull() const { return mData.index() == (Size)JsonType::EMPTY; }

    template<typename T>
    void Append(T &&t) { 

        if (mData.index() != (Size)JsonType::LIST)
        {
            mLog.Error( LogChannel::GENERAL, "Tried to append values to non-list type json object." );
        }

        get< (Size)JsonType::LIST >(mData).EmplaceBack(std::forward<T>(t));
    }

    template<typename T, typename... Args>
    void Append(T &&t, Args&&... args) {

        Append( std::forward<T>(t) ); Append( std::forward<Args>(args)... );
    }

    Json& operator[] (const std::string &key);
    Json& At(const std::string &key);
    const Json& At(const std::string &key) const;

    Json& operator[] (Size idx);
    const Json& operator[] (Size idx) const;

    bool ToBool() const;
    I64 ToInt() const;
    double ToFloat() const;
    std::string ToString() const;
    const JsonList& GetList() const;
    const JsonDict& GetDict() const;

    JsonType GetType() const { return (JsonType)mData.index(); }

    std::string Print(Size depth = 1, std::string tab = "    ") const;

    friend std::ostream& operator<<( std::ostream& os, const Json &json );


private:

    static void SkipWhiteSpace(std::string_view, Size &offset);
    static std::string JsonEscape(const std::string &in);

    static Json Parse(std::string_view, Size &offset);
    static Json ParseDict(std::string_view, Size &offset);
    static Json ParseList(std::string_view, Size &offset);
    static Json ParseString(std::string_view, Size &offset);
    static Json ParseNumber(std::string_view, Size &offset);
    static Json ParseBool(std::string_view, Size &offset);
    static Json ParseNull(std::string_view, Size &offset);
    

    Internal mData;

    Log mLog;
};

    
} // Atuin
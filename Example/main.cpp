#pragma once
#include "../InternedString/InternedString.hpp"
#include <map>

int main(int argc, char* argv[])
{
    InternedString::Initialize();
    InternedString internedString("55555");
    InternedString internedString1("55555");
    InternedString internedString2("666666");
    bool isNull = internedString.IsNULL();
    bool same = internedString == internedString1;
    bool less = internedString1 < internedString2;
    auto sv = internedString.ToStringView();
    auto s = internedString.ToString();
    std::map< InternedString, int> table{};
    table[internedString] = 0;
    auto iter = table.find(internedString);
    return 0;
}
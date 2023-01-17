#pragma once
#include "InternedString.hpp"

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
    return 0;
}
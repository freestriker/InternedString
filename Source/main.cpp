#pragma once
#include "InternedString.hpp"

int main(int argc, char* argv[])
{
    InternedString::Initialize();
    InternedString internedString("55555");
    auto sv = internedString.ToStringView();
    auto s = internedString.ToString();
    return 0;
}
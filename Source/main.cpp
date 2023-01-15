#pragma once
#include "InternedString.hpp"

int main(int argc, char* argv[])
{
    InternedString::Initialize();
    InternedString InternedString("55555");
    return 0;
}
#pragma once
#include "../InternedString/InternedString.hpp"
#include <map>
#include <chrono> 
#include <iostream>
#include <string>
#include <random>
#include <unordered_map>

constexpr uint32_t MIN_STRING_SIZE = 6u;
constexpr uint32_t MAX_STRING_SIZE = 25u;
constexpr uint32_t PER_SIZE_STRING_COUNT = 800u;

std::string GetRandomString(uint32_t size)
{
    char tmp;
    std::string buffer;

    std::random_device rd;
    std::default_random_engine random(rd());

    for (int i = 0; i < size; i++)
    {
        tmp = random() % 36;
        if (tmp < 10) {
            tmp += '0';
        }
        else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return buffer;
}

int main(int argc, char* argv[])
{
    InternedString::Initialize();

    {
        std::string stdString = "stdString";
        std::string_view stdStringView = "stdStringView";
        const char* constCharPtr = "constCharPtr";

        InternedString is0 = InternedString(stdString);
        InternedString is1 = InternedString(stdStringView);
        InternedString is2 = InternedString(constCharPtr);

        bool isNull = is0.IsNULL();
        auto size = is0.Size();
        bool same = is0 == is1;
        bool less = is0 < is1;
        auto hash = std::hash<InternedString>()(is0);
    }

    std::cout << "The test was performed using " << (MAX_STRING_SIZE - MIN_STRING_SIZE) * PER_SIZE_STRING_COUNT << " randomly generated strings ranging in length from " << MIN_STRING_SIZE << " to " << MAX_STRING_SIZE << "." << std::endl;

    std::cout << "-----std::string-----" << std::endl;
    ///std string
    {
        std::array<std::string, PER_SIZE_STRING_COUNT* (MAX_STRING_SIZE - MIN_STRING_SIZE + 1)> stdStringArray{};

        std::cout << "-----build-----" << std::endl;
        ///Create string
        {
            auto start = std::chrono::high_resolution_clock::now();

            for (uint32_t size = MIN_STRING_SIZE; size <= MAX_STRING_SIZE; size++)
            {
                for (uint32_t perSizeIndex = 0; perSizeIndex < PER_SIZE_STRING_COUNT; perSizeIndex++)
                {
                    uint32_t index = (size - MIN_STRING_SIZE) * PER_SIZE_STRING_COUNT + perSizeIndex;
                    stdStringArray[index] = GetRandomString(size);
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::cout
                << "Use "
                << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                << "s to create a std string array."
                << std::endl;
        }

        std::cout << "-----map-----" << std::endl;
        ///Map
        {
            std::map<std::string, uint32_t> table{};

            ///Insert
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : stdStringArray)
                {
                    table[string] = 0;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to build a std string map."
                    << std::endl;
            }

            ///Search once
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : stdStringArray)
                {
                    table[string] += 1;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a std string map once."
                    << std::endl;
            }

            ///Search 10 times
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (int i = 0; i < 10; i++)
                {
                    for (const auto& string : stdStringArray)
                    {
                        table[string] += 1;
                    }
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a std string map 10 times."
                    << std::endl;
            }
        }

        std::cout << "-----hash map-----" << std::endl;
        ///Hash map
        {
            std::unordered_map<std::string, uint32_t> table{};

            ///Insert
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : stdStringArray)
                {
                    table[string] = 0;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to build a std string hash map."
                    << std::endl;
            }

            ///Search once
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : stdStringArray)
                {
                    table[string] += 1;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a std string hash map once."
                    << std::endl;
            }

            ///Search 10 times
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (int i = 0; i < 10; i++)
                {
                    for (const auto& string : stdStringArray)
                    {
                        table[string] += 1;
                    }
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a std string hash map 10 times."
                    << std::endl;
            }
        }

    }
    std::cout << std::endl;

    std::cout << "-----interned string-----" << std::endl;
    ///Interned string
    {
        std::array<InternedString, PER_SIZE_STRING_COUNT* (MAX_STRING_SIZE - MIN_STRING_SIZE + 1)> internedStringArray{};
        
        std::cout << "-----build-----" << std::endl;
        ///Create interned string
        {
            auto start = std::chrono::high_resolution_clock::now();

            for (uint32_t size = MIN_STRING_SIZE; size <= MAX_STRING_SIZE; size++)
            {
                for (uint32_t perSizeIndex = 0; perSizeIndex < PER_SIZE_STRING_COUNT; perSizeIndex++)
                {
                    uint32_t index = (size - MIN_STRING_SIZE) * PER_SIZE_STRING_COUNT + perSizeIndex;
                    internedStringArray[index] = InternedString(GetRandomString(size));
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::cout
                << "Use "
                << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                << "s to create a interned string array."
                << std::endl;
        }

        std::cout << "-----map-----" << std::endl;
        ///Map
        {
            std::map<InternedString, uint32_t> table{};

            ///Insert
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : internedStringArray)
                {
                    table[string] = 0;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to build a interned string map."
                    << std::endl;
            }

            ///Search
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : internedStringArray)
                {
                    table[string] += 1;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a interned string map once."
                    << std::endl;
            }

            ///Search 10 times
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (int i = 0; i < 10; i++)
                {
                    for (const auto& string : internedStringArray)
                    {
                        table[string] += 1;
                    }
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a interned string map 10 times."
                    << std::endl;
            }
        }

        std::cout << "-----hash map-----" << std::endl;
        ///Hash map
        {
            std::unordered_map<InternedString, uint32_t> table{};

            ///Insert
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : internedStringArray)
                {
                    table[string] = 0;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to build a interned string hash map."
                    << std::endl;
            }

            ///Search
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (const auto& string : internedStringArray)
                {
                    table[string] += 1;
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a interned string hash map once."
                    << std::endl;
            }

            ///Search 10 times
            {
                auto start = std::chrono::high_resolution_clock::now();

                for (int i = 0; i < 10; i++)
                {
                    for (const auto& string : internedStringArray)
                    {
                        table[string] += 1;
                    }
                }

                auto end = std::chrono::high_resolution_clock::now();
                std::cout
                    << "Use "
                    << double(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
                    << "s to search a interned string hash map 10 times."
                    << std::endl;
            }
        }
    }
    std::cout << std::endl;

    return 0;
}
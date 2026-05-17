#include <iostream>
#include <string>
#include <cctype>

int main()
{
    std::string buf = "#Dddd";
    for (int i = 0; i < 4; i++)
         buf[i] = std::tolower(buf[i]);

    std::cout << buf << std::endl;
}
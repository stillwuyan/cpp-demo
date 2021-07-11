#include <cstdio>
#include <cstring>
#include <string>

int main()
{
    std::string line;
    FILE* file = fopen("test.txt", "r");
    char buffer[10];
    while (fgets(buffer, sizeof(buffer), file))
    {
        line += buffer;
        if (strnlen(buffer, 10) < 9)
        {
            break;
        }
    }

    for (int i = 0; i < line.size(); i++)
    {
        printf("%02x ", line.c_str()[i]);
    }
    printf("\n");
    return 0;
}

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    std::string filename = "/var/bin/../test/../../a.txt";
    if (argc == 2)
    {
        filename = argv[1];
    }
    std::cout << "current file: " << filename << std::endl;

    // Strip the path traversal
    const std::string toReplace("../");
    for (size_t pos = filename.find(toReplace); pos != std::string::npos;)
    {
        filename.replace(pos, toReplace.length(), "");
        pos = filename.find(toReplace);
    }

    std::cout << "current file: " << filename << std::endl;

    return 0;
}

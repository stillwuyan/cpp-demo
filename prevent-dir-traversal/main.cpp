#include <iostream>
#include <fstream>
#include <string>

void prevent_traversal(std::string& filename)
{
    // Strip the path traversal
    const std::string toReplace("../");
    for (size_t pos = filename.find(toReplace); pos != std::string::npos;)
    {
        filename.replace(pos, toReplace.length(), "");
        pos = filename.find(toReplace);
    }
}

void print_state (const std::ios& stream) {
    std::cout << " good()=" << stream.good() << std::endl;
    std::cout << " eof()=" << stream.eof() << std::endl;
    std::cout << " fail()=" << stream.fail() << std::endl;
    std::cout << " bad()=" << stream.bad() << std::endl;
}

int main(int argc, char* argv[])
{
    std::string filename = argv[1];
    std::cout << "input filename is " << filename << std::endl;

    prevent_traversal(filename);
    std::cout << "after stripping is " << filename << std::endl;

    std::string prefix = "./tmp/";
    std::ofstream output(prefix + filename);
    output.write("hello world\n", 12);
    std::cout << "write to " << prefix + filename << std::endl;
    std::cout << "write result is:" << std::endl;
    print_state(output);
    return 0;
}

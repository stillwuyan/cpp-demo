#include <map>
#include <string>
#include <iostream>

int main() {
    std::cout << "c++ version: " << __cplusplus << std::endl;

    const std::map<std::string, std::string> capitals {
        { "Poland", "Warsaw"},
        { "USA", "Washington"},
        { "France", "Paris"},
        { "UK", "London"},
        { "Germany", "Berlin"}
    };

#if __cplusplus > 201402L
    std::cout << "you can use structured binding" << std::endl;
    for (const auto & [k,v] : capitals) {
        std::cout << k << ":" << v << std::endl;
    }
#else
    std::cout << "you can't use structured binding" << std::endl;
    for (const auto & v : capitals) {
        std::cout << v.first << ":" << v.second << std::endl;
    }
#endif
    return 0;
}



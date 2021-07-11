#include <iostream>
#include <string>
#include <regex>
 
int main()
{
    // Simple regular expression matching
    const std::string fnames[] = {"foo.txt", "bar.txt", "baz.dat", "zoidberg"};
    const std::regex txt_regex("[a-z]+\\.txt");
 
    for (const auto &fname : fnames) {
        std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';
    }
 
    // Extraction of a sub-match
    const std::string version = " PACKAGE_VERSION=547.0.02";
    const std::regex base_regex(R"--(PACKAGE_VERSION=(\d{1,3}\.\d{1,3}\.\d{1,3}))--");
    std::smatch base_match;
 
    if (std::regex_search(version, base_match, base_regex)) {
        // The first sub_match is the whole string; the next
        // sub_match is the first parenthesized expression.
        if (base_match.size() == 2) {
            std::cout << "find the version" << std::endl;
            std::string base = base_match[1];
            std::cout << "version is " << base << '\n';
        }
    }
 
    // Extraction of several sub-matches
    const std::regex pieces_regex("([a-z]+)\\.([a-z]+)");
    std::smatch pieces_match;
 
    for (const auto &fname : fnames) {
        if (std::regex_match(fname, pieces_match, pieces_regex)) {
            std::cout << fname << '\n';
            for (size_t i = 0; i < pieces_match.size(); ++i) {
                std::ssub_match sub_match = pieces_match[i];
                std::string piece = sub_match.str();
                std::cout << "  submatch " << i << ": " << piece << '\n';
            }   
        }   
    }   
}

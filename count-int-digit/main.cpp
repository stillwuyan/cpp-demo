#include <iostream>
#include <chrono>
#include <string>
#include <functional>
#include <iomanip>
#include <cmath>

int use_string(int n)
{
    return std::to_string(n).size();
}

int use_log10(int n)
{
    if (n == 0)
    {
        return 1;
    }

    return (int)(log10(n) + 1);
}

int use_divide(int n)
{
    if (n == 0)
        return 1;

    int l;
    for (l = 0; n > 0; ++l)
        n /= 10;
    return l;
}

int use_divide_and_conquer(int n)
{
    if (n < 100000) {
        // 5 or less
        if (n < 100) {
            // 1 or 2
            if (n < 10)
                return 1;
            else
                return 2;
        } else {
            // 3 or 4 or 5
            if (n < 1000)
                return 3;
            else {
                // 4 or 5
                if (n < 10000)
                    return 4;
                else
                    return 5;
            }
        }
    } else {
        // 6 or more
        if (n < 10000000) {
            // 6 or 7
            if (n < 1000000)
                return 6;
            else
                return 7;
        } else {
            // 8 to 10
            if (n < 100000000)
                return 8;
            else {
                // 9 or 10
                if (n < 1000000000)
                    return 9;
                else
                    return 10;
            }
        }
    }
}

int all_use_string() {
    int x = 0;
    for (int i = 0; i < 1000; i++)
        x = use_string(i);
    for (int i = 1000; i < 100000; i += 10)
        x = use_string(i);
    for (int i = 100000; i < 1000000; i += 100)
        x = use_string(i);
    for (int i = 1000000; i < 2000000000; i += 200)
        x = use_string(i);

    return x;
}

int all_use_log10() {
    int x = 0;
    for (int i = 0; i < 1000; i++)
        x = use_log10(i);
    for (int i = 1000; i < 100000; i += 10)
        x = use_log10(i);
    for (int i = 100000; i < 1000000; i += 100)
        x = use_log10(i);
    for (int i = 1000000; i < 2000000000; i += 200)
        x = use_log10(i);

    return x;
}

int all_use_divide() {
    int x = 0;
    for (int i = 0; i < 1000; i++)
        x = use_divide(i);
    for (int i = 1000; i < 100000; i += 10)
        x = use_divide(i);
    for (int i = 100000; i < 1000000; i += 100)
        x = use_divide(i);
    for (int i = 1000000; i < 2000000000; i += 200)
        x = use_divide(i);

    return x;
}

int all_use_divide_and_conquer() {
    int x = 0;
    for (int i = 0; i < 1000; i++)
        x = use_divide_and_conquer(i);
    for (int i = 1000; i < 100000; i += 10)
        x = use_divide_and_conquer(i);
    for (int i = 100000; i < 1000000; i += 100)
        x = use_divide_and_conquer(i);
    for (int i = 1000000; i < 2000000000; i += 200)
        x = use_divide_and_conquer(i);

    return x;
}

void benchmark(std::function<void()> proc, const std::string& name)
{
    std::cout << std::fixed << std::setprecision(9) << std::left;
    auto start = std::chrono::high_resolution_clock::now();
    proc();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "\"" << std::setw(30) << name << "\"" << " used " << diff.count() << " s" << std::endl;
}

int main(int argc, char* argv[])
{
    benchmark(all_use_string, "use_string");
    benchmark(all_use_log10, "use_log10");
    benchmark(all_use_divide, "use_divide");
    benchmark(all_use_divide_and_conquer, "use_divide_and_conquer");

    benchmark(std::bind(use_string, 999999999), "use_string");
    benchmark(std::bind(use_log10, 999999999), "use_log10");
    benchmark(std::bind(use_divide, 999999999), "use_divide");
    benchmark(std::bind(use_divide_and_conquer, 999999999), "use_divide_and_conquer");
    return 0;
}



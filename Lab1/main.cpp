#include <tuple>
#include <iostream>
#include <cstdlib>

auto f1 = [](int x) { return 20 - abs(x); };
auto f2 = [](int x) { return (2 * (x % 2) - 1) * x / 2; };

auto obliczenia = [](auto f1, auto f2, int n) {
    using namespace std;
    int max_x = f2(0);
    int max_y = f1(max_x);
    for (int i = 1; i < n; i++) {
        int x = f2(i);
        int y = f1(x);
        if (y > max_y) {
            max_x = x;
            max_y = y;
        }
    }
    return make_pair(max_x, max_y);
};

int main(int argc, char **argv) {
    using namespace std;
    auto[a, b] = obliczenia(f1, f2, 100);
    cout << a << " : " << b << endl;
}
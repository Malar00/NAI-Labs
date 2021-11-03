#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;


random_device rd;
mt19937 mt_generator(rd());

std::vector<double>
hill_climbing(std::function<double(std::vector<double>)> f, std::function<bool(std::vector<double>)> f_domain,
              std::vector<double> p0, int iterations) {
    auto p = p0;
    std::uniform_int_distribution<> distrib(0, p.size() - 1);
    std::uniform_real_distribution<> distrib_r(-0.1, 0.1);

    if (!f_domain(p)) throw std::invalid_argument("The p0 must be in domain");
    for (int i = 0; i < iterations; i++) {
        auto p2 = p;

        p[distrib(mt_generator)] += distrib_r(mt_generator);
        double y2 = f(p2);
        if (y2 < f(p)) {
            p = p2;
        }
    }
    return p;
}

std::ostream &operator<<(std::ostream &o, std::vector<double> v) {
    for (auto e: v) {
        o << std::fixed << std::setprecision(5) << "\t" << e;
    }
    return o;
}

vector<double> simulated_annealing(
        function<double(vector<double>)> f,
        function<bool(vector<double>)> f_domain,
        vector<double> p0,
        int iterations,
        function<vector<double>(vector<double>)> N,
        function<double(int)> T) {
    auto s_current = p0;
    auto s_global_best = p0;

    uniform_real_distribution<> u_k(0.0, 1.0);

    if (!f_domain(s_current)) throw std::invalid_argument("The p0 point must be in domain");

    for (int k = 0; k < iterations; k++) {
        auto s_next = N(s_current);
        if (f(s_next) < f(s_current)) {
            s_current = s_next;
        } else {
            double u = u_k(mt_generator);
            if (u < exp(-abs(f(s_next) - f(s_current)) / T(k))) {
                s_current = s_next;
            }
        }
        if (f(s_current) < f(s_global_best)) {
            s_global_best = s_current;
        }
    }
    return s_global_best;
}


int main() {
    auto ackley = [](std::vector<double> v) {
        double x = v.at(0), y = v.at(1);
        return -20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) +
               M_E + 20.0;
    };

    auto ackley_domain = [](std::vector<double> v) {
        return (std::abs(v[0]) <= 5) && (std::abs(v[1]) <= 5);
    };

    uniform_real_distribution<> distrib_r(-5, 5);
    vector<double> ackley_p0 = {
            distrib_r(mt_generator),
            distrib_r(mt_generator),
    };

    int choice = 0;
    cout << "0 - hill\n1 - annealing\n";
    cin >> choice;
    std::vector<double> result;
    switch (choice) {
        case 0:
            result = hill_climbing(ackley, ackley_domain, ackley_p0, 1000000);
            std::cout << result << " -> " << ackley(result) << std::endl;
            break;
        case 1:
            result = simulated_annealing(
                    ackley, ackley_domain, ackley_p0, 100000000,
                    [](auto p) {
                        normal_distribution<double> n(0.0, 0.3);
                        for (auto &e: p) {
                            e = e + n(mt_generator);
                        }
                        return p;
                    },
                    [](int k) { return 1000.0 / k; });
            std::cout << result << " -> " << ackley(result) << std::endl;
            break;
        default:
            throw std::invalid_argument("Wrong number");
    }


    return 0;
}

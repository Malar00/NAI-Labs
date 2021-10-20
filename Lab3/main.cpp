#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <cstdlib>
#include <cmath>

std::random_device rd;
std::mt19937 gen(rd());

std::vector<double>
hill_climbing(std::function<double(std::vector<double>)> f, std::function<bool(std::vector<double>)> f_domain,
              std::vector<double> p0, int iterations) {
    auto p = p0;
    std::uniform_int_distribution<> distrib(0, p.size() - 1);
    std::uniform_real_distribution<> distrib_r(-0.1, 0.1);

    if (!f_domain(p)) throw std::invalid_argument("The p0 must be in domain");
    for (int i = 0; i < iterations; i++) {
        auto p2 = p;

        p[distrib(gen)] += distrib_r(gen);
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


int main() {
    auto ackley = [](std::vector<double> v) {
        double x = v.at(0), y = v.at(1);
        return -20 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) - exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) +
               M_E + 20.0;
    };

    auto ackley_domain = [](std::vector<double> v) {
        return (std::abs(v[0]) <= 5) && (std::abs(v[1]) <= 5);
    };

    auto beale = [](std::vector<double> v) {
        double x = v.at(0), y = v.at(1);
        return pow(1.5 - x + x * y, 2) + pow(2.25 - x + x * pow(y, 2), 2) + pow(2.625 - x + x * pow(y, 3), 2);
    };
    auto beale_domain = [](std::vector<double> v) {
        return (std::abs(v[0]) <= 4.5) && (std::abs(v[1]) <= 4.5);
    };

    auto egg = [](std::vector<double> v) {
        double x = v.at(0), y = v.at(1);
        return -(y + 47) * sin(sqrt(std::abs(x / 2 + (y + 47)))) - x * sin(sqrt(std::abs(x - (y + 47))));
    };
    auto egg_domain = [](std::vector<double> v) {
        return (std::abs(v[0]) <= 512) && (std::abs(v[1]) <= 512);
    };

    std::uniform_real_distribution<> distrib_r(-5, 5);
    std::vector<double> ackley_p0 = {
            distrib_r(gen),
            distrib_r(gen),
    };
    std::uniform_real_distribution<> distrib_r_beal(-4.5, 4.5);
    std::vector<double> beale_p0 = {
            distrib_r_beal(gen),
            distrib_r_beal(gen),
    };
    std::uniform_real_distribution<> distrib_r_egg(-512, 512);
    std::vector<double> egg_p0 = {
            distrib_r_egg(gen),
            distrib_r_egg(gen),
    };


    int choice = 0;
    int iter = 0;
    std::cout << "1. ackley\n" << "2. beale\n" << "3. egg holder\n===>";
    std::cin >> choice;
    std::cout << "Number of iterations\n===>";
    std::cin >> iter;
    if (iter <= 0 || iter > 10000)throw std::invalid_argument("Wrong number");
    std::vector<double> result;

    switch (choice) {
        case 1:
            result = hill_climbing(ackley, ackley_domain, ackley_p0, iter);
            std::cout << result << " -> " << ackley(result) << std::endl;
            break;
        case 2:
            result = hill_climbing(beale, beale_domain, beale_p0, iter);
            std::cout << result << " -> " << beale(result) << std::endl;
            break;
        case 3:
            result = hill_climbing(egg, egg_domain, egg_p0, iter);
            std::cout << result << " -> " << egg(result) << std::endl;
            break;
        default:
            throw std::invalid_argument("Wrong number");
    }

    //auto result = hill_climbing(ackley, ackley_domain, ackley_p0, 100);
    //auto result = hill_climbing(beale, beale_domain, beale_p0, 1000);
    //auto result = hill_climbing(egg, egg_domain, egg_p0, 1000);
    //std::cout << result << " -> " << egg(result) << std::endl;

    return 0;
}

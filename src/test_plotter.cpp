#include <iostream>
#include <cassert>
#include "plotter.h"

void test_set_get_current_pos() {
    Plotter plotter;
    int pos[2] = {10, 20};
    plotter.set_current_pos(pos);

    int* current = plotter.get_current_pos();
    assert(current[0] == 10);
    assert(current[1] == 20);
    std::cout << "test_set_get_current_pos passed\n";
}

void test_set_get_target_pos() {
    Plotter plotter;
    int pos[2] = {30, 40};
    plotter.set_target_pos(pos);

    int* target = plotter.get_target_pos();
    assert(target[0] == 30);
    assert(target[1] == 40);
    std::cout << "test_set_get_target_pos passed\n";
}

void test_calc_pos_error() {
    Plotter plotter;
    int current[2] = {5, 5};
    int target[2] = {15, 25};

    int* delta = plotter.calc_pos_error(current, target);
    assert(delta[0] == 10);
    assert(delta[1] == 20);
    std::cout << "test_calc_pos_error passed\n";
}

int main() {
    test_set_get_current_pos();
    test_set_get_target_pos();
    test_calc_pos_error();

    std::cout << "All tests passed!\n";
    return 0;
}

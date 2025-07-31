#include "plotter.h"

Plotter::Plotter() {
    current_pos[0] = 0;
    current_pos[1] = 0;
    target_pos[0] = 0;
    target_pos[1] = 0;
    delta_pos[0] = 0;
    delta_pos[1] = 0;
}

int *Plotter::get_current_pos() {
    return current_pos;
}

void Plotter::set_current_pos(int pos[2]) {
    current_pos[0] = pos[0];
    current_pos[1] = pos[1];
}

int *Plotter::get_target_pos() {
    return target_pos;
}

void Plotter::set_target_pos(int pos[2]) {
    target_pos[0] = pos[0];
    target_pos[1] = pos[1];
}

int *Plotter::calc_pos_error(int current_pos[2], int target_pos[2]) {
    delta_pos[0] = target_pos[0] - current_pos[0];
    delta_pos[1] = target_pos[1] - current_pos[1];
    return delta_pos;
}

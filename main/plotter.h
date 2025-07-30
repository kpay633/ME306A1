#ifndef PLOTTER_H
#define PLOTTER_H

class Plotter {
    public:
        int get_current_pos();
        void set_current_pos(int pos[2]);
        int get_target_pos();
        void set_target_pos(int pos[2]);
        int control_effort(int pos[2]);

    private:
        int current_pos[2];
        int target_pos[2];
        int delta_pos[2];
        int calc_pos_delta(int pos1[2], int pos2[2]);
};

#endif // PLOTTER_H

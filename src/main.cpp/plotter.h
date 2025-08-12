#ifndef PLOTTER_H
#define PLOTTER_H

class Plotter {
    public:
        Plotter();
        int *get_current_pos();
        void set_current_pos(int pos[2]);
        int *get_target_pos();
        void set_target_pos(int pos[2]);
        int *calc_pos_error(int current[2], int target[2]);
        void home();
        int get_left_boundary();
        int get_right_boundary();
        int get_top_boundary();
        int get_bottom_boundary();
        void set_left_boundary(int boundary);
        void set_right_boundary(int boundary);
        void set_top_boundary(int boundary);
        void set_bottom_boundary(int boundary);

    private:
        int current_pos[2];
        int target_pos[2];
        int delta_pos[2];
        int left_boundary, right_boundary, top_boundary, bottom_boundary;
};

#endif // PLOTTER_H

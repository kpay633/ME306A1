#include <iostream>
#include <chrono>
#include <thread>
enum State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};
struct GCode {
    
}
void new_state(State);
void GCodeParser(*GCode);
void doIdle(Plotter&);
void doHoming(Plotter&);
void doMoving(Plotter&);
void doFault(Plotter&);
State state = IDLE;
int main() {
    Plotter plotter;
    char key_in;
    int x;
    int y;
    
    while(1) {
        key_in = getchar();
        g_code_extractor(key_in);
        switch(state) {
            case IDLE:
                if(key_in == *(G1)) new_state(HOMING);
                if(key_in == *(G28)) new_state(MOVING);
                doIdle(plotter);
                break;
            case HOMING:
                doHoming(plotter);
                break;
            case MOVING:
                doMoving(plotter);  // <--- motor control loop here
                break;
            case FAULT:
                doFault(plotter);
                break;
        }
        // delay ???
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
void new_state(State s) {
    state = s;
    std::cout << "Transitioning to state: " << s << "\n";
}
void doIdle(Plotter& plotter) {
    std::cout << "Idle ..." << "\n";
}
void doHoming(Plotter& plotter) {
    plotter.home();
    new_state(IDLE);
}
void doMoving(Plotter& plotter);
void doFault(Plotter& plotter);
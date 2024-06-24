#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <algorithm>

// Simulation setup
#define SIZE 2
#define MC_STEPS 10000
#define TRY_TIME 1
#define NUM_ACTION 2
#define NUM_STATE 2

/* Game Parameters */
double b = 2;
double c = 1;
/* Information */
int action[SIZE];
int game_type;
int next_game_type;
/* Q-Learning */
double reward[SIZE];
double alpha = 0.1;
double tau = 3;
double gamma = 0.99;
double Qtable[SIZE][NUM_STATE][NUM_ACTION];
double cooperation[SIZE][NUM_STATE];
double mutual_cooperation[MC_STEPS + 1];

/* Payoff matrix */
double payoff_matrix_game1[2][2] = { {b - c,-c},
                                     {b,0} };

double payoff_matrix_game2[2][2] = { {-1,-1},
                                     {-1,-1} };

unsigned int seed = 100;
std::mt19937 gen(seed);
std::uniform_int_distribution<> rand_int(0, 1);
std::uniform_real_distribution<> rand_float(0.0, 1.0);

/* Initialize Q-table */
void init_Qtable() {
    for (int i = 0; i < SIZE; i++) {
        for (int s = 0; s < NUM_STATE; s++) {
            Qtable[i][s][0] = Qtable[i][s][1] = 0;
        }
    }
}

/* Initialize Game Type */
void init() {
    // Random Initialization
    game_type = rand_int(gen);
    // Game1
    //game_type = 0;
}

void select_action(int t) {
    for (int i = 0; i < SIZE; i++) {
        double temp = exp((Qtable[i][game_type][0]) * tau) + exp((Qtable[i][game_type][1]) * tau);
        if (rand_float(gen) < ((exp((Qtable[i][game_type][0]) * tau)) / temp)) {
            action[i] = 0;
        }
        else {
            action[i] = 1;
        }
    }
    if (action[0] == 0 && action[1] == 0) {
        mutual_cooperation[t] += 1;
    }
}

/* Deterministic transition */
int game_transition() {
    int game;
    // Action-dependent transition rule
    if (action[0] == 0 && action[1] == 0) {
        game = 0;
    }
    else {
        game = 1;
    }
    return game;
}

void strategy() {
    for (int i = 0; i < NUM_STATE; i++) {
        cooperation[0][i] = exp((Qtable[0][i][0]) * tau) / (exp((Qtable[0][i][0]) * tau) + exp((Qtable[0][i][1]) * tau));
        cooperation[1][i] = exp((Qtable[1][i][0]) * tau) / (exp((Qtable[1][i][0]) * tau) + exp((Qtable[1][i][1]) * tau));
    }
}

/* Payoff Calculation and Strategy Updating */
void update_stra() {
    if (game_type == 0) {
        reward[0] = payoff_matrix_game1[action[0]][action[1]];
        reward[1] = payoff_matrix_game1[action[1]][action[0]];
    }
    else {
        reward[0] = payoff_matrix_game2[action[0]][action[1]];
        reward[1] = payoff_matrix_game2[action[1]][action[0]];
    }
    next_game_type = game_transition();
    for (int i = 0; i < SIZE; i++) {
        double Qmax = std::max(Qtable[i][next_game_type][0], Qtable[i][next_game_type][1]);
        Qtable[i][game_type][action[i]] += alpha * (reward[i] + gamma * Qmax - Qtable[i][game_type][action[i]]);
    }
    game_type = next_game_type;
}

int main() {
    int step, try_time;
    for (step = 0; step < MC_STEPS + 1; step++) {
        mutual_cooperation[step] = 0;
    }
    for (try_time = 0; try_time < TRY_TIME; try_time++) {
        init_Qtable();
        init();
        for (step = 0; step < MC_STEPS + 1; step++) {
            select_action(step);
            strategy();
            update_stra();
        }
        std::cout << "Time:" << try_time << std::endl;
    }
    std::ofstream fp_data("output.csv");
    for (step = 0; step < MC_STEPS + 1; step++) {
        fp_data << step << "," << mutual_cooperation[step] / TRY_TIME << "\n";
    }
    fp_data.close();

    return 0;
}

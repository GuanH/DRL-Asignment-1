/*
 * This is a c version of simple_custom_taxi_env for faster training
 * (I didn't test whether it is actually faster lol.)
 */
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
int true = 1;
int false = 0;
int max(int a,int b){
    return a>b?a:b;
}
int min(int a,int b){
    return a<b?a:b;
}
typedef struct{
    int max_grid_size;
    int grid_size;
    int fuel_limit;
    int current_fuel;
    int passenger_picked_up;
    int stations[4][2];
    int passenger_loc[2];
    int obstacles[100][2];
    int destination[2];
    int max_obstacles;
    int n_obstacles;
    int taxi_pos[2];
}Taxi;

void get_state_taxi(Taxi* taxi, int states[16]);

void reset_taxi(Taxi* taxi, int states[16]){
    taxi->current_fuel = taxi->fuel_limit;
    taxi->passenger_picked_up = false;
    //if(taxi->max_grid_size == 5) taxi->grid_size = 5;
    //else taxi->grid_size = rand()%(taxi->max_grid_size-5)+5;
    taxi->grid_size = 10;
    const int grids = taxi->grid_size*taxi->grid_size;
    int av[grids][2];

    for(int i=0,k=0;i<taxi->grid_size;i++){
        for(int j=0;j<taxi->grid_size;j++,k++){
            av[k][0] = i;
            av[k][1] = j;
        }
    }
    for(int i=0;i<grids;i++){
        int rnd = rand()%(grids-i);
        int j = i+rnd;
        if(j>=grids){
            puts("I fucked up grids :(");
            j = 0;
        }
        int tmp[2];
        tmp[0] = av[j][0]; tmp[1] = av[j][1];
        av[j][0] = av[i][0]; av[j][1] = av[i][1];
        av[i][0] = tmp[0]; av[i][1] = tmp[1];
    }
    int k=0;
    for(int i=0;i<4;i++){
        int flag = 1;
        while(flag){
            flag=0;
            for(int j=0;j<i;j++){
                if(abs(taxi->stations[j][0]-av[k][0])+abs(taxi->stations[j][1]-av[k][1])==1){
                    flag=1;
                    k++;
                    break;
                }
            }
        }
        taxi->stations[i][0] = av[k][0];
        taxi->stations[i][1] = av[k][1];
        k++;
    }
    taxi->taxi_pos[0] = av[k][0];
    taxi->taxi_pos[1] = av[k][1];
    k++;

    int n_obstacles = taxi->max_obstacles ? rand()%taxi->max_obstacles : 0;
    taxi->n_obstacles = 0;
    for(int i=0;i<n_obstacles&&k<grids;i++){
        taxi->obstacles[i][0] = av[k][0];
        taxi->obstacles[i][1] = av[k][1];
        taxi->n_obstacles++;
        k++;
    }

    int pass = rand()%4;
    taxi->passenger_loc[0] = taxi->stations[pass][0];
    taxi->passenger_loc[1] = taxi->stations[pass][1];
    int des = rand()%4;
    if(des == pass) des = (des+1)%4;
    taxi->destination[0] = taxi->stations[des][0];
    taxi->destination[1] = taxi->stations[des][1];
    return get_state_taxi(taxi, states);
}

void init_taxi(Taxi* self, int states[16], int grid_size, int fuel_limit, int max_obstacle){
    self->grid_size = grid_size;
    self->fuel_limit = fuel_limit;
    self->max_grid_size = max(self->max_grid_size, 5);
    self->max_obstacles = min(max_obstacle, 100);
    reset_taxi(self, states);

}

int pos_in_arr(int x, int y, int arr[][2], int arr_n){
    for(int i=0;i<arr_n;i++){
        if(x == arr[i][0] && y == arr[i][1]){
            return 1;
        }
    }
    return 0;
}

void get_state_taxi(Taxi* taxi, int states[16]){
    int obstacle_north = taxi->taxi_pos[0] == 0 || 
        pos_in_arr(taxi->taxi_pos[0]-1, taxi->taxi_pos[1], taxi->obstacles, taxi->n_obstacles);
    int obstacle_south = taxi->taxi_pos[0] == taxi->grid_size-1 || 
        pos_in_arr(taxi->taxi_pos[0]+1, taxi->taxi_pos[1], taxi->obstacles, taxi->n_obstacles);
    int obstacle_east = taxi->taxi_pos[1] == taxi->grid_size-1 || 
        pos_in_arr(taxi->taxi_pos[0], taxi->taxi_pos[1]+1, taxi->obstacles, taxi->n_obstacles);
    int obstacle_west = taxi->taxi_pos[1] == 0 || 
        pos_in_arr(taxi->taxi_pos[0], taxi->taxi_pos[1]-1, taxi->obstacles, taxi->n_obstacles);

    int passenger_loc_north = taxi->taxi_pos[0] - 1 == taxi->passenger_loc[0]     &&
                              taxi->taxi_pos[1]     == taxi->passenger_loc[1]     ;
    int passenger_loc_south = taxi->taxi_pos[0] + 1 == taxi->passenger_loc[0]     &&
                              taxi->taxi_pos[1]     == taxi->passenger_loc[1]     ;
    int passenger_loc_east =  taxi->taxi_pos[0]     == taxi->passenger_loc[0] + 1 &&
                              taxi->taxi_pos[1]     == taxi->passenger_loc[1]     ;
    int passenger_loc_west =  taxi->taxi_pos[0]     == taxi->passenger_loc[0] - 1 &&
                              taxi->taxi_pos[1]     == taxi->passenger_loc[1]     ;
    int passenger_loc_mid =   taxi->taxi_pos[0]     == taxi->passenger_loc[0]     &&
                              taxi->taxi_pos[1]     == taxi->passenger_loc[1]     ;
    int passenger_look = passenger_loc_north || passenger_loc_south || passenger_loc_east || 
                        passenger_loc_west || passenger_loc_mid;

    int destination_loc_north = taxi->taxi_pos[0] - 1 == taxi->destination[0]     &&
                                taxi->taxi_pos[1]     == taxi->destination[1]     ;
    int destination_loc_south = taxi->taxi_pos[0] + 1 == taxi->destination[0]     &&
                                taxi->taxi_pos[1]     == taxi->destination[1]     ;
    int destination_loc_east =  taxi->taxi_pos[0]     == taxi->destination[0] + 1 &&
                                taxi->taxi_pos[1]     == taxi->destination[1]     ;
    int destination_loc_west =  taxi->taxi_pos[0]     == taxi->destination[0] - 1 &&
                                taxi->taxi_pos[1]     == taxi->destination[1]     ;
    int destination_loc_mid =  taxi->taxi_pos[0]     == taxi->destination[0]     &&
                               taxi->taxi_pos[1]     == taxi->destination[1]     ;
    int destination_look = destination_loc_north || destination_loc_south || destination_loc_east || 
                        destination_loc_west || destination_loc_mid;
    states[0] = taxi->taxi_pos[0];
    states[1] = taxi->taxi_pos[1];
    for(int i=0;i<4;i++){
        states[2+i*2] = taxi->stations[i][0];
        states[2+i*2+1] = taxi->stations[i][1];
    }
    states[10] = obstacle_north;
    states[11] = obstacle_south;
    states[12] = obstacle_east;
    states[13] = obstacle_west;
    states[14] = passenger_look;
    states[15] = destination_look;
}

void step_taxi(Taxi* self, int states[16], int action, float* reward, int* done){
    int taxi_row = self->taxi_pos[0];
    int taxi_col = self->taxi_pos[1];
    int next_row = taxi_row, next_col = taxi_col;
    *reward = 0;
    switch(action){
        case 0: next_row++;break;
        case 1: next_row--;break;
        case 2: next_col++;break;
        case 3: next_col--;break;
    }
    if(0<=action&&action<=3){
        if(pos_in_arr(next_row, next_col, self->obstacles, self->n_obstacles)||
           !(0<=next_row&&next_row<self->grid_size&&0<=next_col&&next_col<self->grid_size)){
            *reward -= 0.5;
        }
        else{
            self->taxi_pos[0] = next_row;
            self->taxi_pos[1] = next_col;
            if(self->passenger_picked_up){
                self->passenger_loc[0] = next_row;
                self->passenger_loc[1] = next_col;
            }
        }
    }
    else if(action == 4){
        if(!self->passenger_picked_up&&self->taxi_pos[0]==self->passenger_loc[0]&&self->taxi_pos[1]==self->passenger_loc[1]){
            self->passenger_picked_up = 1;
            self->passenger_loc[0]=self->taxi_pos[0]; self->passenger_loc[1]=self->taxi_pos[1];
        }
        else *reward -= 0.5;
    }
    else if(action == 5){
        if(self->passenger_picked_up){
            if(self->taxi_pos[0]==self->destination[0]&&self->taxi_pos[1]==self->destination[1]){
                get_state_taxi(self, states);
                *done = 1;
                return;
            }
            else *reward -= 0.5;
            self->passenger_picked_up = 0;
            self->passenger_loc[0] = self->taxi_pos[0]; self->passenger_loc[1] = self->taxi_pos[1];
        }
        else *reward -= 0.5;
    }
    *reward -= 0.1f;
    self->current_fuel -= 1;
    if(self->current_fuel <= 0){
        get_state_taxi(self, states);
        *done = 1;
        return;
    }
    get_state_taxi(self, states);
    *done = 0;
}

void render_taxi(Taxi* self){

    char grid[self->grid_size][self->grid_size+1];
    memset(grid, '.', sizeof(grid));
    for(int i=0;i<self->grid_size;i++) grid[i][self->grid_size] = '\0';
    grid[self->stations[0][0]][self->stations[0][1]] = 'R';
    grid[self->stations[1][0]][self->stations[1][1]] = 'G';
    grid[self->stations[2][0]][self->stations[2][1]] = 'Y';
    grid[self->stations[3][0]][self->stations[3][1]] = 'B';

    grid[self->passenger_loc[0]][self->passenger_loc[1]] = 'P';
    grid[self->destination[0]][self->destination[1]] = 'D';
    grid[self->taxi_pos[0]][self->taxi_pos[1]] = 'x';
    for(int i=0;i<self->n_obstacles;i++){
        grid[self->obstacles[i][0]][self->obstacles[i][1]] = 'o';
    }
    printf("\nTaxi Position: (%d, %d)\n", self->taxi_pos[0], self->taxi_pos[1]);
    if(self->passenger_loc[0]==self->taxi_pos[0] && self->passenger_loc[1]==self->taxi_pos[1]){
        printf("Passenger Position: (%d, %d) (In Taxi)\n", self->passenger_loc[0], self->passenger_loc[1]);
    }
    else printf("Passenger Position: (%d, %d)\n", self->passenger_loc[0], self->passenger_loc[1]);
    printf("Destination: (%d, %d)\n", self->destination[0], self->destination[1]);
    printf("Fuel Left: %d\n", self->current_fuel);
    int states[16];
    get_state_taxi(self, states);
    printf("[%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d]\n", states[0], states[1], states[2], states[3], states[4], states[5], states[6], states[7], states[8], states[9], states[10], states[11], states[12], states[13], states[14], states[15]);
    for(int i=0;i<self->grid_size;i++){
        printf("%s\n", grid[i]);
    }
}

void free_taxi(Taxi* taxi){
    free(taxi);
}

typedef struct{
    int init_n;
    int n;
    Taxi* taxi;
    int (*state)[16];
    float* reward;
    int* done;
}MultiTaxi;

MultiTaxi* create_multi_taxi(int n, int grid_size, int fuel_limit, int max_obstacle){
    srand(time(0));
    MultiTaxi* self = malloc(sizeof(MultiTaxi));
    self->init_n = self->n = n;
    self->taxi = calloc(n, sizeof(Taxi));
    self->state = calloc(n, sizeof(int[16]));
    self->reward = calloc(n, sizeof(float));
    self->done = calloc(n, sizeof(int));
    for(int i=0;i<n;i++){
        init_taxi(&self->taxi[i], self->state[i], grid_size, fuel_limit, max_obstacle);
    }
    return self;
}

void reset_multi_taxi(MultiTaxi* self){
    self->n = self->init_n;
    for(int i=0;i<self->n;i++){
        reset_taxi(&self->taxi[i], self->state[i]);
        self->done[i] = 0;
    }
}

void step_multi_taxi(MultiTaxi* self, int action[]){
    for(int i=0;i<self->n;i++){
        if(self->done[i]) continue;
        step_taxi(&self->taxi[i], self->state[i], action[i], &self->reward[i], &self->done[i]);
    }
}

void render_multi_taxi(MultiTaxi* self, int ind){
    render_taxi(&self->taxi[ind]);
}

void free_multi_taxi(MultiTaxi* self){
    free(self->taxi);
    free(self->state);
    free(self->reward);
    free(self->done);
    free(self);
}

/*
Compile
gcc -shared ctaxi.c -o ctaxi.so -fPIC

*/

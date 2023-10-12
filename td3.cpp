#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos
#include <assert.h>

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
    // IMPLEMENT YOUR FUNCTION HERE
    double* nw_array = new double[new_size];

    for (int i=0; i<length; i++) {
        nw_array[i] = array[i];
    }
    delete[] array;
    for (int i=length; i<new_size; i++) {
        nw_array[i] = 0.0;
    }
    return nw_array;
}

double* shrink_array(double* array, int length, int new_size) {
    if (new_size >= length) {
        return array;
    }
    else{
        double* nw_array = new double[new_size];
        for (int i=0; i<length; i++) {
            nw_array[i] = array[i];
        }
        delete[] array;
        return nw_array;
    }
}


double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {

    if (max_size==current_size) {
        array=extend_array(array,current_size,(current_size+5));
        max_size+=5;
    }
    array[current_size]=element;
    current_size+=1;
    return array;
}


double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
    // IMPLEMENT YOUR FUNCTION HERE
    if (current_size > 0) {
        current_size-=1;
        if (max_size-current_size >= 5) {
            array=shrink_array(array,max_size,current_size);
            max_size= current_size;
        }
    }
    return array;
}



bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
    // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

    bool hit_target, hit_obstacle;
    double v0_x, v0_y, x, y, t;
    double PI = 3.14159265;
    double g = 9.8;

    v0_x = magnitude * cos(angle * PI / 180);
    v0_y = magnitude * sin(angle * PI / 180);

    t = 0;
    x = 0;
    y = 0;

    hit_target = false;
    hit_obstacle = false;
    while (y >= 0 && (! hit_target) && (! hit_obstacle)) {

        telemetry = append_to_array(t,telemetry,telemetry_current_size,telemetry_max_size);
        telemetry = append_to_array(x,telemetry,telemetry_current_size,telemetry_max_size);
        telemetry = append_to_array(y,telemetry,telemetry_current_size,telemetry_max_size);

        double *target_coordinates = find_collision(x, y, targets, tot_targets);
        if (target_coordinates != NULL) {
            remove_target(targets, tot_targets, target_coordinates);
            hit_target = true;
        } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
            hit_obstacle = true;
        } else {
            t = t + simulation_interval;
            y = v0_y * t - 0.5 * g * t * t;
            x = v0_x * t;
        }
    }

    return hit_target;
}


void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &global_telemetry,
                     int &global_telemetry_current_size,
                     int &global_telemetry_max_size) {
    double minim_time;
    int idx, minim_time_index;
    idx=0;

    global_telemetry_current_size = 0;
    for (int i=0; i<tot_telemetries;i++) {
        global_telemetry_current_size+=telemetries_sizes[i];
    }

    global_telemetry= new double[global_telemetry_current_size];

    for (int i=0; i<tot_telemetries; i++) {
        for (int j=0; j<telemetries_sizes[i];j++) {
            global_telemetry[idx]=telemetries[i][j];
            idx+=1;
        }
    }

    //looked the following part online

    for (int i=0; i<global_telemetry_current_size; i+=3) {
        minim_time_index= i;
        minim_time= global_telemetry[i];

        for (int j= (i+3); j <global_telemetry_current_size; j+=3) {
            if (global_telemetry[j] <minim_time) {
                minim_time= global_telemetry[j];
                minim_time_index =j;
            }
        }

        std::swap(global_telemetry[i ], global_telemetry[minim_time_index]);
        std::swap(global_telemetry[1+i], global_telemetry[minim_time_index + 1]);
        std::swap(global_telemetry[2+i], global_telemetry[minim_time_index + 2]);
    }
    global_telemetry_max_size=global_telemetry_current_size;
}


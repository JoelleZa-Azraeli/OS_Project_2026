#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"

typedef struct {
    int path[15];          
    int path_count;        
    int current_index;     
    float segment_percent; 
    Vector2 current_pos;   
    bool is_active;        
    float speed;           
} TrafficAnimation;

#endif

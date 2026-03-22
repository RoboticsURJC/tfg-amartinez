#pragma once
#include <lvgl.h>

#define MAX_TAKES 30

typedef struct {
    char hour[6];
    bool repeat[7];
    bool recordatory;
    int warning_time;
} TakeConfig;

extern int total_takes;
extern TakeConfig takes[MAX_TAKES];

void takes_list_screen();
void edit_takes_screen(int index);
void repetition_screen(int index);
void delete_take(int index);

void show_home_screen();

void today_takes_screen();
int get_today_index();
bool is_take_today(int index);

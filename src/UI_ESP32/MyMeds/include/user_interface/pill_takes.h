#pragma once
#include <lvgl.h>

#define MAX_TAKES 7
#define MAX_MEDICINES_PER_TAKE 3

typedef struct {
    char id[20];
    uint8_t quantity;
} TakeMedicine;

typedef struct {

    char id[20];
    char hour[6];

    uint8_t repeat_mask;

    bool recordatory;
    uint8_t warning_time;

    uint8_t medicine_count;

    TakeMedicine medicines[MAX_MEDICINES_PER_TAKE];

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

inline bool is_day_active(uint8_t mask, int day) {
    return (mask & (1 << day)) != 0;
}

inline void set_day(uint8_t &mask, int day, bool value) {
    if (value)
        mask |= (1 << day);
    else
        mask &= ~(1 << day);
}

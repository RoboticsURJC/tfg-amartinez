#pragma once

#include <Arduino.h>

#define MAX_CATALOG_MEDICINES 8

typedef struct {
    char id[20];
    char name[30];
} CatalogMedicine;

extern CatalogMedicine medicineCatalog[MAX_CATALOG_MEDICINES];
extern int medicine_count;

void saveMedicines();
void loadMedicines();
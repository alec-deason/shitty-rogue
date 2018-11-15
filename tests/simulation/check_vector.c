#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../check_check.h"

#include "../../simulation/vector.h"

void vector_setup(void) {
};

void vector_teardown(void) {
};

START_TEST(development_target) {
    vector *v = make_vector(sizeof(int));
    int data = 0;

    vector_push(v, &data);
    data = 1;
    vector_push(v, &data);
    data = -1;
    vector_push(v, &data);

    ck_assert(*(int*)vector_peek(v) == -1);

    ck_assert(*(int*)vector_get(v, 0) == 0);
    ck_assert(*(int*)vector_get(v, 1) == 1);
    ck_assert(*(int*)vector_get(v, 2) == -1);

    data = 3;
    vector_set(v, 1, (void*)&data);
    ck_assert(*(int*)vector_get(v, 1) == 3);

    vector_swap(v, 1, 2);

    int *r;
    vector_pop(v, (void**)&r);
    ck_assert(*r == 3);
    vector_pop(v, (void**)&r);
    ck_assert(*r == -1);
    vector_pop(v, (void**)&r);
    ck_assert(*r == 0);
} END_TEST

START_TEST(get_and_set) {
    int len = 1000;
    int data[len];
    int i, j;
    vector *v = make_vector(sizeof(int));

    srand(FIXED_SEED);
    for (int i = 0; i < len; i++) {
        data[i] = rand();
        vector_push(v, &data[i]);
    }

    for (i = 0; i < len; i++) {
        ck_assert(data[i] == *(int*)vector_get(v, i));
    }

    for (i = 0, j = len-1; i < len; i++, j--) {
        vector_set(v, i, (void*)&data[j]);
    }

    for (i = 0, j = len-1; i < len; i++, j--) {
        ck_assert(data[i] == *(int*)vector_get(v, j));
    }
} END_TEST

START_TEST(swapping) {
    int len = 3;
    int data[len];
    int i, j;
    vector *v = make_vector(sizeof(int));

    srand(FIXED_SEED);
    for (int i = 0; i < len; i++) {
        data[i] = rand();
        vector_push(v, &data[i]);
    }

    for (i = 0, j = len-1; i < len-1; i++, j--) {
        vector_swap(v, i, j);
    }

    for (i = 0, j = len-1; i < len; i++, j--) {
        ck_assert(data[i] == *(int*)vector_get(v, j));
    }
} END_TEST

Suite * make_vector_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Vector");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, vector_setup, vector_teardown);
    tcase_add_test(tc_core, development_target);
    tcase_add_test(tc_core, get_and_set);
    tcase_add_test(tc_core, swapping);
    suite_add_tcase(s, tc_core);

    return s;
}

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <check.h>

#include "../check_check.h"

#include "../../simulation/min_heap.h"

void mheap_setup(void) {
};

void mheap_teardown(void) {
};

START_TEST(development_target) {
    int data = 42;
    int data2 = 43;
    int data3 = 44;
    mheap *h = make_mheap();
    mheap_push(h, &data, 10);
    mheap_push(h, &data, 9);
    mheap_push(h, &data2, 8);
    mheap_push(h, &data, 20);
    int *new_data;
    int priority;

    mheap_peek(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 43);
    ck_assert(priority == 8);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 43);
    ck_assert(priority == 8);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 9);
    priority = -1;
    new_data = NULL;

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 10);
    priority = -1;
    new_data = NULL;

    mheap_push(h, &data3, 0);
    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 44);
    ck_assert(priority == 0);

    mheap_pop(h, (void*)&new_data, &priority);
    ck_assert(*new_data == 42);
    ck_assert(priority == 20);

    destroy_mheap(h);
} END_TEST

START_TEST(big_insert_with_interspersed_pops) {
    const int len = 10000;
    int data[len*2];
    int last_p, last_pp;
    int cur_p, cur_d;
    int total_popped;
    int total_pushed;

    srand(FIXED_SEED);
    mheap *h = make_mheap();
    for (int i = 0; i < len; i++) {
        data[i] = rand();
        mheap_push(h, (void*)&data[i], data[i]);
    }

    last_p = INT_MAX;
    total_popped = 0;
    total_pushed = h->length;
    while(h->length > 0) {
        mheap_pop(h, (void*)&cur_d, &cur_p);
        ck_assert((cur_p <= last_p) || (cur_p == last_pp));
        total_popped += 1;
        if (total_popped <= len) {
            last_pp = rand();
            data[total_pushed] = last_pp;
            mheap_push(h, (void*)&data[total_pushed], data[total_pushed]);
            total_pushed++;
        }
    }
    ck_assert(total_popped == len*2);

} END_TEST

Suite * make_mheap_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Min Heap");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, mheap_setup, mheap_teardown);
    tcase_add_test(tc_core, development_target);
    tcase_add_test(tc_core, big_insert_with_interspersed_pops);
    suite_add_tcase(s, tc_core);

    return s;
}

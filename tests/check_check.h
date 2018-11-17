#ifndef CHECK_CHECK_H
#define CHECK_CHECK_H

Suite *make_mheap_suite(void);
Suite *make_vector_suite(void);
Suite *make_simulation_suite(void);
Suite *make_chemistry_suite(void);

#define FIXED_SEED 123456

#endif

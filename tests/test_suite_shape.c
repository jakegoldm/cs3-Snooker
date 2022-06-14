#include "list.h"
#include "shape.h"
#include "test_util.h"
#include <assert.h>
#include <stdlib.h>

void verify_square(list_t *square, int radius) {
  assert(vec_equal((vector_t){(int)((vector_t *)list_get(square, 0))->x,
                              (int)((vector_t *)list_get(square, 0))->y},
                   (vector_t){1 * radius, 0}));
  assert(vec_equal((vector_t){(int)((vector_t *)list_get(square, 1))->x,
                              (int)((vector_t *)list_get(square, 1))->y},
                   (vector_t){0, 1 * radius}));
  assert(vec_equal((vector_t){(int)((vector_t *)list_get(square, 2))->x,
                              (int)((vector_t *)list_get(square, 2))->y},
                   (vector_t){-1 * radius, 0}));
  assert(vec_equal((vector_t){(int)((vector_t *)list_get(square, 3))->x,
                              (int)((vector_t *)list_get(square, 3))->y},
                   (vector_t){0, -1 * radius}));
}

void test_draw_shape() {
  int radius = 2;
  vector_t *c = malloc(sizeof(vector_t));
  *c = VEC_ZERO;
  list_t *square = draw_star(2, c, radius, radius);
  verify_square(square, radius);

  list_free(square);
  free(c);
}

void test_shape() {
  int radius = 1;
  vector_t *c = malloc(sizeof(vector_t));
  *c = VEC_ZERO;
  list_t *shape = draw_star(2, c, radius, radius);
  verify_square(shape, radius);

  list_free(shape);
  free(c);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_draw_shape)
  DO_TEST(test_shape)

  puts("shape_test PASS");
}

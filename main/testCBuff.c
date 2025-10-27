#include "__dataStorage.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Mock định nghĩa một vài macro hệ thống nếu cần
#define OKE                 0
#define ERR_NULL            -1
#define ERR_INVALID_ARG     -2
#define ERR_MALLOC_FAILED   -3
#define ERR                 -4

#define __is_null(p)        ((p) == NULL)
#define __isnot_positive(x) ((x) <= 0)
#define DATA(x)             (*(x))   // như cách bạn dùng trong create()

#define __sys_entry(...)    printf("[ENTRY] " __VA_ARGS__), printf("\n")
#define __sys_exit(...)     printf("[EXIT ] " __VA_ARGS__), printf("\n")
#define __sys_warn(...)     printf("[WARN ] " __VA_ARGS__), printf("\n")
#define __sys_err(...)      printf("[ERROR] " __VA_ARGS__), printf("\n")

#define portMUX_TYPE int
#define portMUX_INITIALIZER_UNLOCKED 0
typedef int flag_t;

// Giả lập malloc fail test
static int malloc_fail_after = -1;
void* my_malloc(size_t size) {
    if (malloc_fail_after == 0) return NULL;
    if (malloc_fail_after > 0) malloc_fail_after--;
    return malloc(size);
}
#define malloc(x) my_malloc(x)

////////////////////////////////////////////////////////////////////////////////

void test_cBuffCreate(void) {
    printf("\n=== TEST cBuffCreate ===\n");
    circularBuffer_t *cb = NULL;

    // Case 1: NULL pointer
    assert(cBuffCreate(NULL, 8) == ERR_INVALID_ARG);

    // Case 2: size = 0 (default = 64)
    assert(cBuffCreate(&cb, 0) == OKE);
    assert(cb->maxSize == 64);
    free(cb->buffer); free(cb);

    // Case 3: malloc fail on object
    malloc_fail_after = 0;
    assert(cBuffCreate(&cb, 8) == ERR_MALLOC_FAILED);
    malloc_fail_after = -1;

    // Case 4: malloc fail on buffer
    malloc_fail_after = 1;
    assert(cBuffCreate(&cb, 8) == ERR_MALLOC_FAILED);
    malloc_fail_after = -1;
}

////////////////////////////////////////////////////////////////////////////////

void test_cBuffClear(void) {
    printf("\n=== TEST cBuffClear ===\n");
    circularBuffer_t cb = {0};
    cb.buffer = malloc(8 * sizeof(cBuffData_t));
    cb.maxSize = 8;
    cb.curSize = 5;
    cb.head = 2;
    cb.tail = 6;

    // Case 1: valid clear
    assert(cBuffClear(&cb) == OKE);
    assert(cb.curSize == 0);
    assert(cb.head == 0 && cb.tail == 0);

    // Case 2: NULL
    assert(cBuffClear(NULL) == ERR_NULL);
    free(cb.buffer);
}

////////////////////////////////////////////////////////////////////////////////

void test_cBuffPushBack(void) {
    printf("\n=== TEST cBuffPushBack ===\n");
    circularBuffer_t cb = {0};
    cBuffData_t buf[4];
    cb.buffer = buf;
    cb.maxSize = 4;

    // Case 1: NULL
    assert(cBuffPushBack(NULL, 123) == ERR_NULL);

    // Case 2: push normally
    for (int i = 0; i < 4; i++) cBuffPushBack(&cb, i + 1);
    assert(cb.curSize == 4);

    // Case 3: push overwrite (head advances)
    size_t old_head = cb.head;
    cBuffPushBack(&cb, 99);
    assert(cb.head == (old_head + 1) % cb.maxSize);
}

////////////////////////////////////////////////////////////////////////////////

void test_cBuffPushFront(void) {
    printf("\n=== TEST cBuffPushFront ===\n");
    circularBuffer_t cb = {0};
    cBuffData_t buf[4];
    cb.buffer = buf;
    cb.maxSize = 4;

    // Case 1: NULL
    assert(cBuffPushFront(NULL, 88) == ERR_NULL);

    // Case 2: push normally
    for (int i = 0; i < 4; i++) cBuffPushFront(&cb, i + 1);
    assert(cb.curSize == 4);

    // Case 3: push overwrite (tail retreats)
    size_t old_tail = cb.tail;
    cBuffPushFront(&cb, 99);
    assert(cb.tail == ((old_tail - 1 + cb.maxSize) % cb.maxSize));
}

////////////////////////////////////////////////////////////////////////////////

void test_cBuffPopBack(void) {
    printf("\n=== TEST cBuffPopBack ===\n");
    circularBuffer_t cb = {0};
    cBuffData_t buf[4];
    cBuffData_t out = 0;
    cb.buffer = buf;
    cb.maxSize = 4;

    // Case 1: NULL
    assert(cBuffPopBack(NULL, &out) == ERR_NULL);

    // Case 2: empty
    assert(cBuffPopBack(&cb, &out) == ERR);

    // Case 3: normal pop
    for (int i = 0; i < 4; i++) cBuffPushBack(&cb, i + 1);
    assert(cBuffPopBack(&cb, &out) == OKE);
}

////////////////////////////////////////////////////////////////////////////////

void test_cBuffPopFront(void) {
    printf("\n=== TEST cBuffPopFront ===\n");
    circularBuffer_t cb = {0};
    cBuffData_t buf[4];
    cBuffData_t out = 0;
    cb.buffer = buf;
    cb.maxSize = 4;

    // Case 1: NULL
    assert(cBuffPopFront(NULL, &out) == ERR_NULL);

    // Case 2: empty
    assert(cBuffPopFront(&cb, &out) == ERR);

    // Case 3: normal pop
    for (int i = 0; i < 4; i++) cBuffPushBack(&cb, i + 1);
    assert(cBuffPopFront(&cb, &out) == OKE);
}

////////////////////////////////////////////////////////////////////////////////

int main(void) {
    test_cBuffCreate();
    test_cBuffClear();
    test_cBuffPushBack();
    test_cBuffPushFront();
    test_cBuffPopBack();
    test_cBuffPopFront();

    printf("\n✅ All tests passed!\n");
    return 0;
}

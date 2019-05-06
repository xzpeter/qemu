// SPDX-License-Identifier: GPL-2.0
/*
 * Bitmap.c unit-tests.
 *
 * Copyright (C) 2019, Red Hat, Inc.
 *
 * Author: Peter Xu <peterx@redhat.com>
 */

#include <stdlib.h>
#include "qemu/osdep.h"
#include "qemu/bitmap.h"

static void check_bitmap_test(void)
{
    long size = 1024;
    unsigned long *bmap = bitmap_new(size);

    g_assert(bmap);
    g_assert(bitmap_test(bmap, size) == false);

    bitmap_set(bmap, 120, 80);

    g_assert(bitmap_test(bmap, 120) == false);
    g_assert(bitmap_test(bmap + BIT_WORD(120), BITS_PER_LONG) == true);
    g_assert(bitmap_test(bmap + BIT_WORD(120) + 1, 80) == true);
    g_assert(bitmap_test(bmap + BIT_WORD(200) + 1, 200) == false);

    bitmap_clear(bmap, 120, 80);

    g_assert(bitmap_test(bmap + BIT_WORD(120), BITS_PER_LONG) == false);
    g_assert(bitmap_test(bmap + BIT_WORD(120) + 1, 80) == false);

    g_free(bmap);
}

#define BMAP_SIZE  1024

static void check_bitmap_copy_with_offset(void)
{
    int i;
    unsigned long *bmap1, *bmap2, *bmap3, *bmap4, total;

    bmap1 = bitmap_new(BMAP_SIZE);
    bmap2 = bitmap_new(BMAP_SIZE);
    bmap3 = bitmap_new(BMAP_SIZE);
    bmap4 = bitmap_new(BMAP_SIZE);

    *bmap1 = random();
    *(bmap1 + 1) = random();
    *(bmap1 + 2) = random();
    *(bmap1 + 3) = random();
    total = BITS_PER_LONG * 4;

    bitmap_copy_with_dst_offset(bmap2, bmap1, 115, total);
    bitmap_copy_with_dst_offset(bmap3, bmap2, 85, total + 115);
    bitmap_copy_with_src_offset(bmap2, bmap3, 200, total);
    bitmap_copy_and_clear_with_offset_atomic(bmap4, bmap3, 200, total);

    for (i = 0; i < 3; i++) {
        g_assert(*(bmap1 + i) == *(bmap2 + i));
        g_assert(*(bmap1 + i) == *(bmap4 + i));
    }

    bitmap_clear(bmap1, 0, BMAP_SIZE);
    /* Set bits in bmap1 are 100-245 */
    bitmap_set(bmap1, 100, 145);

    /* Set bits in bmap2 are 60-205 */
    bitmap_copy_with_src_offset(bmap2, bmap1, 40, 250);
    for (i = 0; i < 60; i++) {
        g_assert(test_bit(i, bmap2) == 0);
    }
    for (i = 60; i < 205; i++) {
        g_assert(test_bit(i, bmap2));
    }
    g_assert(test_bit(205, bmap2) == 0);

    /* Set bits in bmap3 are 135-280 */
    bitmap_copy_with_dst_offset(bmap3, bmap1, 35, 250);
    for (i = 0; i < 135; i++) {
        g_assert(test_bit(i, bmap3) == 0);
    }
    for (i = 135; i < 280; i++) {
        g_assert(test_bit(i, bmap3));
    }
    g_assert(test_bit(280, bmap3) == 0);

    /* Set bits in bmap4 are 60-205 */
    bitmap_copy_and_clear_with_offset_atomic(bmap4, bmap1, 40, 250);
    for (i = 0; i < 60; i++) {
        g_assert(test_bit(i, bmap4) == 0);
    }
    for (i = 60; i < 205; i++) {
        g_assert(test_bit(i, bmap4));
    }
    g_assert(test_bit(205, bmap4) == 0);
    for (i = 0; i < BMAP_SIZE; i++) {
        g_assert(test_bit(i, bmap1) == 0);
    }

    g_free(bmap1);
    g_free(bmap2);
    g_free(bmap3);
    g_free(bmap4);
}

int main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/bitmap/bitmap_test", check_bitmap_test);
    g_test_add_func("/bitmap/bitmap_copy_with_offset",
                    check_bitmap_copy_with_offset);

    g_test_run();

    return 0;
}

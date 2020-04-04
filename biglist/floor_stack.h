#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define depth_track_hi_16(ptr) ((uint16_t *)offset(ptr, 2))
#define depth_track_lo_16(ptr) ((uint16_t *)ptr)
#else
#define depth_track_hi_16(ptr) ((uint16_t *)ptr)
#define depth_track_lo_16(ptr) ((uint16_t *)offset(ptr, 2))
#endif

typedef union {
    uint32_t int_32;
    uint16_t int_16;
} uint_32_16;

typedef struct {
    int content_sizeof;
    uint_32_16 depth_track;
} floor_stack_Stack;

static inline void floor_stack_make(int content_sizeof,
                                    floor_stack_Stack **out_stack,
                                    void **out_floor);

static inline void floor_stack_push(floor_stack_Stack *stack,
                                    void **out_content);

static inline void floor_stack_peak(floor_stack_Stack *stack,
                                    void **out_content);

static inline void floor_stack_peak_floor(floor_stack_Stack *stack,
                                          void **out_content);

static inline void floor_stack_pop_maybe(floor_stack_Stack *stack);


static inline void floor_stack_make(int content_sizeof,
                                    floor_stack_Stack **out_stack,
                                    void **out_floor)
{
    *out_stack = calloc(1, sizeof(floor_stack_Stack)
        + (uint16_t)~0 * content_sizeof);
    (*out_stack)->content_sizeof = content_sizeof;


    *depth_track_hi_16(&(*out_stack)->depth_track) = 1;
    *depth_track_lo_16(&(*out_stack)->depth_track) = 0;

    floor_stack_peak(*out_stack, out_floor);
}

static inline void floor_stack_push(floor_stack_Stack *stack,
                                    void **out_content)
{
    ++stack->depth_track.int_32;
    floor_stack_peak(stack, out_content);
}

static inline void floor_stack_peak(floor_stack_Stack *stack,
                                    void **out_content)
{
    *out_content = offset(stack, sizeof(floor_stack_Stack)
        + *depth_track_lo_16(&stack->depth_track) * stack->content_sizeof);
}

static inline void floor_stack_peak_floor(floor_stack_Stack *stack,
                                          void **out_content)
{
    *out_content = offset(stack, sizeof(floor_stack_Stack));
}

static inline void floor_stack_pop_maybe(floor_stack_Stack *stack)
{
    /* Trick to decrement 16 bit integer, unless it would underflow. */
    /* Relies on higher 16 bits being == 1. */
    --stack->depth_track.int_32;
    *depth_track_lo_16(&stack->depth_track) -= *depth_track_hi_16(&stack->depth_track);
    *depth_track_lo_16(&stack->depth_track) += 1;
    *depth_track_hi_16(&stack->depth_track) = 1;
}
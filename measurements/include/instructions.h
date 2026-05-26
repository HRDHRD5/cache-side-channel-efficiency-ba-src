

#ifndef CBSC_MEASUREMENT_INSTRUCTIONS_H
#define CBSC_MEASUREMENT_INSTRUCTIONS_H

// from l1tf_reloaded/include/asm.h
// accessible at https://github.com/ThijsRay/l1tf_reloaded
// from commit 340ab9ca33b7be5d9438dbf5834933292f8b72b6
// variables may be renamed
static inline __attribute__((always_inline)) void clflush(void *p) {
  __asm__ volatile("clflush (%0)\n" ::"r"(p));
}

// from l1tf_reloaded/include/asm.h
// accessible at https://github.com/ThijsRay/l1tf_reloaded
// from commit 340ab9ca33b7be5d9438dbf5834933292f8b72b6
// variables may be renamed
static inline __attribute__((always_inline)) void maccess(void *ptr) {
  __asm__ volatile("movb (%0), %%al\n" : : "r"(ptr) : "al");
}

static inline __attribute__((always_inline)) void prefetch(void *ptr) {
  __asm__ volatile("prefetcht0 (%0)\n" : : "r"(ptr) : "al");
}

// from l1tf_reloaded/include/asm.h
// accessible at https://github.com/ThijsRay/l1tf_reloaded
// from commit 340ab9ca33b7be5d9438dbf5834933292f8b72b6
// variables may be renamed
static inline __attribute__((always_inline)) void lfence(void) { __asm__ volatile("lfence\n"); }

// from l1tf_reloaded/include/asm.h
// accessible at https://github.com/ThijsRay/l1tf_reloaded
// from commit 340ab9ca33b7be5d9438dbf5834933292f8b72b6
// variables may be renamed
static inline __attribute__((always_inline)) void mfence(void) { __asm__ volatile("mfence\n"); }

// from l1tf_reloaded/include/asm.h
// This function is originally from figure 4 of Yarom and Falkner, “FLUSH+RELOAD: A High Resolution, Low Noise,
// L3 Cache Side-Channel Attack.”
// accessible at https://github.com/ThijsRay/l1tf_reloaded
// from commit 340ab9ca33b7be5d9438dbf5834933292f8b72b6
// variables may be renamed
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

// flush_flush/sc/cacheutils.h
// accessible at https://github.com/isec-tugraz/flush_flush
// from commit 37a3ca725e34cedfc150051e0ddf262700bb7588
// variables may be renamed
static inline size_t rdtsc() {
  size_t a, d;
  __asm__ volatile ("mfence");
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  a = (d<<32) | a;
  __asm__ volatile ("mfence");
  return a;
}

#endif
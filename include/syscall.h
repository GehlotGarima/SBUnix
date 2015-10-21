#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

#define T_SYSCALL   128

static __inline uint64_t syscall_0(uint64_t n) {

	uint64_t a = -1;  
    __asm volatile("int $0x80" : "=a" (a) : "0" (n));  
    return a;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
	uint64_t val;                                                                                                                          
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rbx;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1)
                   :"rbp","rcx","rdx","rsi","rdi","r12","r11","r10","r9","r8","r13","r14","r15"//Clobberred registers
                  );
   return val;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	uint64_t res;
    __asm__ volatile ("int %1"\
                    :"=a"(res)\
                    :"i"(T_SYSCALL),"0"(n) ,"b"((uint64_t)(a1)),"c"((uint64_t)(a2))\
                    :"cc","memory");
    return res;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t a; 
    __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3)); 
    return a;


}

#endif

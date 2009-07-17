#ifndef SECURE_MEM_H__
#define SECURE_MEM_H__

#include <stdlib.h>

namespace playground {

class SecureMem {
 public:
  typedef struct Args {
    union {
      struct {
        struct Args* self;
        long  sequence;
        long  syscallNum;
        void* arg1;
        void* arg2;
        void* arg3;
        void* arg4;
        void* arg5;
        void* arg6;
        void* ret;
        void* rbp;
        void* rbx;
        void* rcx;
        void* rdx;
        void* rsi;
        void* rdi;
        void* r8;
        void* r9;
        void* r10;
        void* r11;
        void* r12;
        void* r13;
        void* r14;
        void* r15;
        void* secureCradle;
        long  processFd;
        long  cloneFd;
        char  filename[24];
      } __attribute__((packed));
      char securePage[4096];
    };
    union {
      struct {
        long  tmpSyscallNum;
        void* tmpArg1;
        void* tmpArg2;
        void* tmpArg3;
        void* tmpArg4;
        void* tmpArg5;
        void* tmpArg6;
        void* tmpReturnValue;
      } __attribute__((packed));
      char scratchPage[4096];
    };
  } __attribute__((packed)) Args;

  static void abandonSystemCall(int fd, int err);
  static void lockSystemCall(Args* mem);
  static void sendSystemCall(int fd, bool locked, Args* mem,
                             int syscallNum) {
    sendSystemCallInternal(fd, locked, mem, syscallNum);
  }
  template<class T1> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1);
  }
  template<class T1, class T2> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1, T2 arg2) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1,
                           (void*)arg2);
  }
  template<class T1, class T2, class T3> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1, T2 arg2, T3 arg3) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1,
                           (void*)arg2, (void*)arg3);
  }
  template<class T1, class T2, class T3, class T4> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1,
                           (void*)arg2, (void*)arg3, (void*)arg4);
  }
  template<class T1, class T2, class T3, class T4, class T5> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1, T2 arg2, T3 arg3, T4 arg4,
                      T5 arg5) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1,
                           (void*)arg2, (void*)arg3, (void*)arg4,
                           (void*)arg5);
  }
  template<class T1, class T2, class T3, class T4, class T5, class T6> static
  void sendSystemCall(int fd, bool locked, Args* mem,
                      int syscallNum, T1 arg1, T2 arg2, T3 arg3, T4 arg4,
                      T5 arg5, T6 arg6) {
    sendSystemCallInternal(fd, locked, mem, syscallNum, (void*)arg1,
                           (void*)arg2, (void*)arg3, (void*)arg4,
                           (void*)arg5, (void*)arg6);
  }

 private:
  static void sendSystemCallInternal(int fd, bool locked, Args* mem,
                                     int syscallNum, void* arg1 = 0,
                                     void* arg2 = 0, void* arg3 = 0,
                                     void* arg4 = 0, void* arg5 = 0,
                                     void* arg6 = 0);
};

} // namespace

#endif // SECURE_MEM_H__

// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "debug.h"
#include "mutex.h"
#include "securemem.h"

namespace playground {

void SecureMem::abandonSystemCall(const SyscallRequestInfo& rpc, long err) {
  void* rc = reinterpret_cast<void *>(err);
  if (err) {
    Debug::message("System call failed\n");
  }
  Sandbox::SysCalls sys;
  if (Sandbox::write(sys, rpc.applicationFd, &rc, sizeof(rc)) != sizeof(rc)) {
    Sandbox::die("Failed to send system call");
  }
}

void SecureMem::dieIfParentDied(int parentMapsFd) {
  // The syscall_mutex_ should not be contended. If it is, we are either
  // experiencing a very unusual load of system calls that the sandbox is not
  // optimized for; or, more likely, the sandboxed process terminated while the
  // trusted process was in the middle of waiting for the mutex. We detect
  // this situation and terminate the trusted process.
  int alive = !lseek(parentMapsFd, 0, SEEK_SET);
  if (alive) {
    char buf;
    do {
      alive = read(parentMapsFd, &buf, 1);
    } while (alive < 0 && errno == EINTR);
  }
  if (!alive) {
    Sandbox::die();
  }
}

void SecureMem::lockSystemCall(const SyscallRequestInfo& rpc) {
  while (!Mutex::lockMutex(&Sandbox::syscall_mutex_, 500)) {
    dieIfParentDied(rpc.parentMapsFd);
  }
  asm volatile(
  #if defined(__x86_64__)
      "lock; incq (%0)\n"
  #elif defined(__i386__)
      "lock; incl (%0)\n"
  #else
  #error Unsupported target platform
  #endif
      :
      : "q"(&rpc.mem->sequence)
      : "memory");
}

void SecureMem::sendSystemCallInternal(const SyscallRequestInfo& rpc,
                                       bool locked,
                                       void* arg1, void* arg2, void* arg3,
                                       void* arg4, void* arg5, void* arg6) {
  if (!locked) {
    asm volatile(
    #if defined(__x86_64__)
        "lock; incq (%0)\n"
    #elif defined(__i386__)
        "lock; incl (%0)\n"
    #else
    #error Unsupported target platform
    #endif
        :
        : "q"(&rpc.mem->sequence)
        : "memory");
  }
  rpc.mem->callType    = locked ? -2 : -1;
  rpc.mem->syscallNum  = rpc.sysnum;
  rpc.mem->arg1        = arg1;
  rpc.mem->arg2        = arg2;
  rpc.mem->arg3        = arg3;
  rpc.mem->arg4        = arg4;
  rpc.mem->arg5        = arg5;
  rpc.mem->arg6        = arg6;
  asm volatile(
  #if defined(__x86_64__)
      "lock; incq (%0)\n"
  #elif defined(__i386__)
      "lock; incl (%0)\n"
  #else
  #error Unsupported target platform
  #endif
      :
      : "q"(&rpc.mem->sequence)
      : "memory");
  Sandbox::SysCalls sys;
  if (Sandbox::write(sys, rpc.trustedThreadFd, &rpc.mem->callType,
                     sizeof(int)) != sizeof(int)) {
    Sandbox::die("Failed to send system call");
  }
  if (locked) {
    while (!Mutex::waitForUnlock(&Sandbox::syscall_mutex_, 500)) {
      dieIfParentDied(rpc.parentMapsFd);
    }
  }
}

} // namespace

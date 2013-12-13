/* Copyright (c) 2013,  Regents of the Columbia University 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __TERN_COMMON_RUNTIME_STAT_H
#define __TERN_COMMON_RUNTIME_STAT_H
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#define nMaxThreads 256
uint32_t *threadLocalTurnNum=NULL;
namespace tern {
class RuntimeStat {
public:
/*
	unsigned long threadLocalTurnNum[nMaxThreads];
	unsigned long threadLocalNextTurnNum[nMaxThreads];
*/
	int shmid;
  long globalTurnNum;
  long nDetPthreadSyncOp; /* Number of deterministic pthread sync operations called (excluded idle thread and non-det sync operations).*/
  long nInterProcSyncOp;/* Number of inter-process sync operations called (networks, signals, wait, fork is scheduled by us and counted as nDetPthreadSyncOp).*/
  long nLineupSucc; /* Number of successful lineup operations (if multiple threads lineup and succeed for once, count as 1). */
  long nLineupTimeout; /* Number of lineup timeouts. */
  long nNonDetRegions;  /* Number of times all threads entering the non-det regions (and exiting the regions must be the same value). */
  long nNonDetPthreadSync; /* Number of non-det pthread sync operations called within a non-det region. */
  
public:
  ~RuntimeStat(){
	  //The program is ending, thus the turn will be useless for thread sanitizer. Uniformize them into a same number
	unsigned int max=0;
	for(int i=0;i<2*nMaxThreads;++i){
		if(threadLocalTurnNum[i]>max) max=threadLocalTurnNum[i];
	}
	for(int i=0;i<2*nMaxThreads;++i) threadLocalTurnNum[i]=max;
#ifdef TSAN1
	  //detach itself
  	if(shmdt(threadLocalTurnNum)<0){
		perror("[Shared Memory] shmdt");
		exit(1);
	}
	//try destroy the shared memory segmetn
	if(shmctl(shmid,IPC_RMID,NULL)<0){
		perror("[Shared Memory] shmctl, IPCS_RMID");
		exit(1);
	}
	//here we will use local memory for remaining useless turns
	threadLocalTurnNum=new uint32_t[nMaxThreads*2];
#endif
  }
  RuntimeStat() {
#ifdef TSAN1
	//get the pid of the process, this will be used as a key for the shared memory
	key_t key=getpid();
	size_t sz=sizeof(uint32_t)*nMaxThreads*2;
	//grant the permission only to the owner
    shmid=shmget(key,sz,IPC_CREAT | 0600 );
	if(shmid<0){
		perror("[Shared Memory] shmget");
		exit(1);
	}
	void* ret=shmat(shmid,NULL,0);
	if(ret==(void*)-1){
		perror("[Shared Memory] shmat");
		exit(1);
	}
	threadLocalTurnNum=static_cast<uint32_t*>(ret);
	//initialization of the shared memory
	  for(int i=0;i<nMaxThreads;++i){
		  threadLocalTurnNum[2*i]=0;
		  threadLocalTurnNum[2*i+1]=0;
	  }
#else
	  threadLocalTurnNum=new uint32_t[2*nMaxThreads];
#endif
    nDetPthreadSyncOp = 0;
    nInterProcSyncOp = 0;
    nLineupSucc = 0;
    nLineupTimeout = 0;
    nNonDetRegions = 0;
    nNonDetPthreadSync = 0;    
	globalTurnNum = 0;
  }
  void print() {
    std::cout << "\n\nRuntimeStat:\n"
      << "nDetPthreadSyncOp\t" << "nInterProcSyncOp\t" << "nLineupSucc\t" << "nLineupTimeout\t" << "nNonDetRegions\t" << "nNonDetPthreadSync\t" << "\n"    
      << "RUNTIME_STAT: "
      << nDetPthreadSyncOp << "\t" << nInterProcSyncOp << "\t" << nLineupSucc << "\t" << nLineupTimeout << "\t" << nNonDetRegions << "\t" << nNonDetPthreadSync
      << "\n\n" << std::flush;
  }

};

}

#endif


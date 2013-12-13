#ifndef TS_PARROT_H
#define TS_PARROT_H
#ifndef NULL
#define NULL 0
#endif
	template<typename TyRet, typename TyArg0, typename TyArg1, typename TyArg2, typename TyArg3, typename TyArg4, typename TyArg5>
	TyRet ParrotCall(void* fun, TyArg0 arg0, TyArg1 arg1, TyArg2 arg2, TyArg3 arg3, TyArg4 arg4, TyArg5 arg5) {
		return ((TyRet (*)(TyArg0, TyArg1, TyArg2, TyArg3, TyArg4, TyArg5))fun)(arg0, arg1, arg2, arg3 ,arg4, arg5);
	}
	template<typename TyRet, typename TyArg0, typename TyArg1, typename TyArg2, typename TyArg3, typename TyArg4>
	TyRet ParrotCall(void* fun, TyArg0 arg0, TyArg1 arg1, TyArg2 arg2, TyArg3 arg3, TyArg4 arg4) {
		return ((TyRet (*)(TyArg0, TyArg1, TyArg2, TyArg3, TyArg4))fun)(arg0, arg1, arg2, arg3 ,arg4);
	}
	template<typename TyRet, typename TyArg0, typename TyArg1, typename TyArg2, typename TyArg3>
	TyRet ParrotCall(void* fun, TyArg0 arg0, TyArg1 arg1, TyArg2 arg2, TyArg3 arg3) {
		return ((TyRet (*)(TyArg0, TyArg1, TyArg2, TyArg3))fun)(arg0, arg1, arg2, arg3);
	}
	template<typename TyRet, typename TyArg0, typename TyArg1, typename TyArg2>
	TyRet ParrotCall(void* fun, TyArg0 arg0, TyArg1 arg1, TyArg2 arg2) {
		return ((TyRet (*)(TyArg0, TyArg1, TyArg2))fun)(arg0, arg1, arg2);
	}
	template<typename TyRet, typename TyArg0, typename TyArg1>
	TyRet ParrotCall(void* fun, TyArg0 arg0, TyArg1 arg1) {
		return ((TyRet (*)(TyArg0, TyArg1))fun)(arg0, arg1);
	}
	template<typename TyRet, typename TyArg0>
	TyRet ParrotCall(void* fun, TyArg0 arg0) {
		return ((TyRet (*)(TyArg0))fun)(arg0);
	}
	template<typename TyRet>
	TyRet ParrotCall(void* fun) {
		return ((TyRet (*)())fun)();
	}

	#define LOAD_PARROT \
		do{\
			if(parrot_handle==NULL)\
		  		parrot_handle=REAL(dlopen)("/home/xinan/work/xtern/dync_hook/interpose.so",RTLD_LAZY);\
		CHECK_NE(parrot_handle,NULL);\
		}while(0)
	
	#define PARROT(func) Parrot_##func
	#define PARROT_DECLARE(func) \
		static void* PARROT(func) = NULL

	#define PARROT_INTERCEPT(func) \
		LOAD_PARROT;\
		CHECK_NE(parrot_handle,NULL);\
		if(PARROT(func)==NULL){\
			PARROT(func)=dlsym(parrot_handle,#func);\
			CHECK_NE(PARROT(func),NULL);\
			Printf("[PARROT-TSAN DEBUG] Hooked -- " #func " --\n");\
		}
	#define PARROT_CALL(ret,func,...) \
		ParrotCall<ret>(PARROT(func),##__VA_ARGS__)



	void* parrot_handle = NULL;
	PARROT_DECLARE(pthread_create);
	PARROT_DECLARE(pthread_cancel);//
	PARROT_DECLARE(pthread_join);
	PARROT_DECLARE(pthread_detach);
	PARROT_DECLARE(pthread_exit);//

	PARROT_DECLARE(pthread_mutex_init);
	PARROT_DECLARE(pthread_mutex_lock);//
	PARROT_DECLARE(pthread_mutex_timedlock);
	PARROT_DECLARE(pthread_mutex_trylock);
	PARROT_DECLARE(pthread_mutex_unlock);//

	PARROT_DECLARE(pthread_cond_wait);//
	PARROT_DECLARE(pthread_cond_timedwait);
	PARROT_DECLARE(pthread_cond_broadcast);//
	PARROT_DECLARE(pthread_cond_signal);//

	PARROT_DECLARE(sem_init);
	PARROT_DECLARE(sem_wait);
	PARROT_DECLARE(sem_post);
	PARROT_DECLARE(sem_trywait);
	PARROT_DECLARE(sem_timedwait);

	PARROT_DECLARE(pthread_barrier_wait);
	PARROT_DECLARE(pthread_barrier_init);
	PARROT_DECLARE(pthread_barrier_destroy);

	PARROT_DECLARE(pthread_rwlock_rdlock);
	PARROT_DECLARE(pthread_rwlock_tryrdlock);
	PARROT_DECLARE(pthread_rwlock_trywrlock);
	PARROT_DECLARE(pthread_rwlock_wrlock);
	PARROT_DECLARE(pthread_rwlock_unlock);

	PARROT_DECLARE(__libc_start_main);
	u32 *parrotTurnNum=NULL;
	typedef u32* (*get_func_t) ();

	get_func_t get_parrotTurnNum=NULL;
#endif

#pragma once
#include <atomic>
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			//原子计数类
			class AtomicCounter
			{
			private:
				mutable volatile atomic_type reference_counter;
			public:	
				inline AtomicCounter() {
					reference_counter = 0;
				}
				inline AtomicCounter(atomic_type value) {
					reference_counter = value;
				}
				inline operator atomic_type()const {
					return reference_counter;
				}
				AtomicCounter&operator=(atomic_type value) {
#ifdef _WIN64
					_InterlockedExchange64(&reference_counter, value);
#else
					_InterlockedExchange(&reference_counter, value);
#endif // X64
					return *this;
				}
				inline atomic_type operator++()const{
#ifdef _WIN64
					return _InterlockedIncrement64(&reference_counter);
#else
					return _InterlockedIncrement(&reference_counter);
#endif // X64
				}
				inline atomic_type operator--()const
				{
#ifdef _WIN64
					return _InterlockedDecrement64(&reference_counter);
#else
					return _InterlockedDecrement(&reference_counter);
#endif // X64
				}
				inline atomic_type operator++(int)const {
#ifdef _WIN64
					return _InterlockedIncrement64(&reference_counter);
#else
					return _InterlockedIncrement(&reference_counter);
#endif // X64
				}
				inline atomic_type operator--(int)const
				{
#ifdef _WIN64
					return _InterlockedDecrement64(&reference_counter);
#else
					return _InterlockedDecrement(&reference_counter);
#endif // X64
				}
				~AtomicCounter(){}
			};

			//读写锁
			class RWLock {
				SRWLOCK lock;
			public:
				RWLock() {
					InitializeSRWLock(&lock);
				}
				inline void AcquireWrite() {
					AcquireSRWLockExclusive(&lock);
				}
				inline void AcquireRead() {
					AcquireSRWLockShared(&lock);
				}
				inline void ReleaseWrite() {
					ReleaseSRWLockExclusive(&lock);
				}
				inline void ReleaseRead() {
					ReleaseSRWLockShared(&lock);
				}
				inline SRWLOCK*operator&() {
					return &lock;
				}
				inline SRWLOCK const*operator&()const {
					return &lock;
				}
				~RWLock() {
				}
			};

			//条件变量
			class ConditionVariable {
				friend class Thread;
				CONDITION_VARIABLE cond_var;
			public:
				ConditionVariable() {
					InitializeConditionVariable(&cond_var);
				}
				//唤醒所有等待的线程
				inline void WakeUpAll() {
					WakeAllConditionVariable(&cond_var);
				}
				//随机唤醒一个等待线程
				inline void WakeUpSingle() {
					WakeConditionVariable(&cond_var);
				}
				inline void Wait(RWLock const&root,unsigned long milisecs) {
					SleepConditionVariableSRW(&cond_var, &const_cast<RWLock&>(root), milisecs, 0);
				}
				~ConditionVariable() {
				}
			};

			//互斥锁
			class Mutex
			{
				void* mutex_ptr = nullptr;
			public:
				Mutex() {
					mutex_ptr = CreateMutexW(nullptr, false, nullptr);
				}
				inline void Lock()
				{
					WaitForSingleObject(mutex_ptr, INFINITE);
				}
				inline void Lock(DWORD milisecs) {
					WaitForSingleObject(mutex_ptr, milisecs);
				}
				inline void UnLock() {
					ReleaseMutex(mutex_ptr);
				}
				~Mutex() {
					if (mutex_ptr != nullptr)
					{
						CloseHandle(mutex_ptr);
						mutex_ptr = nullptr;
					}
				}
			};

			//自旋锁
			class SpinLock
			{
				std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
			public:
				inline void Lock() {
					while (m_flag.test_and_set(std::memory_order_acquire));
				}
				inline void Unlock() {
					m_flag.clear(std::memory_order_release);
				}
			};

			class Volatile
			{
				AtomicCounter m_operation_enter_counter = 0;
				AtomicCounter m_operation_exit_counter = 0;
				SpinLock m_enter;
			public:
				//进入操作临界区
				inline void OperationEnter() {
					this->m_enter.Lock();
					++this->m_operation_enter_counter;
					this->m_enter.Unlock();
				}
				//退出操作临界区
				inline void OperationExit() {
					++this->m_operation_exit_counter;
				}
				//尝试进行操作(宿主)
				inline void TryLock() {
					while (m_operation_enter_counter != m_operation_exit_counter)Sleep(0);
					this->m_enter.Lock();
				}
				//解锁(宿主)
				inline void UnLock() {
					this->m_enter.Unlock();
				}
			};
			
		}
	}
}
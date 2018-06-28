#pragma once
#include <process.h>
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			namespace Internal
			{
				struct thread_pack
				{
					Functional::Delegate<Functional::Auto> target_function;//Ŀ�꺯��
					Generic::UArray<UPointer::uobject> parameters;//����
					UThread self_reference;//������
					UPointer::uobject object_ptr;//����ָ��
					UPointer::uobject async_ret;//�첽����ֵ
				};
			}

			class Thread:public UPointer::UPtrThis<Thread>
			{
				friend class Task;
			private:
				HANDLE tid;
				Internal::thread_pack*pack = nullptr;
				static void TargetFun(void*args) {
					Internal::thread_pack*lptr = (Internal::thread_pack*)args;
					if (lptr->object_ptr.IsNull())
						lptr->async_ret = lptr->target_function.Invoke(lptr->parameters);
					else
						lptr->async_ret = lptr->target_function.UInvoke(lptr->object_ptr, lptr->parameters);
					lptr->self_reference = nullptr;
				}
			public:
				Thread(Functional::Delegate<Functional::Auto> const& Target,UPointer::uobject const& Object,Generic::UArray<UPointer::uobject> const& Args) {
					pack = new Internal::thread_pack;
					pack->target_function = Target;
					pack->parameters = Args;
					pack->object_ptr = Object;
				}
				//�����߳�
				inline void Start() {
					if (pack)
					{
						pack->self_reference = this->GetThisUPtr();
						tid = (void*)(_beginthread(TargetFun, 0, this->pack));
					}
				}
				inline void Join()const {
					WaitForSingleObject(tid, INFINITE);
				}
				inline void Wait(unsigned long MiliSeconds)const {
					WaitForSingleObject(tid, MiliSeconds);
				}
				Thread&operator=(Thread const&) = delete;
				Thread&operator=(Thread&&rhs) {
					this->~Thread();
					if (rhs.pack)
					{
						pack = rhs.pack;
						tid = rhs.tid;
						rhs.pack = nullptr;
					}
					return *this;
				}
				~Thread()
				{
					if (pack) {
						delete pack;
						pack = nullptr;
					}
				}
			};

			//�첽����ֵ
			template<class T>
			class AsyncResult
			{
				UThread thread;
				friend class Task;
			public:
				AsyncResult(UThread const&target_thread):thread(target_thread) {}
				AsyncResult(AsyncResult const&rhs) = default;
				AsyncResult&operator=(AsyncResult const&rhs) = default;
				~AsyncResult() = default;
			};

			//�첽����ֵ
			template<>
			class AsyncResult<void>
			{
				UThread thread;
				friend class Task;
			public:
				AsyncResult(UThread const&target_thread) :thread(target_thread) {}
				AsyncResult(AsyncResult const&rhs) = default;
				AsyncResult&operator=(AsyncResult const&rhs) = default;
				~AsyncResult() = default;
			};

			//���߳�
			class Task
			{
			public:
				template<class T>
				static T Await(AsyncResult<T>const&result) {
					if (result.thread.IsNull())
						HL::Exception::Throw<HL::Exception::ArgumentNullException>();
					result.thread->Join();
					return result.thread->pack->async_ret;
				}
				static void Await(AsyncResult<void> const&result) {
					if (result.thread.IsNull())
						HL::Exception::Throw<HL::Exception::ArgumentNullException>();
					result.thread->Join();
				}
				inline static void Delay(DWORD MilSec)
				{
					Sleep(MilSec);
				}
				template<class T>
				static uptr<Generic::Array<T>> Await(Iteration::IEnumerator<AsyncResult<T>>const& results)
				{
					auto ret = newptr<Generic::Array<T>>();
					for (auto&result : results)
						ret->Add(result);
					return ret;
				}
				//�첽ִ��
				template<class T,class...Args>
				static AsyncResult<T> Run(Functional::Delegate<Functional::Auto> const&target, Args const&...args) {
					UPointer::uptr<Threading::Thread> Thr = GC::newgc<Threading::Thread>(target, nullptr, params(args...));
					Thr->Start();
					return Thr;
				}
				//�첽ִ��
				template<class T, class...Args>
				static AsyncResult<T> Run(Functional::Delegate<Functional::Auto> const&target, UPointer::uobject const& object, Args const&...args) {
					UPointer::uptr<Threading::Thread> Thr = GC::newgc<Threading::Thread>(target, object, params(args...));
					Thr->Start();
					return Thr;
				}
				//�첽ִ��
				template<class...Args,class Functor>
				static AsyncResult<typename Template::GetFunctionInfo<Functor>::R> Run(Functor fx, Args const&...args)
				{
					Functional::Delegate<Functional::Auto>Func = Bind(fx);
					UPointer::uptr<Threading::Thread> Thr = GC::newgc<Threading::Thread>(Func, nullptr, Functional::params(args...));
					Thr->Start();
					return Thr;
				}
				//�첽ִ��
				template<class...Args, class TT, class Functor>
				static AsyncResult<typename Template::GetFunctionInfo<Functor>::R> Run(Functor fx, TT*obj, Args const&...args)
				{
					Functional::Delegate<Functional::Auto>Func = Bind(fx, obj);
					UPointer::uptr<Threading::Thread> Thr = GC::newgc<Threading::Thread>(Func, nullptr, Functional::params(args...));
					Thr->Start();
					return Thr;
				}
			};

		}
	}
}
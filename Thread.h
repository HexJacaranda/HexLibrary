#pragma once
#include <process.h>
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			//�̻߳��������ӿ�
			class IThread
			{
			public:
				typedef void* Handle;
				//�ȴ�����ʱ��
				static const unsigned long Infinite = 0xFFFFFFFF;
				//˯��
				static void SleepFor(unsigned long Miliseconds) {
					Sleep(Miliseconds);
				}
				//�����߳�
				template<class Fun>
				static Handle StartNativeThread(Fun Target, size_t StackSize, void*Args) {
					return (Handle)_beginthread(Target, StackSize, Args);
				}
				//�ȴ��߳�
				static void WaitFor(Handle Target, unsigned long Miliseconds) {
					WaitForSingleObject(Target, Miliseconds);
				}
				//�ر��߳�
				static void Close(Handle Target) {
					_endthread();
				}
			};

			//Task��
			class TaskPackage
			{
			public:
				//�첽���
				UPointer::uobject AsyncResult;
				//Ŀ�꺯��
				Functional::Delegate<Functional::Auto> Target;
				//��ǰ�߳�
				UPointer::uptr<Thread> CurrentThread;
				//��Ա��������ָ��
				UPointer::uobject ObjectPtr;
				//����
				Generic::UArray<UPointer::uobject> Arguments;

				TaskPackage(Functional::Delegate<Functional::Auto> const& TargetFun, UPointer::uobject const& Object, Generic::UArray<UPointer::uobject> const& Args) {
					Arguments = Args;
					ObjectPtr = Object;
					Target = TargetFun;
				}
			};

			//�߳���
			class Thread:public UPointer::UPtrThis<Thread>
			{
				IThread::Handle m_handle;
				UPointer::uptr<TaskPackage> m_task_package;

				static void Proxy(void*args) {
					UPointer::uptr<TaskPackage>*ptr = (UPointer::uptr<TaskPackage>*)args;
					uobject ret = nullptr;
					if (ptr->GetObjectPtr()->ObjectPtr.IsNull())
						ret = ptr->GetObjectPtr()->Target.Invoke(ptr->GetObjectPtr()->Arguments);
					else
						ret = ptr->GetObjectPtr()->Target.UInvoke(ptr->GetObjectPtr()->ObjectPtr, ptr->GetObjectPtr()->Arguments);
					ptr->GetObjectPtr()->AsyncResult = ret;
					ptr->GetObjectPtr()->CurrentThread = nullptr;
					ptr->SetToNull();
					IThread::Close(nullptr);
				}
			public:
				Thread(UPointer::uptr<TaskPackage> const&Target) :m_task_package(Target) {
				}
				Thread(Functional::Delegate<Functional::Auto> const& Target, UPointer::uobject const& Object, Generic::UArray<UPointer::uobject> const& Args) {
					m_task_package = Reference::newptr<TaskPackage>(Target, Object, Args);
				}
				void Go() {
					m_task_package->CurrentThread = this->GetThisUPtr();
					m_handle = IThread::StartNativeThread(Proxy, 0, &m_task_package);
				}
				//������ǰ�߳�ֱ������
				void Join() {
					IThread::WaitFor(m_handle, IThread::Infinite);
				}
				//����
				void Detach() {

				}
				//˯��
				static void Sleep(unsigned long Miliseconds){
					IThread::SleepFor(Miliseconds);
				}
				~Thread() {
					
				}
			};

			//Task
			template<class T>
			class Task
			{
				UPointer::uptr<TaskPackage> m_task_package;
			public:
				Task(UPointer::uptr<TaskPackage> const& Target) :m_task_package(Target) {
				}
				//����
				template<class Functor>
				UPointer::uptr<Task<typename Template::GetFunctionInfo<Functor>::R>> Then(Functor Target) {
					m_task_package->CurrentThread->Join();
					return Reference::newptr<Thread>(Functional::Bind(Target));
				}
				//�ȴ������첽���
				T Await() {
					m_task_package->CurrentThread->Join();
					return m_task_package->AsyncResult;
				}
			};
			template<>
			class Task<void>
			{
				UPointer::uptr<TaskPackage> m_task_package;
			public:
				Task(UPointer::uptr<TaskPackage> const& Target) :m_task_package(Target) {
				}
				//�ȴ��첽
				void Await() {
					m_task_package->CurrentThread->Join();
				}
				//����
				template<class Functor,class...Args>
				UPointer::uptr<Task<typename Template::GetFunctionInfo<Functor>::R>> Then(Functor Target,Args const&...args) {
					m_task_package->CurrentThread->Join();
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Functional::Bind(Target), nullptr, Functional::params(args...));
					GC::newgc<Thread>(package)->Go();
					return Reference::newptr<Task<typename Template::GetFunctionInfo<Functor>::R>>(package);
				}
				//����
				template<class T,class...Args>
				UPointer::uptr<Task<T>> Then(Functional::Delegate<Functional::Auto>const& Target, Args const&...args) {
					m_task_package->CurrentThread->Join();
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Target, nullptr, Functional::params(args...));
					GC::newgc<Thread>(package)->Go();
					return Reference::newptr<Task<T>>(package);
				}
				//����
				template<class Functor,class TT,class...Args>
				UPointer::uptr<Task<typename Template::GetFunctionInfo<Functor>::R>> ThenWith(Functor Target, TT*Object, Args const&...args) {
					m_task_package->CurrentThread->Join();
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Functional::Bind(Target, Object), nullptr, Functional::params(args...));
					GC::newgc<Thread>(package)->Go();
					return Reference::newptr<Task<typename Template::GetFunctionInfo<Functor>::R>>(package);
				}
				template<class T,class...Args>
				UPointer::uptr<Task<T>> ThenWith(Functional::Delegate<Functional::Auto> const&Target, UPointer::uobject const&Object, Args const&...args) {
					m_task_package->CurrentThread->Join();
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Target, Object, Functional::params(args...));
					GC::newgc<Thread>(package)->Go();
					return Reference::newptr<Task<T>>(package);
				}
				//�첽ִ��
				template<class T,class...Args>
				static UPointer::uptr<Task<T>> RunVia(Functional::Delegate<Functional::Auto> const&Target, Args const&...args) {
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Target, nullptr, Functional::params(args...));
					GC::newgc<Threading::Thread>(package)->Go();
					return Reference::newptr<Task<T>>(package);
				}
				//�첽ִ��
				template<class T, class...Args>
				static UPointer::uptr<Task<T>> RunViaWith(Functional::Delegate<Functional::Auto> const&Target, UPointer::uobject const& Object, Args const&...args) {
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Target, Object, Functional::params(args...));
					GC::newgc<Threading::Thread>(package)->Go();
					return Reference::newptr<Task<T>>(package);
				}
				//�첽ִ��
				template<class Functor,class...Args>
				static UPointer::uptr<Task<typename Template::GetFunctionInfo<Functor>::R>> Run(Functor Target,Args const&...args) {
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Functional::Bind(Target), nullptr, Functional::params(args...));
					GC::newgc<Threading::Thread>(package)->Go();
					return Reference::newptr<Task<typename Template::GetFunctionInfo<Functor>::R>>(package);
				}
				//�첽ִ��
				template<class Functor, class TT, class...Args>
				static UPointer::uptr<Task<typename Template::GetFunctionInfo<Functor>::R>> RunWith(Functor Target, TT*Object, Args const&...args) {
					UPointer::uptr<TaskPackage> package = Reference::newptr<TaskPackage>(Functional::Bind(Target, Object), nullptr, Functional::params(args...));
					GC::newgc<Threading::Thread>(package)->Go();
					return Reference::newptr<Task<typename Template::GetFunctionInfo<Functor>::R>>(package);
				}
			};
		}
	}
}
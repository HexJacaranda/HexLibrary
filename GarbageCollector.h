#pragma once
#include <process.h>
namespace HL
{
	//内部保留使用
	namespace GC
	{
		namespace Internal
		{
			class GCObjectBase
			{
			public:
				size_t reachable = false;
				virtual ~GCObjectBase(){}
			};
			template<class T>
			class GCObject :public GCObjectBase 
			{
				T*object_ptr;
			public:
				GCObject(T*ptr):object_ptr(ptr){}
				inline bool IsNull()const {
					return object_ptr == nullptr;
				}
				virtual ~GCObject() {
					if (object_ptr)
					{
						delete object_ptr;
						object_ptr = nullptr;
					}
				}
			};

			class Tracker
			{
			public:
				GCObjectBase* reference_target;
				bool is_on_reference;
				inline static Tracker* NewInstance() {
					return (Tracker*)System::Memory::Allocator::Alloc(sizeof(Internal::Tracker));
				}
			};
		}

		//垃圾回收器(内部保留使用)
		class GarbageCollector
		{
			void* scan_thread = nullptr;
			System::Threading::Volatile lock;
			size_t state = 0x01;
			size_t duration = 10;
			size_t scanindex = 0;
			System::Threading::Mutex object_enter;
			System::Generic::Array<Internal::GCObjectBase*>object_table;
			System::Threading::Mutex handle_enter;
			System::Generic::Array<Internal::Tracker*>handle_table;

			volatile bool is_changed = false;
			size_t last_time_count = 0;

			void ScanThread(bool once) 
			{
				while (state != 0x00)
				{
					Start:
					if (!once)
					{
						if (scanindex++ < duration) 
						{ 
							Sleep(1000);
							continue; 
						}
					}
					if (is_changed == false)//GC是否有更改
					{
						if (once)
						{
							return;
						}
						scanindex = 0;
						goto Start;
					}
					else//有更改的话
					{
						state = 0x02;
						scanindex = 0;
						this->lock.TryLock();//检查是否有操作在进行
						//上锁
						if (handle_table.Count() - last_time_count > handle_table.Count() / 3)//满足一定条件才回收
						{
							System::Generic::Array<Internal::GCObjectBase*> sobjs(object_table.Count());
							System::Generic::Array<Internal::Tracker*> shandles(handle_table.Count());

							for (size_t i = 0; i < handle_table.Count(); ++i)
							{
								if (handle_table[i]->is_on_reference)
								{
									handle_table[i]->reference_target->reachable = 0x01;
									shandles.Add(handle_table[i]);
								}
								else
									System::Memory::Allocator::Free(handle_table[i]);
							}
							handle_table = static_cast<System::Generic::Array<Internal::Tracker*>&&>(shandles);
							if (handle_table.Capacity() - handle_table.Count() > handle_table.Count() / 4)//满足条件
								handle_table.ShrinkToFit();//收缩空间
							for (size_t i = 0; i < object_table.Count(); ++i)
							{
								if (object_table[i]->reachable == 0x01)
								{
									object_table[i]->reachable = 0x00;
									sobjs.Add(object_table[i]);
								}
								else
									delete object_table[i];
							}
							object_table = static_cast<System::Generic::Array<Internal::GCObjectBase*>&&>(sobjs);
							if (object_table.Capacity() - object_table.Count() > object_table.Count() / 4)//满足条件
								object_table.ShrinkToFit();//收缩空间

							is_changed = false;
							this->lock.UnLock();//解锁
							last_time_count = handle_table.Count();//更新count
							state = 0x01;//更新状态
							if (once)//如果为一次性扫描
								return;//退出
						}				
					}	
				}
			}
		private:
			static void ScanProxy(void*arg) {
				GarbageCollector*ptr = (GarbageCollector*)arg;
				ptr->ScanThread(false);
			}
		public:
			GarbageCollector() {
				scan_thread=(void*)_beginthread(ScanProxy, 0, this);
			}
			void AddObject(Internal::GCObjectBase*ptr) {
				if (ptr)
				{
					lock.OperationEnter();
					object_enter.Lock();
					object_table.Add(ptr);
					if (!is_changed)
						is_changed = true;
					object_enter.UnLock();
					lock.OperationExit();
				}
			}
			void AddHandle(Internal::Tracker*ptr) {
				if (ptr)
				{
					lock.OperationEnter();
					handle_enter.Lock();
					handle_table.Add(ptr);
					handle_enter.UnLock();
					lock.OperationExit();
				}
			}
			//立即发起回收动作
			void CollectNow() {
				scanindex = 0;
				ScanThread(true);
			}
			~GarbageCollector() {
				while (state == 0x02)Sleep(0);
				if (state != 0x00)
				{
					state = 0x00;
					this->lock.OperationEnter();
					object_enter.Lock();
					for (size_t i = 0; i < object_table.Count(); ++i)
						delete object_table[i];
					object_enter.UnLock();
					handle_enter.Lock();
					for (size_t i = 0; i < handle_table.Count(); ++i)
						System::Memory::Allocator::Free(handle_table[i]);
					handle_enter.UnLock();
					this->lock.OperationExit();
					CloseHandle(this->scan_thread);
				}
			}
		};

		//GC全局唯一实体(内部保留使用)
		static GarbageCollector& GC() {
			static GarbageCollector gc;
			return gc;
		}

		//uptr对接GC配置
		namespace InternalAdopt
		{
			class managed_resource_base :public System::UPointer::uptr_resource
			{
			public:
				Internal::Tracker* tracker = nullptr;
				virtual void release() final{
					if (tracker)
					{
						tracker->is_on_reference = false;
						tracker = nullptr;
					}
				}
				virtual System::UPointer::uptr_resource* clone()const = 0;
				virtual managed_resource_base* selfclone()const = 0;
			};

			template<class T>
			class managed_resource:public managed_resource_base
			{	
			public:
				virtual System::UPointer::uptr_resource* clone()const final{
					InternalAdopt::managed_resource<T> * ptr = new InternalAdopt::managed_resource<T>;
					T*object_ptr = (T*)this->object;
					T*new_object = Interface::ICloneable<T>::GetClonePtr(*object_ptr);
					Internal::GCObjectBase*base = new Internal::GCObject<T>(new_object);
					ptr->tracker = Internal::Tracker::NewInstance();
					ptr->tracker->reference_target = base;
					ptr->tracker->is_on_reference = true;
					ptr->object = new_object;
					GC().AddHandle(ptr->tracker);
					GC().AddObject(base);
					return ptr;
				}
				virtual managed_resource_base* selfclone()const final {
					return new managed_resource<T>;
				}
			};

			class managed_keeper :public System::UPointer::uptr_resource_keeper
			{
			public:	
				managed_keeper() {
					this->keeper_type_id = System::UPointer::resource_keeper_type_id<managed_keeper>();
				}
				virtual void release_resource(System::UPointer::uptr_resource*ptr) final {
					if (ptr)
					{
						managed_resource_base*objptr = static_cast<managed_resource_base*>(ptr);
						objptr->release();
						objptr->self_release = true;
					}
				}
				virtual System::UPointer::uptr_resource*update(System::UPointer::uptr_resource* ptr, System::Reference::IntPtr) final {
					if (ptr)
					{
						managed_resource_base*objptr = static_cast<managed_resource_base*>(ptr);
						managed_resource_base*lptr = objptr->selfclone();
						lptr->object = objptr->object;
						lptr->tracker = Internal::Tracker::NewInstance();
						lptr->tracker->is_on_reference = true;
						lptr->tracker->reference_target = objptr->tracker->reference_target;
						GC().AddHandle(lptr->tracker);
						return lptr;
					}
					else
						return nullptr;
				}
				virtual bool is_invalid(System::UPointer::uptr_resource*ptr)const final {
					if (ptr)
					{
						managed_resource_base*objptr = static_cast<managed_resource_base*>(ptr);
						return (objptr->tracker == nullptr);
					}
					else
						return true;
					
				}

			};
		}

		template<class U,class...Args>
		HL::System::UPointer::uptr<U> newgc(Args &&...args) {
			InternalAdopt::managed_resource<U> * ptr = new InternalAdopt::managed_resource<U>;
			InternalAdopt::managed_keeper *lptr = System::UPointer::keeper_interface<InternalAdopt::managed_keeper>();
			U* object_ptr = new U(const_cast<Args&>(args)...);
			Internal::GCObjectBase*base = new Internal::GCObject<U>(object_ptr);
			ptr->tracker = Internal::Tracker::NewInstance();
			ptr->tracker->reference_target = base;
			ptr->tracker->is_on_reference = true;
			ptr->object = object_ptr;
			GC().AddHandle(ptr->tracker);
			GC().AddObject(base);
			return HL::System::UPointer::uptr<U>(object_ptr, ptr, lptr);
		}
		//针对gc-uptr的类型
		inline static atomic_type gc_type_id() {
			return System::UPointer::resource_keeper_type_id<InternalAdopt::managed_keeper>();
		}
	}
}
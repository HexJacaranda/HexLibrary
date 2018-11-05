#pragma once
#include <process.h>
#include <stdio.h>
#include <atomic>
namespace HL
{
	//内部保留使用
	namespace GC
	{
		//状态
		enum class ObjectStatus :unsigned
		{
			Reachable = 1,
			UnReachable = 0
		};
		//托管对象基类
		class ManagedObjectBase
		{
		public:
			ObjectStatus Status = ObjectStatus::UnReachable;
			virtual ~ManagedObjectBase() { }
		};
		//托管对象
		template<class UDT>
		class ManagedObject :public ManagedObjectBase
		{
		public:
			UDT*ObjectPtr = nullptr;
			virtual ~ManagedObject() {
				if (ObjectPtr != nullptr)
				{
					delete ObjectPtr;
					ObjectPtr = nullptr;
				}
			}
		};

		enum class HandleType
		{
			OnStack,
			OnHeap
		};
		//句柄
		class ObjectHandle
		{
		public:
			ManagedObjectBase* Target = nullptr;
			std::atomic<bool> OnReference;
			std::atomic<bool> OnScanning;
			HandleType Type = HandleType::OnStack;
		};
		//GC状态
		enum class GCStatus:unsigned
		{
			Idle,
			OnRegularCollecting,
			OnOnceCollection,
			Exiting
		};
		//回收器
		class GarbageCollector
		{
			void*m_scan_thread = nullptr;
			System::Threading::Volatile m_access;
			std::atomic<GCStatus> m_status = GCStatus::Idle;
			size_t m_duration = 10;
			size_t m_last = 0;
			size_t m_duration_index = 0;
			std::atomic<bool> m_change_flag = false;
			System::Threading::SpinLock m_object_lock;
			System::Threading::SpinLock m_handle_lock;
			System::Generic::Array<ManagedObjectBase*> m_object_table;
			System::Generic::Array<ObjectHandle*> m_handle_table;

			template<class UDT>
			inline static UDT* Alloc() {
				return new UDT;
			}
			template<class UDT>
			inline static void Free(UDT*Target){
				delete Target;
			}

			void OnceScan() {
				if (!(m_status.load(std::memory_order_acquire) == GCStatus::Idle))
					return;
				if (!m_change_flag.load(std::memory_order_acquire))
					return;
				InternalCollect(GCStatus::OnOnceCollection);
			}
			void RegularScan() {
				for (;;)
				{
					if (m_duration_index++ < m_duration)
					{
						Sleep(1000);
						continue;
					}
					if (m_change_flag.load(std::memory_order_acquire) == false)
					{
						m_duration_index = 0;
						continue;
					}
					else
					{
						InternalCollect(GCStatus::OnRegularCollecting);
						m_duration_index = 0;
					}
				}
			}
			void InternalCollect(GCStatus Status) {
				m_access.TryLock();
				m_status.store(Status, std::memory_order_release);
				if (!(m_handle_table.Count() - m_last > m_handle_table.Count() / 2))//检查数量
					goto Exit;
				{
					System::Generic::Array<ManagedObjectBase*> survived_object(m_object_table.Count());
					System::Generic::Array<ObjectHandle*> survived_handle(m_handle_table.Count());
					for (index_t i = 0; i < m_handle_table.Count(); ++i) {
						if (m_handle_table[i]->Type == HandleType::OnStack) {
							if (m_handle_table[i]->OnReference.load(std::memory_order_acquire) == true)
							{
								m_handle_table[i]->Target->Status = ObjectStatus::Reachable;
								survived_handle.Add(m_handle_table[i]);
							}
							else
								Free(m_handle_table[i]);
						}
					}
					for (index_t i = 0; i < m_object_table.Count(); ++i) {
						if (m_object_table[i]->Status == ObjectStatus::Reachable) {
							m_object_table[i]->Status = ObjectStatus::UnReachable;
							survived_object.Add(m_object_table[i]);
						}
						else
							Free(m_object_table[i]);
					}
					m_object_table = System::Forward(survived_object);
					m_handle_table = System::Forward(survived_handle);
					if (m_handle_table.Capacity() - m_handle_table.Count() > m_handle_table.Count() / 4)//满足条件
						m_handle_table.ShrinkToFit();//收缩空间
					if (m_object_table.Capacity() - m_object_table.Count() > m_object_table.Count() / 4)//满足条件
						m_object_table.ShrinkToFit();//收缩空间
					m_last = m_handle_table.Count();
				}
			Exit:
				m_status.store(GCStatus::Idle, std::memory_order_release);
				m_change_flag.store(false, std::memory_order_release);
				m_access.UnLock();
			}
			static void ScanProxy(void*args) {
				GarbageCollector*ptr = (GarbageCollector*)args;
				ptr->RegularScan();
			}
		public:
			GarbageCollector() {
				m_scan_thread = (void*)_beginthread(ScanProxy, 0, this);
			}
			//获取Handle
			ObjectHandle* GetObjectHandle(ManagedObjectBase*Target, HandleType Type = HandleType::OnStack) {
				ObjectHandle*ret = Alloc<ObjectHandle>();
				ret->OnReference.store(true, std::memory_order_release);
				ret->Target = Target;
				return ret;
			}
			//添加Handle
			void AddHandle(ObjectHandle*Target) {
				m_access.OperationEnter();
				m_handle_lock.Lock();
				m_change_flag.store(true, std::memory_order_release);
				m_handle_table.Add(Target);
				m_handle_lock.Unlock();
				m_access.OperationExit();
			}
			//获取托管对象
			template<class UDT>
			ManagedObject<UDT>* GetManagedObject(UDT*Target) {
				ManagedObject<UDT>*ret = Alloc<ManagedObject<UDT>>();
				ret->ObjectPtr = Target;
				return ret;
			}
			//添加Object
			void AddObject(ManagedObjectBase*Target) {
				m_access.OperationEnter();
				m_object_lock.Lock();
				m_change_flag.store(true, std::memory_order_release);
				m_object_table.Add(Target);
				m_object_lock.Unlock();
				m_access.OperationExit();
			}
			//从指定句柄添加引用
			ObjectHandle* AddReferenceFrom(ObjectHandle*Target) {
				ObjectHandle*ret = Alloc<ObjectHandle>();
				ret->OnReference.store(true, std::memory_order_release);
				m_access.OperationEnter();
				m_object_lock.Lock();
				m_change_flag.store(true, std::memory_order_release);
				ret->Target = Target->Target;
				m_object_lock.Unlock();
				m_access.OperationExit();
				AddHandle(ret);
				return ret;
			}
			//析构
			~GarbageCollector() {
				while (m_status.load(std::memory_order_acquire) != GCStatus::Idle)Sleep(0);
				this->m_access.OperationEnter();
				m_object_lock.Lock();
				for (size_t i = 0; i < m_object_table.Count(); ++i)
					Free(m_object_table[i]);
				m_object_lock.Unlock();
				m_handle_lock.Lock();
				for (size_t i = 0; i < m_handle_table.Count(); ++i)
					Free(m_handle_table[i]);
				m_handle_lock.Unlock();
				this->m_access.OperationExit();
				CloseHandle(m_scan_thread);
			}
		};

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
				ObjectHandle* tracker = nullptr;
				virtual void release() final{
					if (tracker)
					{
						tracker->OnReference.store(false, std::memory_order_release);
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
					T*new_object = System::Interface::ICloneable::GetClonePtr(*object_ptr);
					ManagedObjectBase*base = GC().GetManagedObject<T>(new_object);
					ptr->tracker = GC().GetObjectHandle(base);
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
						lptr->tracker = GC().AddReferenceFrom(objptr->tracker);
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
			ptr->object = object_ptr;
			ManagedObjectBase* base = GC().GetManagedObject<U>(object_ptr);
			ptr->tracker = GC().GetObjectHandle(base);
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
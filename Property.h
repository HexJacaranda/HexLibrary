#pragma once
namespace HL
{
	namespace System
	{
		namespace Property
		{
			template<class T>
			class interface_property
			{
			public:
				virtual operator T const&()const = 0;
				virtual void operator=(T const&)const = 0;
			};

			template<class T>
			class property_ptr
			{
				System::UPointer::uptr<interface_property<T>> interface_ptr;
			};

			template<class T>
			class property
			{
				System::UPointer::uptr<interface_property<T>> interface_ptr;
			public:
				property(System::UPointer::uptr<interface_property<T>> const&ptr) : interface_ptr(ptr) {}
				property(System::UPointer::uptr<interface_property<T>> &&ptr) : interface_ptr(ptr) {}
				property(property const&) = delete;
				property(property &&lhs) {
					this->interface_ptr = lhs.interface_ptr;
					lhs.interface_ptr = nullptr;
				}
				property&operator=(property const&) = delete;
				property&operator=(property&&lhs) {
					this->~property();
					this->interface_ptr = lhs.interface_ptr;
					lhs.interface_ptr = nullptr;
					return *this;
				}
				inline void operator=(T const&rhs) {
					interface_ptr->operator=(rhs);
				}
				inline operator T const&()const {
					return interface_ptr->operator const T &();
				}
				inline operator T&() {
					return const_cast<T&>(interface_ptr->operator const T &());
				}
				~property() {
				}
			};

			template<class PropertyType, class TargetType, PropertyType const&(TargetType::*Getter)()const, void(TargetType::*Setter)(PropertyType const&)>
			class property_getset :public interface_property<PropertyType>
			{
			public:
				virtual operator PropertyType const&()const {
					return (ObjPtr->*Getter)();
				}
				virtual void operator=(PropertyType const& Obj)const {
					(ObjPtr->*Setter)(Obj);
				}
				property_getset(TargetType const*Obj) {
					ObjPtr = const_cast<TargetType*>(Obj);
				}
			private:
				TargetType*ObjPtr;
			};

			template<class PropertyType, class TargetType, PropertyType const&(TargetType::*Getter)()const>
			class property_get :public interface_property<PropertyType>
			{
			public:
				virtual void operator=(PropertyType const&Obj)const {
					throw Exception::GetOnlyException();
				}
				virtual operator PropertyType const&()const {
					return (ObjPtr->*Getter)();
				}
				property_get(TargetType const*Obj) {
					ObjPtr = const_cast<TargetType*>(Obj);
				}
			private:
				TargetType*ObjPtr;
			};

			template<class PropertyType, class TargetType, void(TargetType::*Setter)(PropertyType const&)>
			class property_set :public interface_property<PropertyType>
			{
			public:
				virtual operator PropertyType const&()const {
					throw Exception::SetOnlyException();
				}
				virtual void operator=(PropertyType const& Obj)const {
					(ObjPtr->*Setter)(Obj);
					return *this;
				}
				property_set(TargetType const*Obj) {
					ObjPtr = const_cast<TargetType*>(Obj);
				}
			private:
				TargetType*ObjPtr;
			};

			template<class PropertyType, PropertyType const&(*Getter)(), void(*Setter)(PropertyType const&)>
			class staticproperty_getset :public interface_property<PropertyType>
			{
			public:
				virtual operator PropertyType const&()const {
					return (Getter)();
				}
				virtual void operator=(PropertyType const& Obj)const {
					(Setter)(Obj);
				}
			};

			template<class PropertyType, PropertyType const&(*Getter)()>
			class staticproperty_get :public interface_property<PropertyType>
			{
			public:
				virtual void operator=(PropertyType const&Obj)const {
					throw Exception::GetOnlyException();
				}
				virtual operator PropertyType const&()const {
					return (Getter)();
				}
			};

			template<class PropertyType, void(*Setter)(PropertyType const&)>
			class staticproperty_set :public interface_property<PropertyType>
			{
			public:
				virtual operator PropertyType const&()const {
					throw Exception::SetOnlyException();
				}
				virtual void operator=(PropertyType const& Obj)const {
					(Setter)(Obj);
				}
			};

#define GET
#define SET
#define PROPERTY_GETTER_SETTER(Type,Name,GetCode,SetCode)\
			private:\
			Type const& internal_getter_##Name()const{##GetCode}\
            void internal_setter_##Name(Type const&value){##SetCode}

#define PROPERTY_DECLARE(Type,ClassType,Name)\
            HL::System::Property::property<Type> Name = HL::System::Reference::newptr<HL::System::Property::property_getset<Type,ClassType,&ClassType::internal_getter_##Name,&ClassType::internal_setter_##Name>>(this)
		}
	}
}
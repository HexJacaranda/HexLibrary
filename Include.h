#pragma once
#pragma warning(disable:4067) 
#pragma warning(disable:4018)
#pragma warning(disable:4305)
#pragma warning(disable:4552)
#include <initializer_list>
#define WIN32_LEAN_AND_MEAN  //排除极少用的东西
#include <windows.h>

#include "Define.h"
#include "TemplateTraits.h"
#include "Classes.h"
#include "Exception.h"
#include "algorithm.h"
#include "Covariance.h"
#include "Interface.h"
#include "IntPtr.h"
#include "Cast.h"
#include "Value.h"
#include "Allocator.h"
#include "Hash.h"
#include "Tuple.h"
#include "Memory.h"
#include "Lock.h"
#include "UnifiedPtr.h"
#include "Ptr.h"
#include "Iterator.h"
#include "Linq.h"
#include "Array.h"
#include "Queue.h"
#include "Dictionary.h"
#include "GarbageCollector.h"
#include "Delegate.h"
#include "String.h"
#include "Thread.h"
#include "Console.h"
#include "Property.h"
#include "JSON.h"
#include "Dynamic.h"
#pragma once
namespace HL
{
#if defined(_MSC_VER)//�Ƿ�ΪVC������
#define VC 1
#endif

#if (_MANAGED == 1) || (_M_CEE == 1) //�й�֧��
	//��ʾ�й�����������ʱ֧��
#define _CLR 1
#endif

#if _MSC_VER>=1600
#define _USE_RIGHT_VALUE_REFERENCE 1//��ֵ����
#endif
#if _MSC_VER>=1800
#define _FOR_LOOP_SYNTACTIC_SUGAR 1//for�﷨�ǣ�C++11����
#endif

#ifdef _WIN64
	typedef long long atomic_type;
#else
	typedef long atomic_type;
#endif // _WIN64
	typedef atomic_type index_t;
	class Void;
}
#pragma once
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

// �w���p�֐�
// �����o�֐��|�C���^��void*�ɕϊ�����
// reinterpret_cast�ł��ł��Ȃ��댯�ȃL���X�g
template < class Ty >
void* union_cast(Ty ptr)
{ 
	union
	{
		Ty _mem_func_ptr;
		void* _void_ptr;
	};
	_mem_func_ptr = ptr;
	return _void_ptr;
}

//
// �C�x���g�N���X
template < class >
class Event {};
template < class Ret, class... Args >
class Event < Ret(Args...) >
{
	// _KeyPair�ɒu������
	using _KeyPair = pair < void*, void* >;

	struct _Value
	{
		// std::function �֐��|�C���^�̂悤�Ȃ���
		// Ret�͖߂�l(Args...)�͈���
		function < Ret(Args...) > function;
		_KeyPair keys;
	};

	// �o�^����Ă���C�x���g�̃��X�g
	vector < _Value > value;

	auto find(void* first, void* second = nullptr) noexcept
	{
		_KeyPair arg{ first, second };

		// �����_��
		return find_if(value.begin(), value.end(), [&](_Value& val){return val.keys == arg;});
	}

public:
	///////////////////////////////
	// �C�x���g�ǉ��֐��S
	///////////////////////////////

	// �֐��|�C���^�p
	template < class Ty >
	void Add(Ty* func)
	{ 
		if (find(func) != value.end())
			return;

		// �����_��
		value.push_back({ [=](Args... args) -> Ret{return func(forward < Args >(args)...);}, _KeyPair(func, nullptr) });
	}

	// �֐��I�u�W�F�N�g�p
	template < class Ty >
	void Add(Ty& func) 
	{
		if (find(&func) != value.end())
			return;

		// �����_��
		value.push_back({ [&](Args... args) -> Ret{return func(forward < Args >(args)...);}, _KeyPair(&func, nullptr) });
	}

	// �֐��I�u�W�F�N�g�p
	// �E�Ӓl�Q��
	// �ꎞ�I�u�W�F�N�g�̑���p
	template < class Ty >
	void Add(Ty&& func) 
	{
		// �����_��
		value.push_back({ [func = move(func)](Args... args) mutable->Ret{return func(forward < Args >(args)...);}, _KeyPair(nullptr, nullptr) });
	}

	// �����o�֐��|�C���^�p
	// �����o�ւ̃A�N�Z�X�́A�I�u�W�F�N�g��m��K�v������
	template < class Ty1, class Ty2 >
	void Add(Ty1& obj, Ty2 ptr)
	{
		if (find(&obj, union_cast(ptr)) != value.end())
			return;

		// �����_��
		value.push_back({ [&obj, ptr](Args... args) -> Ret{return (obj.*ptr)(forward < Args >(args)...);}, _KeyPair(&obj, union_cast(ptr)) });
	}

	/////////////////////////////
	// �폜�֐��S
	/////////////////////////////

	// �֐��|�C���^�p
	template < class Ty >
	void Remove(Ty* func)
	{
		auto itr = find(func);
		if (itr != value.end())
			value.erase(itr);
	}

	// �֐��I�u�W�F�N�g�p
	template < class Ty >
	void Remove(Ty& func)
	{
		remove(&func);
	}

	// �����o�֐��|�C���^�p
	template < class Ty1, class Ty2 >
	void Remove(Ty1& obj, Ty2 ptr)
	{
		auto itr = find(&obj, union_cast(ptr));
		if (itr != value.end())
			value.erase(itr);
	}


	// �o�^����Ă���C�x���g�̐�
	int GetEventCount()
	{
		return value.size();
	}


	// �C�x���g�Ăяo��
	void operator()(Args... args) const
	{
		for (auto&& elem : value) { elem.function(forward < Args >(args)...); }
	}
};
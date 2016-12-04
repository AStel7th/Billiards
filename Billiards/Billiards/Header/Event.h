#pragma once
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

// ヘルパ関数
// メンバ関数ポインタをvoid*に変換する
// reinterpret_castでもできない危険なキャスト
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
// イベントクラス
template < class >
class Event {};
template < class Ret, class... Args >
class Event < Ret(Args...) >
{
	// _KeyPairに置き換え
	using _KeyPair = pair < void*, void* >;

	struct _Value
	{
		// std::function 関数ポインタのようなもの
		// Retは戻り値(Args...)は引数
		function < Ret(Args...) > function;
		_KeyPair keys;
	};

	// 登録されているイベントのリスト
	vector < _Value > value;

	auto find(void* first, void* second = nullptr) noexcept
	{
		_KeyPair arg{ first, second };

		// ラムダ式
		return find_if(value.begin(), value.end(), [&](_Value& val){return val.keys == arg;});
	}

public:
	///////////////////////////////
	// イベント追加関数郡
	///////////////////////////////

	// 関数ポインタ用
	template < class Ty >
	void Add(Ty* func)
	{ 
		if (find(func) != value.end())
			return;

		// ラムダ式
		value.push_back({ [=](Args... args) -> Ret{return func(forward < Args >(args)...);}, _KeyPair(func, nullptr) });
	}

	// 関数オブジェクト用
	template < class Ty >
	void Add(Ty& func) 
	{
		if (find(&func) != value.end())
			return;

		// ラムダ式
		value.push_back({ [&](Args... args) -> Ret{return func(forward < Args >(args)...);}, _KeyPair(&func, nullptr) });
	}

	// 関数オブジェクト用
	// 右辺値参照
	// 一時オブジェクトの代入用
	template < class Ty >
	void Add(Ty&& func) 
	{
		// ラムダ式
		value.push_back({ [func = move(func)](Args... args) mutable->Ret{return func(forward < Args >(args)...);}, _KeyPair(nullptr, nullptr) });
	}

	// メンバ関数ポインタ用
	// メンバへのアクセスは、オブジェクトを知る必要がある
	template < class Ty1, class Ty2 >
	void Add(Ty1& obj, Ty2 ptr)
	{
		if (find(&obj, union_cast(ptr)) != value.end())
			return;

		// ラムダ式
		value.push_back({ [&obj, ptr](Args... args) -> Ret{return (obj.*ptr)(forward < Args >(args)...);}, _KeyPair(&obj, union_cast(ptr)) });
	}

	/////////////////////////////
	// 削除関数郡
	/////////////////////////////

	// 関数ポインタ用
	template < class Ty >
	void Remove(Ty* func)
	{
		auto itr = find(func);
		if (itr != value.end())
			value.erase(itr);
	}

	// 関数オブジェクト用
	template < class Ty >
	void Remove(Ty& func)
	{
		remove(&func);
	}

	// メンバ関数ポインタ用
	template < class Ty1, class Ty2 >
	void Remove(Ty1& obj, Ty2 ptr)
	{
		auto itr = find(&obj, union_cast(ptr));
		if (itr != value.end())
			value.erase(itr);
	}


	// 登録されているイベントの数
	int GetEventCount()
	{
		return value.size();
	}


	// イベント呼び出し
	void operator()(Args... args) const
	{
		for (auto&& elem : value) { elem.function(forward < Args >(args)...); }
	}
};
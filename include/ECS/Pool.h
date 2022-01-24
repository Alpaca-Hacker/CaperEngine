#pragma once
#include <vector>
class IPool
{
public:
	virtual ~IPool() {}
};

template <typename T>
class Pool : public IPool
{
public:
	Pool(const int size = 100) { Resize(size); }
	virtual ~Pool() = default;

	[[nodiscard]] bool IsEmpty() const { return data_.empty(); }
	[[nodiscard]] int GetSize() const { return data_.size(); }
	void Resize(int n) { data_.resize(n); }
	void Clear() { data_.clear(); }
	void Add(T object) { data_.push_back(object); }
	void Set(int index, T object) { data_[index] = object; }
	T& Get(int index) { return static_cast<T&>(data_[index]); }
	T& operator [](unsigned int index) { return data_[index]; }

private:
	std::vector<T> data_;
};

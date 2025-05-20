#pragma once

#include <array>
#include <iterator>
#include <vector>

template<typename Iter>
class RangeView
{
public:
    // Constructors
    constexpr RangeView() {

    }
    constexpr RangeView(Iter first, Iter last) :
        first_(first), last_(last) {

    }

    template<typename Container>
    constexpr explicit RangeView(Container& c) :
        RangeView(c.begin(), c.end()) {

    }

    template<typename T, std::size_t N>
    constexpr explicit RangeView(T(&arr)[N]) :
        RangeView(std::begin(arr), std::end(arr)) {

    }

    template<typename T, std::size_t N>
    constexpr explicit RangeView(const std::array<T, N>& arr) :
        RangeView(arr.begin(), arr.end()) {

    }

    template<typename T>
    constexpr explicit RangeView(std::initializer_list<T> ilist) :
        RangeView(ilist.begin(), ilist.end()) {

    }

    // Iterator Accessors
    constexpr Iter begin() const {
        return first_;
    }

    constexpr Iter end() const {
        return last_;
    }

    constexpr std::reverse_iterator<Iter> rbegin() const {
        return std::reverse_iterator<Iter>(last_);
    }

    constexpr std::reverse_iterator<Iter> rend() const {
        return std::reverse_iterator<Iter>(first_);
    }

    // Const Iterator Accessors
    constexpr typename std::add_const<Iter>::type cbegin() const {
        return first_;
    }

    constexpr typename std::add_const<Iter>::type cend() const {
        return last_;
    }

    constexpr std::reverse_iterator<typename std::add_const<Iter>::type> crbegin() const {
        return std::reverse_iterator<typename std::add_const<Iter>::type>(last_);
    }

    constexpr std::reverse_iterator<typename std::add_const<Iter>::type> crend() const {
        return std::reverse_iterator<typename std::add_const<Iter>::type>(first_);
    }

    // Size Accessor
    constexpr std::size_t size() const {
        return std::distance(first_, last_);
    }

    constexpr const typename std::iterator_traits<Iter>::reference operator[](std::size_t idx) const {
        assert(idx < size());
        return *(first_ + idx);
    }

    constexpr typename std::iterator_traits<Iter>::reference operator[](std::size_t idx) {
        assert(idx < size());
        return *(first_ + idx);
    }

private:
    Iter first_ = nullptr;
    Iter last_ = nullptr;
};


template<typename T, typename ConT>
constexpr bool operator==(const RangeView<T>& lhs, const ConT& rhs) {
    auto it_l = lhs.begin();
    auto it_r = rhs.begin();
    auto it_e = rhs.end();

    for (; it_r != it_e; ++it_l, ++it_r) {
        if (*it_r != *it_l) {
            return false;
        }
    }
    return true;
}
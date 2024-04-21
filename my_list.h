#ifndef MY_LIST_H
#define MY_LIST_H

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

struct ListNodeBase {
    ListNodeBase *next = nullptr;
};

template <typename T>
class MyList {
    ListNodeBase dummy;
    
    struct ListNode; // определен вне класса
    
    template<bool IsConst>
    class IteratorTemplate; // определен вне класса
public:
    using value_type = T;
    using Iterator = IteratorTemplate<false>;
    using ConstIterator = IteratorTemplate<true>;
    
    ~MyList() {
        auto ptr = static_cast<ListNode*>(dummy.next);
        while (ptr != nullptr) {
            auto next = static_cast<ListNode*>(ptr->next);
            delete ptr;
            ptr = next;
        }
    }
    
    // before_begin() возвращает итератор, указывающий на несуществующий элемент перед первым элементом
    // разыменование before_begin() приведет к неопределенному поведению 
    Iterator before_begin() { return Iterator{&dummy}; }
    ConstIterator before_begin() const { return cbefore_begin(); }
    ConstIterator cbefore_begin() const { return ConstIterator{&dummy}; }
    
    Iterator begin() { return Iterator{dummy.next}; }
    ConstIterator begin() const { return cbegin(); }
    ConstIterator cbegin() const { return ConstIterator{dummy.next}; }
    
    Iterator end() { return Iterator{nullptr}; }
    ConstIterator end() const { return cend(); }
    ConstIterator cend() const { return ConstIterator{nullptr}; }
    
    Iterator insert_after(ConstIterator pos, T&& value) {
        if (pos == cend()) {
            throw std::logic_error("MyList::insert_after: invalid position");
        }
        auto next = pos.ptr->next;
        pos.ptr->next = new ListNode{std::move(value), next};
        return Iterator{pos.ptr->next};        
    }
    
    Iterator insert_after(ConstIterator pos, const T& value) {
        auto copy = value;
        return insert_after(pos, std::move(copy));
    }
    
    Iterator erase_after(ConstIterator pos) {
        if (pos == cend() || pos.ptr->next == nullptr) {
            throw std::logic_error("MyList::erase_after: invalid position");
        }
        auto next = pos.ptr->next->next;
        delete pos.ptr->next;
        pos.ptr->next = next;
        return Iterator{next};
    }
    
    Iterator push_front(const T& value) { return insert_after(before_begin(), value); }
    Iterator push_front(T&& value) { return insert_after(before_begin(), std::move(value)); }
    
    Iterator pop_front() { return erase_after(before_begin()); }
    
    Iterator find(const T& query) { return std::find(begin(), end(), query); }
    ConstIterator find(const T& query) const { return std::find(cbegin(), cend(), query); }
};


template <typename T>
struct MyList<T>::ListNode : public ListNodeBase {
    T value;
    
    ListNode(const T& val, ListNodeBase* ptr) : ListNodeBase{ptr}, value{val} {}  
    ListNode(T&& val, ListNodeBase* ptr) : ListNodeBase{ptr}, value{std::move(val)} {}  
};

// шаблон для мутабельных и константных итераторов
template <typename T>
template <bool IsConst>
class MyList<T>::IteratorTemplate 
{
    ListNodeBase* ptr;
public:
    friend class IteratorTemplate<true>;
    friend class MyList;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = typename std::conditional<IsConst, const T*, T*>::type;
    using reference         = typename std::conditional<IsConst, const T&, T&>::type;

    explicit IteratorTemplate(ListNodeBase* p) : ptr{p} {}
    
    // имплицитное приведение мутабельного итератора к константному итератору
    IteratorTemplate(const IteratorTemplate<false>& rhs) : ptr{rhs.ptr} {}

    reference operator*() const {
        return static_cast<ListNode*>(ptr)->value;
    }
    pointer operator->() {
        return &(static_cast<ListNode*>(ptr)->value);
    }
    IteratorTemplate& operator++() {  // префиксный инкремент
        ptr = ptr->next;
        return *this;
    }
    IteratorTemplate operator++(int) { // постфиксный инкремент
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator==(const IteratorTemplate& a, const IteratorTemplate& b) {
        return a.ptr == b.ptr;
    };
    friend bool operator!=(const IteratorTemplate& a, const IteratorTemplate& b) {
        return !(a == b);
    };
};

#endif//MY_LIST_H
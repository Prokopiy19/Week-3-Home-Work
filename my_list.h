#ifndef MY_LIST_H
#define MY_LIST_H

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

struct ListNodeBase {
    ListNodeBase *next;
    ListNodeBase(ListNodeBase* p = nullptr) : next{p} {}
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
    
    MyList() = default;
    
    MyList(const MyList& rhs) {
        auto tail = before_begin();
        for (const auto& element : rhs) {
            tail = insert_after(tail, element);
        } 
    }
    
    void swap(MyList& with) {
        std::swap(dummy, with.dummy);
    }
    
    MyList& operator=(const MyList& rhs) {
        if (this != &rhs) { // избегаем присваивание себе
            // чтобы минимизировать количество аллокаций динамической памяти,
            // мы переиспользуем уже существующие узлы
            auto lit = before_begin();
            auto rit = rhs.cbefore_begin();
            while (std::next(lit) != end() && std::next(rit) != rhs.cend()) {
                ++lit;
                ++rit;
                *lit = *rit;
            }
            ++rit;
            if (rit != rhs.cend()) {
                // на правом списке больше элементов, чем в нашей, поэтому нам нужно создать новые узлы
                while (rit != rhs.cend()) {
                    lit = insert_after(lit, *rit);
                    ++rit;
                }
            }
            else {
                // размер нашего списка был больше правого, поэтому удаляем лишние узлы
                while (std::next(lit) != end()) {
                    erase_after(lit);
                }
            }
        }
        return *this;
    }
    
    MyList(MyList&& rhs) {
        dummy = rhs.dummy;
        rhs.dummy.next = nullptr;
    }
    
    MyList& operator=(MyList&& rhs) {
        if (this != &rhs) { // избегаем присваивание себе
            clear();
            dummy = rhs.dummy;
            rhs.dummy.next = nullptr;
        }
        return *this;
    }
    
    ~MyList() {
        clear();
    }
    
    bool empty() const { return begin() == end(); }
    
    void clear() {
        while (!empty()) {
            pop_front();
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
        auto tmp = pos.ptr->next;
        pos.ptr->next = new ListNode{std::move(value), tmp};
        return Iterator{pos.ptr->next};        
    }
    
    Iterator insert_after(ConstIterator pos, const T& value) {
        auto copy = value;
        return insert_after(pos, std::move(copy));
    }
    
    Iterator erase_after(ConstIterator pos) {
        if (pos == cend() || std::next(pos) == cend()) {
            throw std::logic_error("MyList::erase_after: invalid position");
        }
        auto tmp = pos.ptr->next->next;
        delete static_cast<ListNode*>(pos.ptr->next);
        pos.ptr->next = tmp;
        return Iterator{tmp};
    }
    
    // удаление всех узлов со значением value
    int erase_by_value(const T& value) {
        int erased_count = 0;
        auto it = before_begin();
        auto next = std::next(it);
        while (next != end()) {
            if (*next == value) {
                ++erased_count;
                next = erase_after(it);
            }
            else {
                ++it;
                ++next;
            }
        }
        return erased_count;
    }
    
    Iterator push_front(const T& value) { return insert_after(before_begin(), value); }
    Iterator push_front(T&& value) { return insert_after(before_begin(), std::move(value)); }
    
    Iterator pop_front() { return erase_after(before_begin()); }
    
    Iterator find(const T& query, ConstIterator pos = ConstIterator{nullptr}) { return std::find(begin(), end(), query); }
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

    explicit IteratorTemplate(const ListNodeBase* p) : ptr{const_cast<ListNodeBase*>(p)} {}
    
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
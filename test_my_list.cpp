#include <algorithm>
#include <iostream>
#include <string>

#include "my_list.h"

// я тут тестирую класс MyList
// не предназначено для чтения

struct A {
    int x;
    A() = delete;
    A(int val) : x{val} {}
};

template<typename T>
void print(const MyList<T>& list)
{
    std::cout << "list content: ";
    for (const auto& element : list) {
        std::cout << element << ' ';
    }
    std::cout << '\n';
}

int main()
{
    MyList<std::string> list;
    list.push_front("hello");
    list.push_front("world");
    list.push_front("test");
    list.push_front("bye");
    *list.begin() = "welcome";
    print(list);
    
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';
    
    std::string query = "drum";
    auto it = list.find(query);
    if (it == list.cend()) {
        std::cout << query << " not found\n";
    }
    else {
        std::cout << *it << " is found\n";
    }
    const auto& clist = list;
    
    auto cit = list.cbegin();
    ++cit;
    std::cout << "erasing after " << *cit << '\n';
    list.erase_after(cit);
    list.insert_after(list.begin(), "insert");
    
    print(clist);
        
    MyList<std::string> list2;
    list2 = list;
    list = list2;
    list = list;
    
    std::cout << "we copied a bunch\n";
    print(list2);
    
    auto new_list = std::move(list);
    if (list.empty()) {
        std::cout << "list became empty after move\n";
    }
    
    auto insert_it = new_list.begin();
    insert_it = new_list.insert_after(insert_it, "test");
    insert_it = new_list.insert_after(insert_it, "lol");
    insert_it = new_list.insert_after(insert_it, "lol");
    insert_it = new_list.insert_after(insert_it, "test");
    insert_it = new_list.insert_after(insert_it, "kek");
    insert_it = new_list.insert_after(insert_it, "time");
    
    print(new_list);
    
    new_list.erase_by_value("test");
    
    print(new_list);
    
    new_list.clear();
    
    for (int i = 0; i < 5; ++i) {
        new_list.push_front("test");
    }
    
    print(new_list);
    
    new_list.erase_by_value("test");
   
    print(new_list);
    
    
    {
        std::cout << "\nTest of copy assignment\n";
        MyList<int> l1;
        for (int i = 0; i < 3; ++i) {
            l1.push_front(i);
        }
        MyList<int> l2;
        for (int i = 0; i < 5; ++i) {
            l2.push_front(i);
        }
        print(l1);
        print(l2);
        std::cout << "l1 = l2\n";
        l1 = l2;
        print(l1);
        print(l2);
    }
    
    {
        std::cout << "\nTest of copy assignment\n";
        MyList<int> l1;
        for (int i = 0; i < 15; ++i) {
            l1.push_front(i);
        }
        MyList<int> l2;
        for (int i = 0; i < 6; ++i) {
            l2.push_front(i);
        }
        print(l1);
        print(l2);
        std::cout << "l1 = l2\n";
        l1 = l2;
        print(l1);
        print(l2);
    }
    
    return 0;
}
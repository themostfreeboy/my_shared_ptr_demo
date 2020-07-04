#ifndef __MY_SHARED_PTR_H__
#define __MY_SHARED_PTR_H__

#include <iostream>
#include <mutex>
#include <string>

template<typename T>
class MyShared_ptr {
public:
    class Meta {
    public:
        // 构造函数
        Meta(T* data_pointer = nullptr,
                int count = 1,
                bool has_release = false) :
                _data_pointer(data_pointer),
                _count(count),
                _has_release(has_release) {};

        // 拷贝构造函数
        Meta(const Meta& meta) {
            std::lock_guard<std::mutex> lock(_mutex);
            _data_pointer = meta._data_pointer;
            _count = meta._count;
            _has_release = meta._has_release;
        }

        // 赋值函数
        Meta& operator=(const Meta& meta) = delete;

         // 移动构造函数
        Meta(Meta&& meta) = delete;

        // 移动赋值函数
        Meta& operator=(Meta&& meta) = delete;

        // 析构函数
        ~Meta() = default;

        void set_pointer_count_release(T* data_pointer,
                int count,
                bool has_release) {
            std::lock_guard<std::mutex> lock(_mutex);
            _data_pointer = data_pointer;
            _count = count;
            _has_release = has_release;
        }

        void set_pointer(T* data_pointer) {
            std::lock_guard<std::mutex> lock(_mutex);
            _data_pointer = data_pointer;
        }

        void set_count(int count) {
            std::lock_guard<std::mutex> lock(_mutex);
            _count = count;
        }

        void set_release(bool has_release) {
            std::lock_guard<std::mutex> lock(_mutex);
            _has_release = has_release;
        }

        T* get_pointer() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _data_pointer;
        }

        int get_count() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count;
        }

        bool get_release() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _has_release;
        }

        void set_count_add_one() {
            std::lock_guard<std::mutex> lock(_mutex);
            ++_count;
        }

        void set_count_minus_one() {
            std::lock_guard<std::mutex> lock(_mutex);
            --_count;
        }

        bool check_and_delete_pointer() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_has_release && _count == 0 && _data_pointer != nullptr) {
                delete _data_pointer;
                _data_pointer = nullptr;
                return true;
            }
            return false;
        }

    private:
        std::mutex _mutex;
        T* _data_pointer;
        int _count;
        bool _has_release;
    };

    // 构造函数
    MyShared_ptr(T* data_pointer = nullptr) : MyShared_ptr(new(std::nothrow) Meta(data_pointer, 1, false)) {};

    // 构造函数
    explicit MyShared_ptr(std::nullptr_t pointer) : MyShared_ptr(new(std::nothrow) Meta(nullptr, 1, false)) {};

    // 拷贝构造函数
    MyShared_ptr(const MyShared_ptr& my_shared_ptr) {
        std::lock_guard<std::mutex> lock(_mutex);
        Meta* meta = my_shared_ptr->get_meta();
        meta->set_count_add_one();
        this->MyShared_ptr(meta);
    }

    // 赋值函数
    MyShared_ptr& operator=(const MyShared_ptr& my_shared_ptr) {
        return MyShared_ptr(my_shared_ptr->get_meta());
    }

    // 赋值函数
    MyShared_ptr& operator=(std::nullptr_t pointer) {
        return MyShared_ptr(nullptr);
    }

    // 移动构造函数
    MyShared_ptr(MyShared_ptr&& my_shared_ptr) = delete;

    // 移动赋值函数
    MyShared_ptr& operator=(MyShared_ptr&& my_shared_ptr) = delete;

    // 析构函数
    ~MyShared_ptr() {
        std::string notice_count = "run destructor, count=" + std::to_string(_meta->get_count()) + "\n";
        std::cout << notice_count;
        _meta->set_count_minus_one();
        if(_meta->check_and_delete_pointer()) {
            delete _meta;
            _meta = nullptr;
            std::string notice_destructor = "run destructor, count==0, going to delete\n";
            std::cout << notice_destructor;
        }
    };

    // 此接口不应该暴露出去，但是如果不暴露，拷贝构造函数中也无法获得，目前没想到什么更好的方法
    Meta* get_meta() {
        return _meta;
    }

    // 判断_data_pointer是否为nullptr
    operator bool() {
        return _meta->get_pointer() != nullptr;
    }

    T& operator*() {
        return *(_meta->get_pointer());
    }

    T* operator->() {
        return _meta->get_pointer();
    }

    // 返回实际的指针
    T* get() {
        return _meta->get_pointer();
    }

    // 返回当前引用计数
    int use_count() {
        return _meta->get_count();
    }

    // 释放对应的指针计数，不再进行自动析构
    void release() {
        _meta->set_release(true);
    }

private:
    std::mutex _mutex;
    Meta* _meta;

    // 构造函数
    MyShared_ptr(const Meta* meta) {
        std::lock_guard<std::mutex> lock(_mutex);
        _meta = const_cast<Meta*>(meta);
        std::string notice_constructor = "run constructor, count=" + std::to_string(_meta->get_count()) + "\n";
        std::cout << notice_constructor;
    };
};

#endif

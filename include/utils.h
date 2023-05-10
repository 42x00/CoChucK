//
// Created by Yikai Li on 5/8/23.
//

#ifndef COCHUCK_UTILS_H
#define COCHUCK_UTILS_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <experimental/coroutine>
#include <cstdint>
#include <exception>


// Thread-safe queue
template<typename T>
class TSQueue {
private:
    // Underlying queue
    std::queue<T> m_queue;

    // mutex for thread synchronization
    std::mutex m_mutex;

    // Condition variable for signaling
    std::condition_variable m_cond;

public:
    // Pushes an element to the queue
    void push(T item) {

        // Acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Add item
        m_queue.push(item);

        // Notify one thread that
        // is waiting
        m_cond.notify_one();
    }

    // Pops an element off the queue
    T pop() {

        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue is not empty
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });

        // retrieve item
        T item = m_queue.front();
        m_queue.pop();

        // return item
        return item;
    }

    // Empty
    bool empty() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
};


template<typename T>
struct Generator {
    // The class name 'Generator' is our choice and it is not required for coroutine
    // magic. Compiler recognizes coroutine by the presence of 'co_yield' keyword.
    // You can use name 'MyGenerator' (or any other name) instead as long as you include
    // nested struct promise_type with 'MyGenerator get_return_object()' method.

    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;

    struct promise_type // required
    {
        T value_;
        std::exception_ptr exception_;

        Generator get_return_object() {
            return Generator(handle_type::from_promise(*this));
        }

        std::experimental::suspend_always initial_suspend() { return {}; }

        std::experimental::suspend_always final_suspend() noexcept { return {}; }

        void unhandled_exception() { exception_ = std::current_exception(); } // saving
        // exception

        template<std::convertible_to<T> From>
        // C++20 concept
        std::experimental::suspend_always yield_value(From &&from) {
            value_ = std::forward<From>(from); // caching the result in promise
            return {};
        }

        void return_void() {}
    };

    handle_type h_;

    Generator(handle_type h)
            : h_(h) {
    }

    ~Generator() { h_.destroy(); }

    explicit operator bool() {
        fill(); // The only way to reliably find out whether or not we finished coroutine,
        // whether or not there is going to be a next value generated (co_yield)
        // in coroutine via C++ getter (operator () below) is to execute/resume
        // coroutine until the next co_yield point (or let it fall off end).
        // Then we store/cache result in promise to allow getter (operator() below
        // to grab it without executing coroutine).
        return !h_.done();
    }

    T operator()() {
        fill();
        full_ = false; // we are going to move out previously cached
        // result to make promise empty again
        return std::move(h_.promise().value_);
    }

private:
    bool full_ = false;

    void fill() {
        if (!full_) {
            h_();
            if (h_.promise().exception_)
                std::rethrow_exception(h_.promise().exception_);
            // propagate coroutine exception in called context

            full_ = true;
        }
    }
};

class ChuckDur {
public:
    ChuckDur() : samples(0) {}

    ChuckDur(double samples) : samples(samples) {}

    ChuckDur(const ChuckDur &other) : samples(other.samples) {}

    ChuckDur operator/(double rhs) const {
        return {samples / rhs};
    }

    ChuckDur operator*(double rhs) const {
        return {samples * rhs};
    }

    friend ChuckDur operator*(double lhs, const ChuckDur &rhs) {
        return {lhs * rhs.samples};
    }

    double samples;
};

class ChuckTime {
public:
    ChuckTime(double sample) : sample(sample) {}

    double sample;
};

#endif //COCHUCK_UTILS_H

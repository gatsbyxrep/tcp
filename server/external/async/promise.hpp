#ifndef PROMISE_HPP
#define PROMISE_HPP

#include <coroutine>
#include <functional>

#include "common.hpp"
#include "future.hpp"
#include "task.hpp"

namespace Async {

    // https://lewissbaker.github.io/2018/09/05/understanding-the-promise-type

    template<typename T>
    class ContinuationAwaiter {
    public:
        ContinuationAwaiter() noexcept {}

        bool await_ready() const noexcept {
            return false;
        }

        template<typename PromiseType,
                 typename = typename std::enable_if_t<!std::is_same<PromiseType, FinalTaskPromise>::value, PromiseType>>
        std::coroutine_handle<> await_suspend(std::coroutine_handle<PromiseType> handle) const noexcept {
            try {
                handle.promise().on_finish();
            } catch (const std::exception& e) {

            }

            return handle.promise().get_continuation();
        }

        template<typename PromiseType,
                 typename = typename std::enable_if_t<std::is_same<PromiseType, FinalTaskPromise>::value, PromiseType>>
        bool await_suspend(std::coroutine_handle<PromiseType> handle) const noexcept {
            try {
                handle.promise().on_finish();
            } catch (const std::exception& e) {

            }

            return false;
        }

        void await_resume() const noexcept {}
    };

    template<typename T>
    class PromiseBase {
    public:

        PromiseBase() = default;

        ContinuationAwaiter<T> final_suspend() const noexcept {
            return ContinuationAwaiter<T>();
        }

        void on_finish_callback(std::function<void(std::exception_ptr e)>&& onFinish) noexcept {
            onFinishFn = std::move(onFinish);
        }

        void on_finish()  {
            if (onFinishFn) {
                onFinishFn(e);
            }
        }

        void unhandled_exception() noexcept {
            e = std::current_exception();

            try {
                std::rethrow_exception(e);
            } catch (const CancelledError& e) {

            } catch (const std::exception& e) {

            }
        }

        std::exception_ptr get_exception() const noexcept {
            return e;
        }

        void set_continuation(std::coroutine_handle<> continuation) noexcept {
            this->continuation = continuation;
        }

        std::coroutine_handle<> get_continuation() const noexcept {
            return continuation;
        }

        template <typename FutureType>
        typename std::enable_if<std::is_base_of<Schedulable, FutureType>::value, FutureAwaiter<FutureType>>::type
        await_transform(FutureType&& future) {
            return FutureAwaiter<FutureType>(std::forward<FutureType>(future));
        }

        template <typename Awaitable>
        typename std::enable_if<!std::is_base_of<Schedulable, Awaitable>::value, Awaitable>::type
        await_transform(Awaitable&& awaitable) {
            return std::forward<Awaitable>(awaitable);
        }

        virtual ~PromiseBase() = default;

    private:
        std::coroutine_handle<> continuation;
        std::function<void(std::exception_ptr)> onFinishFn;
        std::exception_ptr e;

    };

    class FinalTaskPromise : public PromiseBase<void> {
    public:
        FinalTaskPromise() = default;

        std::suspend_never initial_suspend() const noexcept {
            return {};
        }

        FinalTask get_return_object() {
            return FinalTask(std::coroutine_handle<FinalTaskPromise>::from_promise(*this));
        }

        void return_void() const noexcept {}
    };

    template<typename T>
    class Promise : public PromiseBase<T> {
    public:
        Promise() = default;

        std::suspend_always initial_suspend() const noexcept {
            return {};
        }

        Task<T> get_return_object() {
            return Task<T>(std::coroutine_handle<Promise<T>>::from_promise(*this));
        }

        void return_value(T&& value) noexcept {
            this->value = std::forward<T>(value);
        }

        T get_return_value() const {
            auto e = PromiseBase<T>::get_exception();
            if (e) {
                std::rethrow_exception(e);
            }
            return value;
        }

    private:
        T value;
    };

    template<>
    class Promise<void> : public PromiseBase<void> {
    public:
        Promise() = default;

        std::suspend_always initial_suspend() const noexcept {
            return {};
        }

        Task<void> get_return_object() {
            return Task<void>(std::coroutine_handle<Promise<void>>::from_promise(*this));
        }

        void return_void() const noexcept {}

        void get_return_value() const {
            auto e = PromiseBase<void>::get_exception();
            if (e) {
                std::rethrow_exception(e);
            }
        }
    };
}

#endif // PROMISE_HPP

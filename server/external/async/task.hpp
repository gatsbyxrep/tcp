#ifndef TASK_HPP
#define TASK_HPP

#include <coroutine>
#include <memory>
#include <atomic>
#include <functional>

namespace Async {
    template<typename T>
    class Promise;
    class FinalTaskPromise;

    template<typename T>
    class TaskAwaiter {
        public:
            TaskAwaiter(std::coroutine_handle<Promise<T>> handle)
            :   handle(handle) {}

            bool await_ready() const noexcept {
                return handle.done();
            }

            template<typename ContinuationPromiseType>
            std::coroutine_handle<> await_suspend(std::coroutine_handle<ContinuationPromiseType> handle) const noexcept {
                this->handle.promise().set_continuation(handle);
                return this->handle;
            }

            T await_resume() const {
                return handle.promise().get_return_value();
            }

        private:
            std::coroutine_handle<Promise<T>> handle;
    };

    template<typename T>
    class Task {
        public:
            using promise_type = Promise<T>;
            Task(std::coroutine_handle<promise_type> handle)
            :   handle(handle) {
            }

            Task(Task&) = delete;

            Task(Task&& other) noexcept
            : handle{other.handle} {
                other.handle = {};
            }

            TaskAwaiter<T> operator co_await() noexcept {
                return TaskAwaiter<T> { handle };
            }


            bool done() const noexcept {
                return handle.done();
            }

            ~Task() {
                if (handle) {
                    handle.destroy();
                }
            }

        private:
            std::coroutine_handle<promise_type> handle;
    };

    struct CoState {
        std::exception_ptr e;
        std::atomic_bool done;
    };

    class FinalTask {
        public:
            using promise_type = FinalTaskPromise;

            FinalTask(std::coroutine_handle<promise_type> handle);

            FinalTask(FinalTask&) = delete;

            FinalTask(FinalTask&& other) noexcept
            :   handle{other.handle}  {
                other.handle = {};
            }

            std::shared_ptr<CoState> get_coroutine_state() const noexcept {
                return state;
            }

            bool done() const noexcept {
                return handle.done();
            }

            void destroy() {
                if (handle) {
                    handle.destroy();
                }
            }

            ~FinalTask() {}

        private:
            std::shared_ptr<CoState> state;
            std::coroutine_handle<promise_type> handle;
    };

    FinalTask spawnFinal(std::function<Task<void>()>&& awaitable);
}

#endif // TASK_HPP

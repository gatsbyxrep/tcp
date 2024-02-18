#ifndef FUTURE_HPP
#define FUTURE_HPP

#include <memory>
#include <coroutine>


#include "executor.hpp"
#include "promise.hpp"

namespace Async {
    class Schedulable {
    public:
        virtual ~Schedulable() = default;
        virtual void schedule(std::coroutine_handle<> handle) const = 0;
    };

    class Future : public Schedulable {
    public:
        Future(std::shared_ptr<Executor> executor)
        :   executor(executor) {}

        virtual void schedule(std::coroutine_handle<> handle) const {
            auto conHandle = [handle]() mutable {
                handle.resume();

            };
            executor->scheduleSuspended(conHandle);
        }

        virtual ~Future() = default;
    protected:
        const std::shared_ptr<Executor> executor;

    };

    template<typename FutureType,
             typename = typename std::enable_if_t<std::is_base_of<Schedulable, FutureType>::value, FutureType>>
    class FutureAwaiter {
    public:
        FutureAwaiter(FutureType&& futureAwaitable)
        :   futureAwaitable(std::forward<FutureType>(futureAwaitable)) {}

        bool await_ready() const noexcept {
            return false;
        }

        void await_suspend([[maybe_unused]] std::coroutine_handle<> handle) const noexcept {
            //futureAwaitable.schedule(handle);
        }

        void await_resume() const {

        }

    private:
        FutureType futureAwaitable;
    };
}

#endif // FUTURE_HPP

#include "task.hpp"

#include <functional>

#include "promise.hpp"

using namespace Async;

FinalTask Async::spawnFinal(std::function<Task<void>()>&& awaitable) {
    co_await awaitable();
}

FinalTask::FinalTask([[maybe_unused]] std::coroutine_handle<promise_type> handle)
    : state(std::make_shared<CoState>()),
      handle(handle) {
    handle.promise().on_finish_callback([state = this->state] (std::exception_ptr e) {
        state->e = e;
        state->done = true;
    });
}


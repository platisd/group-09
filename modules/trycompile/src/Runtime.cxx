/*
 *  Runtime.cxx
 *  Copyright 2020 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <thread>
#include <utility>
#include "detail/Interrupt.hxx"
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Object.hxx"
#include "Runtime.hxx"

namespace smce {

SketchRuntime::~SketchRuntime() noexcept {
    switch (status) {
    case Status::uninitialized:
        [[fallthrough]];
    case Status::ready:
        break;
    case Status::running:
        [[fallthrough]];
    case Status::suspended:
        murder();
        break;
    case Status::loop_paused:
        exit_tok = true;
        stop_cv.notify_all();
        break;
    }
}

bool SketchRuntime::start() noexcept {
    if (status != Status::ready)
        return false;
    status = Status::running;
    thr.set_mutex((vehicle_dat->interrupt_mut = std::make_unique<std::recursive_mutex>()).get());
    thr.start([&] {
        vehicle_dat->board_thread_id = std::this_thread::get_id();
        curr_sketch.setup();
        while (true) {
            std::unique_lock lk{stop_mut};
            if (exit_tok)
                break;
            if (stop_tok) {
                stop_tok = false;
                stop_cv.wait(lk);
            }
            if (exit_tok)
                break;
            lk.unlock();
            curr_sketch.loop();
        }
    });
    return true;
}

bool SketchRuntime::resume() noexcept {
    switch (status) {
    case Status::loop_paused:
        stop_tok = false;
        stop_cv.notify_all();
        break;
    case Status::suspended:
        if (thr.resume())
            break;
        [[fallthrough]];
    default:
        return false;
    }
    status = Status::running;
    return true;
}

void SketchRuntime::pause_on_next_loop() noexcept {
    if (status != Status::running)
        return;
    std::lock_guard lk{stop_mut};
    stop_tok = true;
    status = Status::loop_paused;
}

void SketchRuntime::pause_now() noexcept {
    if (status != Status::running)
        return;
    assert(thr.suspend());
    status = Status::suspended;
}

void SketchRuntime::murder() noexcept {
    if (status != Status::running && status != Status::loop_paused && status != Status::suspended)
        return;
    assert(thr.murder());
    status = Status::murdered;
    clear();
}

bool SketchRuntime::set_sketch_and_car(SketchObject so, BoardData& bdata, const BoardInfo& binfo) noexcept {
    if (status != Status::uninitialized && status != Status::ready)
        return false;
    curr_sketch = load(std::move(so));
    if (!curr_sketch)
        return false;
    curr_sketch.init(&bdata, &binfo);
    vehicle_dat = &bdata;
    status = Status::ready;
    return true;
}

bool SketchRuntime::clear() {
    switch (status) {
    case Status::uninitialized:
        [[fallthrough]];
    case Status::ready:
        return true;
    case Status::running:
        [[fallthrough]];
    case Status::suspended:
        murder(); // murder set status = Status::murdered and calls clear() itself.
        return true;
    case Status::loop_paused:
        std::scoped_lock stop_lock{stop_mut};
        exit_tok = true;
        stop_cv.notify_all();
        break;
    }
    vehicle_dat = nullptr;
    curr_sketch.deinit();
    curr_sketch = {};
    thr.reset();
    stop_tok = exit_tok = false;
    status = Status::uninitialized;
    return true;
}

} // namespace smce
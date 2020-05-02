/*
 *  PerfectAnalogLaserSensor.cxx
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
 */

#include <range/v3/algorithm/find.hpp>
#include "components/PerfectAnalogLaserSensor.hxx"

constexpr auto err_msg = "Attempted to create component PerfectAnalogLaserSensor with an invalid configuration";

PerfectAnalogLaserSensor::PerfectAnalogLaserSensor(Urho3D::Context* context, BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin)
    : LaserCaster{context, node} {
    if (!pin.HasMember("bus_id") || pin.HasMember("address"))
        throw std::runtime_error{err_msg};

    bus_id = pin["bus_id"].GetUint();
    store.address = pin["address"].GetUint();

    bus = &bd.i2c_buses[bus_id];
    std::scoped_lock lock{bus->devices_mut};
    bus->slaves[store.address].second.emplace<1>([&](std::size_t size) mutable {
        std::scoped_lock lock{bus->devices_mut};
        auto& buf = bus->slaves[store.address].first;
        std::scoped_lock tx_lock{buf.rx_mutex, buf.tx_mutex};

        auto reg = static_cast<uint8_t>(buf.tx.back().front());
        buf.tx.erase(buf.tx.end() - 1);

        std::basic_string<std::byte> ret{};
        ret.resize(size);
        // Prevent reading beyond the rom
        if (reg + size > store.data.size()) {
            buf.rx.emplace_back(ret);
            return;
        }
        // Deal with read requests which need active intervention by the sensor.
        switch (reg) {
        case (MEASUREMENT_RESULT + 10): {
            uint16_t dist = measure();
            std::memcpy(&store.data[reg], &dist, sizeof(dist));
        } break;
        default:
            break;
        }
        // Place requested bytes into rx buffer
        std::memcpy(ret.data(), &store.data[reg], size);
        buf.rx.emplace_back(ret);
    });
}

void PerfectAnalogLaserSensor::Update(float timeStep) { p_laser_map::make_ticker(vlx, store, *bus, *this)(); }
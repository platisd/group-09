#include <filesystem>
#include <variant>
#include <catch2/catch.hpp>
#include <utility.hxx>
#include "Runtime.hxx"
#include "Toolchain.hxx"
#include "test_utility.hxx"

namespace stdfs = std::filesystem;

TEST_CASE("Arduino format compile and load"
          "[Arduino compile]") {
    constexpr std::string_view src = R"(extern "C" bool init() { return true; }
extern "C" void setup() {}
extern "C" void loop() {})";
    constexpr std::string_view src_name = "arduino.cpp";

    REQUIRE(write_file(src_name, src));
    const auto source_path = stdfs::absolute(stdfs::path(src_name));
    const auto ret = smce::compile_sketch(smce::SketchSource{source_path}, ".");
    std::visit(Visitor{[](const smce::SketchObject& so) {
                           auto ld = smce::load(so);
                           REQUIRE(ld);
                           REQUIRE(ld.init);
                           REQUIRE(ld.loop);
                           REQUIRE(ld.setup);
                           REQUIRE(ld.init(nullptr, nullptr, nullptr));
                       },
                       [](const std::runtime_error& err) { REQUIRE(false); }},
               ret);
}
#include <array>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <redasm/redasm.h>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
    rd_setloglevel(LOGLEVEL_TRACE);

    rd_addsearchpath("/home/davide/Dev/Progetti/Research/REDasmTech/"
                     "build_REDasmLib_Debug/plugins/src");

    if(rd_init()) {
        int result = Catch::Session().run(argc, argv);

        rd_deinit();
        return result;
    }

    return 1;
}

TEST_CASE("Type System") {
    // RDHandle hfile = rd_loadfile("/home/davide/Dev/Cavia.exe");
    RDBuffer* buffer =
        rd_loadfile("/home/davide/Dev/Campioni/PE Test/CM01.exe");
    REQUIRE(buffer);

    const RDTestResult* results = nullptr;
    usize c = rd_test(buffer, &results);
    REQUIRE(c > 0);

    rd_select(results[0].context);

    // const RDAnalyzer* analyzers = nullptr;
    // c = rd_getanalyzers(&analyzers);
    //
    // for(usize i = 0; i < c; i++)
    //     rdanalyzer_select(analyzers[i].name, true);
    //
    // rd_analyze();

    rd_disassemble();

    usize val = rd_sizeof("u8");
    REQUIRE(val == sizeof(u8));

    val = rd_sizeof("u16");
    REQUIRE(val == sizeof(u16));

    val = rd_sizeof("u32");
    REQUIRE(val == sizeof(u32));

    val = rd_sizeof("u64");
    REQUIRE(val == sizeof(u64));

    val = rd_sizeof("u16be");
    REQUIRE(val == sizeof(u16));

    val = rd_sizeof("u32be");
    REQUIRE(val == sizeof(u32));

    val = rd_sizeof("u64be");
    REQUIRE(val == sizeof(u64));

    val = rd_sizeof("char[4]");
    REQUIRE(val == sizeof(char) * 4);

    constexpr std::array<RDStructField, 2> POINT = {
        RDStructField{"u32", "x"},
        RDStructField{"u32", "u"},
    };

    const char* name = rd_createstruct("Point", POINT.data(), POINT.size());
    REQUIRE(name);

    val = rd_sizeof("Point");
    REQUIRE(val == sizeof(u32) * 2);

    constexpr std::array<RDStructField, 2> SIZE = {
        RDStructField{"u32", "width"},
        RDStructField{"u32", "width"},
    };

    name = rd_createstruct("Size", SIZE.data(), SIZE.size());
    REQUIRE(name);

    val = rd_sizeof("Size");
    REQUIRE(val == sizeof(u32) * 2);

    constexpr std::array<RDStructField, 2> RECT = {
        RDStructField{"Point", "position"},
        RDStructField{"Size", "size"},
    };

    name = rd_createstruct("Rect", RECT.data(), RECT.size());
    REQUIRE(name);

    val = rd_sizeof("Rect");
    REQUIRE(val == sizeof(u32) * 4);

    val = rd_sizeof("Rect[2]");
    REQUIRE(val == (sizeof(u32) * 4) * 2);
}

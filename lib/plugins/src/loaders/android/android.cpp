#include "dalvik.h"
#include "dex.h"

void rdplugin_init() {
    RDLoader ldex{};
    ldex.id = "dex";
    ldex.name = "Dalvik Executable";
    ldex.flags = LF_NOMERGE | LF_NOAUTORENAME;
    ldex.init = dex::init;
    rd_registerloader(&ldex);

    RDProcessor ldalvik;
    ldalvik.address_size = 2;
    ldalvik.integer_size = 2;
    ldalvik.id = "dalvik";
    ldalvik.name = "Dalvik VM";
    ldalvik.decode = dalvik::decode;
    ldalvik.emulate = dalvik::emulate;
    ldalvik.renderinstruction = dalvik::render_instruction;
    rd_registerprocessor(&ldalvik);
}

#pragma once

namespace redasm {

class Emulator;

namespace mem {

void merge_code(Emulator* e);
void process_memory();
void process_listing();

} // namespace mem

} // namespace redasm

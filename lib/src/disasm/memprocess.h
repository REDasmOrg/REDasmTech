#pragma once

namespace redasm {

class Emulator;

namespace memprocess {

void merge_code(Emulator* e);
void process_memory();
void process_listing();

} // namespace memprocess

} // namespace redasm

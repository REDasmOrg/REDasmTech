#include "abstractstream.h"

namespace redasm {

usize AbstractStream::seek(usize o) {
    usize oldpos = this->position;
    this->position = std::min(o - this->index_base(), this->size());
    return oldpos;
}

usize AbstractStream::index_base() const { return 0; }

tl::optional<typing::Value>
AbstractStream::peek_type(typing::FullTypeName tname) const {
    usize idx = this->index_base() + this->position;
    return this->buffer().get_type(idx, tname);
}

tl::optional<typing::Value>
AbstractStream::read_type(typing::FullTypeName tname) {
    usize pos = this->index_base() + this->position;
    auto v = this->buffer().get_type(pos, tname, pos);
    if(v)
        this->position = pos;
    return v;
}

tl::optional<std::string> AbstractStream::peek_str(usize n) const {
    if(this->position + n >= this->size())
        n = this->size() - this->position;

    return this->buffer().get_str(this->index_base() + this->position, n);
}

tl::optional<std::string> AbstractStream::peek_str() const {
    return this->buffer().get_str(this->index_base() + this->position);
}

tl::optional<std::string> AbstractStream::peek_wstr(usize n) const {
    if(this->position + n >= this->size())
        n = this->size() - this->position;

    return this->buffer().get_wstr(this->index_base() + this->position, n);
}

tl::optional<std::string> AbstractStream::peek_wstr() const {
    return this->buffer().get_wstr(this->index_base() + this->position);
}

tl::optional<std::string> AbstractStream::read_str(usize n) {
    auto s = this->peek_str(n);
    if(s)
        this->position += s->size();
    return s;
}

tl::optional<std::string> AbstractStream::read_str() {
    auto s = this->peek_str();
    if(s)
        this->position += s->size();
    return s;
}

tl::optional<std::string> AbstractStream::read_wstr(usize n) {
    auto s = this->peek_wstr(n);
    if(s)
        this->position += s->size();
    return s;
}

tl::optional<std::string> AbstractStream::read_wstr() {
    auto s = this->peek_wstr();
    if(s)
        this->position += s->size();
    return s;
}

tl::optional<bool> AbstractStream::peek_bool() const {
    return this->buffer().get_bool(this->position - this->index_base());
}

tl::optional<char> AbstractStream::peek_char() const {
    return this->buffer().get_char(this->position - this->index_base());
}

tl::optional<u8> AbstractStream::peek_u8() const {
    return this->buffer().get_u8(this->position - this->index_base());
}

tl::optional<u16> AbstractStream::peek_u16(bool big) const {
    return this->buffer().get_u16(this->position - this->index_base(), big);
}

tl::optional<u32> AbstractStream::peek_u32(bool big) const {
    return this->buffer().get_u32(this->position - this->index_base(), big);
}

tl::optional<u64> AbstractStream::peek_u64(bool big) const {
    return this->buffer().get_u64(this->position - this->index_base(), big);
}

tl::optional<i8> AbstractStream::peek_i8() const {
    return this->buffer().get_i8(this->position - this->index_base());
}

tl::optional<i16> AbstractStream::peek_i16(bool big) const {
    return this->buffer().get_i16(this->position - this->index_base(), big);
}

tl::optional<i32> AbstractStream::peek_i32(bool big) const {
    return this->buffer().get_i32(this->position - this->index_base(), big);
}

tl::optional<i64> AbstractStream::peek_i64(bool big) const {
    return this->buffer().get_i64(this->position - this->index_base(), big);
}

tl::optional<bool> AbstractStream::read_bool() {
    auto v = this->peek_bool();
    if(v)
        this->position += sizeof(bool);
    return v;
}

tl::optional<char> AbstractStream::read_char() {
    auto v = this->peek_char();
    if(v)
        this->position += sizeof(char);
    return v;
}

tl::optional<u8> AbstractStream::read_u8() {
    auto v = this->peek_u8();
    if(v)
        this->position += sizeof(u8);
    return v;
}

tl::optional<u16> AbstractStream::read_u16(bool big) {
    auto v = this->peek_u16(big);
    if(v)
        this->position += sizeof(u16);
    return v;
}

tl::optional<u32> AbstractStream::read_u32(bool big) {
    auto v = this->peek_u32(big);
    if(v)
        this->position += sizeof(u32);
    return v;
}

tl::optional<u64> AbstractStream::read_u64(bool big) {
    auto v = this->peek_u64(big);
    if(v)
        this->position += sizeof(u64);
    return v;
}

tl::optional<i8> AbstractStream::read_i8() {
    auto v = this->peek_i8();
    if(v)
        this->position += sizeof(i8);
    return v;
}

tl::optional<i16> AbstractStream::read_i16(bool big) {
    auto v = this->peek_i16(big);
    if(v)
        this->position += sizeof(i16);
    return v;
}

tl::optional<i32> AbstractStream::read_i32(bool big) {
    auto v = this->peek_i32(big);
    if(v)
        this->position += sizeof(i32);
    return v;
}

tl::optional<i64> AbstractStream::read_i64(bool big) {
    auto v = this->peek_i64(big);
    if(v)
        this->position += sizeof(i64);
    return v;
}

} // namespace redasm

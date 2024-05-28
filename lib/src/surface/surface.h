#pragma once

#include "../listing.h"
#include "../utils/object.h"
#include "renderer.h"
#include <memory>
#include <string>

namespace redasm {

class Surface: public Object {
public:
    explicit Surface();
    inline Renderer* renderer() const { return m_renderer.get(); }
    [[nodiscard]] usize current_index() const;
    [[nodiscard]] const Segment* current_segment() const;
    [[nodiscard]] RDSurfacePosition position() const;
    [[nodiscard]] RDSurfacePosition start_selection() const;
    [[nodiscard]] RDSurfacePosition end_selection() const;
    [[nodiscard]] std::string_view get_selected_text() const;
    [[nodiscard]] std::string_view get_text() const;
    [[nodiscard]] bool has_selection() const;
    void render(usize s, usize n);
    void set_columns(usize cols);
    void set_position(usize row, usize col);
    bool select_word(usize row, usize col);
    bool select(usize row, usize col);
    void seek(usize index);

private:
    void render_hexdump(const ListingItem& item);
    void render_segment(const ListingItem& item);
    void render_function(const ListingItem& item);
    void render_array(const ListingItem& item);
    void render_type(const ListingItem& item);
    void render_code(const ListingItem& item);
    void fit(usize& row, usize& col);
    void clear_selection();

public:
    usize start{static_cast<usize>(-1)};
    SurfaceRows rows;

private:
    std::unique_ptr<Renderer> m_renderer;
    mutable std::string m_strcache;
    usize m_row{0}, m_col{0};
    usize m_selrow{0}, m_selcol{0};
};

} // namespace redasm

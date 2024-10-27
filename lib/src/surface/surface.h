#pragma once

#include "../disasm/function.h"
#include "../listing.h"
#include "../memory/byte.h"
#include "../utils/object.h"
#include "renderer.h"
#include <memory>
#include <redasm/renderer.h>
#include <set>
#include <stack>
#include <string>

namespace redasm {

class Surface: public Object {
private:
    using History = std::stack<MIndex>;

public:
    explicit Surface(usize flags = SURFACE_DEFAULT);
    Renderer* renderer() const { return m_renderer.get(); }
    [[nodiscard]] bool can_goback() const { return !m_histback.empty(); }
    [[nodiscard]] bool can_goforward() const { return !m_histforward.empty(); }
    [[nodiscard]] tl::optional<MIndex> current_index() const;
    [[nodiscard]] tl::optional<usize> index_under_cursor() const;
    [[nodiscard]] const Function* current_function() const;
    [[nodiscard]] const Segment* current_segment() const;
    [[nodiscard]] RDSurfacePosition position() const;
    [[nodiscard]] RDSurfacePosition start_selection() const;
    [[nodiscard]] RDSurfacePosition end_selection() const;
    [[nodiscard]] std::string_view get_selected_text() const;
    [[nodiscard]] std::string_view get_text() const;
    [[nodiscard]] const std::vector<RDSurfacePath>& get_path() const;
    [[nodiscard]] bool has_selection() const;
    int index_of(MIndex index) const;
    int last_index_of(MIndex index) const;
    void clear_selection();
    void clear_history();
    void render_function(const Function& f);
    void render(usize n);
    void set_columns(usize cols);
    void set_position(usize row, usize col);
    bool select_word(usize row, usize col);
    bool select(usize row, usize col);
    bool go_back();
    bool go_forward();
    void seek_position(LIndex index);
    void seek(LIndex index);
    bool jump_to(MIndex index);
    bool jump_to_ep();

private:
    RDRendererParams create_render_params(const ListingItem& item) const;
    const ListingItem& get_listing_item(const SurfaceRow& sfrow) const;
    int calculate_index(usize idx) const;
    void update_history(History& history) const;
    void insert_path(Byte b, int fromrow, int torow) const;
    void render_finalize();
    void render_range(LIndex start, usize n);
    void render_hexdump(const ListingItem& item);
    void render_jump(const ListingItem& item);
    void render_segment(const ListingItem& item);
    void render_function(const ListingItem& item);
    void render_array(const ListingItem& item);
    void render_type(const ListingItem& item);
    void render_instr(const ListingItem& item);
    void render_comments(const ListingItem& item);
    void render_refs(const ListingItem& item);
    void fit(usize& row, usize& col);

    template<typename Callback>
    void lock_history(Callback cb) {
        m_lockhistory = true;
        cb();
        m_lockhistory = false;
    }

public:
    LIndex start{0};
    SurfaceRows rows;

private:
    std::unique_ptr<Renderer> m_renderer;
    History m_histback, m_histforward;
    mutable std::set<std::pair<usize, usize>> m_done;
    mutable std::vector<RDSurfacePath> m_path;
    mutable std::string m_strcache;
    bool m_lockhistory{false};
    usize m_row{0}, m_col{0};
    usize m_selrow{0}, m_selcol{0};
};

} // namespace redasm

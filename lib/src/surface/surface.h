#pragma once

#include "../disasm/function.h"
#include "../listing.h"
#include "../memory/byte.h"
#include "../utils/object.h"
#include "renderer.h"
#include <deque>
#include <memory>
#include <redasm/renderer.h>
#include <set>
#include <string>
#include <tl/optional.hpp>

namespace redasm {

class Surface: public Object {
private:
    struct HistoryItem {
        LIndex start;
        int row, col;

        bool operator==(const HistoryItem& rhs) const {
            return this->start == rhs.start && this->row == rhs.row &&
                   this->col == rhs.col;
        }

        bool operator!=(const HistoryItem& rhs) const {
            return !this->operator==(rhs);
        }
    };

    using History = std::deque<HistoryItem>;

public:
    explicit Surface(usize flags = SURFACE_DEFAULT);
    Renderer* renderer() const { return m_renderer.get(); }
    bool can_goback() const { return !m_histback.empty(); }
    bool can_goforward() const { return !m_histforward.empty(); }
    tl::optional<LIndex> current_listing_index() const;
    tl::optional<MIndex> current_index() const;
    tl::optional<usize> index_under_pos(RDSurfacePosition pos) const;
    tl::optional<usize> index_under_cursor() const;
    const Function* current_function() const;
    const Segment* current_segment() const;
    RDSurfacePosition position() const;
    RDSurfacePosition start_selection() const;
    RDSurfacePosition end_selection() const;
    std::string_view get_selected_text() const;
    std::string_view get_text() const;
    const std::vector<RDSurfacePath>& get_path() const;
    bool has_selection() const;
    bool has_rdil() const;
    int index_of(MIndex index) const;
    int last_index_of(MIndex index) const;
    void clear_selection();
    void clear_history();
    void render_function(const Function& f);
    void render(usize n);
    void set_rdil(bool v);
    void set_columns(usize cols);
    void set_position(int row, int col);
    bool select_word(int row, int col);
    bool select(int row, int col);
    bool go_back();
    bool go_forward();
    void seek_position(LIndex index);
    void seek(LIndex index);
    bool jump_to(MIndex index);
    bool jump_to_ep();

private:
    const ListingItem& get_listing_item(const SurfaceRow& sfrow) const;
    int calculate_index(usize idx) const;
    void update_history(History& history) const;
    void insert_path(Byte b, int fromrow, int torow) const;
    void render_finalize();
    void render_range(LIndex start, usize n);
    void render_hexdump(const ListingItem& item);
    void render_fill(const ListingItem& item);
    void render_label(const ListingItem& item);
    void render_segment(const ListingItem& item);
    void render_function(const ListingItem& item);
    void render_array(const ListingItem& item);
    void render_type(const ListingItem& item);
    void render_comment(const ListingItem& item);
    void render_refs(const ListingItem& item);
    void fit(int& row, int& col);
    int rows_count() const { return static_cast<int>(this->rows.size()); }

    int cols_count(int row) const {
        return static_cast<int>(this->rows[row].cells.size());
    }

    template<typename Callback>
    void lock_history(Callback cb) {
        m_lockhistory = true;
        cb();
        m_lockhistory = false;
    }

public:
    tl::optional<LIndex> start{0};
    SurfaceRows rows;

private:
    std::unique_ptr<Renderer> m_renderer;
    History m_histback, m_histforward;
    mutable std::set<std::pair<int, int>> m_done;
    mutable std::vector<RDSurfacePath> m_path;
    mutable std::string m_strcache;
    bool m_lockhistory{false};
    int m_row{0}, m_col{0};
    int m_selrow{0}, m_selcol{0};
};

} // namespace redasm

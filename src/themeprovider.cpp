#include "themeprovider.h"
#include "settings.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPalette>
#include <QVariant>
#include <cmath>

#define THEME_UI_SET_COLOR(theme, palette, key)                                \
    if(auto it = theme.find(#key); it != theme.end())                          \
        palette.setColor(QPalette::key, it.value().toString());

namespace themeprovider {

namespace {

QJsonDocument g_theme;
RDThemeColors g_colors{};

// clang-format off
const QHash<QString, RDThemeKind> THEMES = {
    {"foreground", THEME_FOREGROUND}, {"background", THEME_BACKGROUND},
    {"seek", THEME_SEEK},
    {"auto_comment", THEME_AUTOCOMMENT}, {"comment", THEME_COMMENT},
    {"highlightfg", THEME_HIGHLIGHTFG}, {"highlightbg", THEME_HIGHLIGHTBG},
    {"selectionfg", THEME_SELECTIONFG}, {"selectionbg", THEME_SELECTIONBG},
    {"cursor_fg", THEME_CURSORFG}, {"cursor_bg", THEME_CURSORBG},
    {"segment", THEME_SEGMENT},
    {"function", THEME_FUNCTION},
    {"address", THEME_ADDRESS},
    {"constant", THEME_CONSTANT},
    {"reg", THEME_REG},
    {"string", THEME_STRING},
    {"label", THEME_LABEL},
    {"data", THEME_DATA},
    {"pointer", THEME_POINTER},
    {"import", THEME_IMPORT},
    {"type", THEME_TYPE},
    {"nop", THEME_NOP},
    {"ret", THEME_RET},
    {"call", THEME_CALL},
    {"jump", THEME_JUMP},
    {"jump_c", THEME_JUMPCOND},
    {"entry_fg", THEME_ENTRYFG}, {"entry_bg", THEME_ENTRYBG},
    {"graph_bg", THEME_GRAPHBG}, {"graph_edge", THEME_GRAPHEDGE}, {"graph_edge_loop", THEME_GRAPHEDGELOOP}, {"graph_edge_loop_c", THEME_GRAPHEDGELOOPCOND},
    {"success", THEME_SUCCESS}, {"fail", THEME_FAIL}, {"warning", THEME_WARNING}};
// clang-format on

bool load_theme(const QString& theme) {
    if(!g_theme.isNull())
        return true;

    QFile f(QString(":/res/themes/%1.json").arg(theme.toLower()));
    if(!f.open(QFile::ReadOnly))
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);

    if(err.error != QJsonParseError::NoError)
        return false;

    QPalette palette = qApp->palette();
    QJsonObject obj = doc.object();

    // Apply some missing builtins
    if(!obj.contains("selectionfg"))
        obj["selectionfg"] = palette.color(QPalette::HighlightedText).name();

    if(!obj.contains("selectionbg"))
        obj["selectionbg"] = palette.color(QPalette::Highlight).name();

    doc.setObject(obj);

    g_theme.swap(doc);
    return true;
}

QStringList read_themes(const QString& path) {
    QStringList themes = QDir(path).entryList({"*.json"});

    for(QString& theme : themes) {
        theme.remove(".json");
        theme[0] = theme[0].toUpper();
    }

    return themes;
}

void apply_listing_theme() {
    QJsonObject theme = g_theme.object();

    for(auto it = theme.begin(); it != theme.end(); it++) {
        auto thit = THEMES.find(it.key());
        if(thit == THEMES.end())
            continue;

        rd_settheme(thit.value(), qUtf8Printable(it.value().toString()));
    }

    g_colors = rd_getfulltheme(); // Cache for performance
}

} // namespace

QStringList themes() { return themeprovider::read_themes(":/res/themes"); }

QString theme(const QString& name) {
    return QString(":/res/themes/%1.json").arg(name.toLower());
}

bool is_dark_theme() {
    QPalette p = qApp->palette();
    QColor c = p.window().color();

    double hsp = std::sqrt(0.299 * (c.red() * c.red()) +
                           0.587 * (c.green() * c.green()) +
                           0.114 * (c.blue() * c.blue()));

    return hsp <= 127.5;
}

QColor color(RDThemeKind kind) {
    if(kind >= THEME_COUNT)
        return {};

    return g_colors[kind];
}

QIcon icon(const QString& name) {
    REDasmSettings settings;

    return QIcon(QString(":/res/%1/%2.png")
                     .arg(themeprovider::is_dark_theme() ? "dark" : "light")
                     .arg(name));

    return {};
}

void apply_theme() {
    REDasmSettings settings;

    if(!themeprovider::load_theme(settings.current_theme()))
        return;

    QJsonObject theme = g_theme.object();
    QPalette palette = qApp->palette();

    THEME_UI_SET_COLOR(theme, palette, Shadow);
    THEME_UI_SET_COLOR(theme, palette, Base);
    THEME_UI_SET_COLOR(theme, palette, AlternateBase);
    THEME_UI_SET_COLOR(theme, palette, Text);
    THEME_UI_SET_COLOR(theme, palette, Window);
    THEME_UI_SET_COLOR(theme, palette, WindowText);
    THEME_UI_SET_COLOR(theme, palette, Button);
    THEME_UI_SET_COLOR(theme, palette, ButtonText);
    THEME_UI_SET_COLOR(theme, palette, Highlight);
    THEME_UI_SET_COLOR(theme, palette, HighlightedText);
    THEME_UI_SET_COLOR(theme, palette, ToolTipBase);
    THEME_UI_SET_COLOR(theme, palette, ToolTipText);

    qApp->setPalette(palette);
    themeprovider::apply_listing_theme();
}

} // namespace themeprovider

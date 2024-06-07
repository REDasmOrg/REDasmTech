#include "welcomeview.h"
#include "../fontawesome.h"
#include "../models/recentfilesdelegate.h"
#include "../models/recentfilesmodel.h"

namespace {

void style_social_button(QPushButton* button, const QIcon& icon) {
    const QString SOCIAL_STYLESHEET = QString("QPushButton {"
                                              "text-align: left;"
                                              "}");

    button->setFlat(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(SOCIAL_STYLESHEET);
    button->setIconSize({28, 28});
    button->setIcon(icon);
}

} // namespace

WelcomeView::WelcomeView(QWidget* parent): DashboardView{parent}, m_ui{this} {
    this->apply_logo(m_ui.lblbrand);

    m_ui.lvrecents->viewport()->setAttribute(Qt::WA_Hover);
    m_ui.lvrecents->viewport()->setBackgroundRole(QPalette::Window);

    m_ui.pbopen->setCursor(Qt::PointingHandCursor);
    m_ui.pbsettings->setCursor(Qt::PointingHandCursor);
    m_ui.pbabout->setCursor(Qt::PointingHandCursor);

    this->make_bordered(m_ui.pbopen);
    this->make_bordered(m_ui.pbsettings);
    this->make_bordered(m_ui.pbabout);

    style_social_button(m_ui.pbredasmws, FA_ICON(0xf015));
    style_social_button(m_ui.pbtwitter, FAB_ICON(0xf099));
    style_social_button(m_ui.pbtelegram, FAB_ICON(0xf3fe));
    style_social_button(m_ui.pbreddit, FAB_ICON(0xf281));
    style_social_button(m_ui.pbgithub, FAB_ICON(0xf113));

    auto* recentfilesmodel = new RecentFilesModel(m_ui.lvrecents);
    recentfilesmodel->update();

    m_ui.lvrecents->setItemDelegate(new RecentFilesDelegate(m_ui.lvrecents));
    m_ui.lvrecents->setModel(recentfilesmodel);

    connect(m_ui.pbopen, &QPushButton::clicked, this,
            &WelcomeView::open_requested);
    connect(m_ui.pbsettings, &QPushButton::clicked, this,
            &WelcomeView::settings_requested);
    connect(m_ui.pbabout, &QPushButton::clicked, this,
            &WelcomeView::about_requested);
    connect(m_ui.lvrecents, &QListView::clicked, this,
            &WelcomeView::on_file_selected);
}

void WelcomeView::on_file_selected(const QModelIndex& index) {
    const auto* recentfilesmodel =
        static_cast<const RecentFilesModel*>(index.model());

    Q_EMIT file_selected(recentfilesmodel->file_path(index));
}

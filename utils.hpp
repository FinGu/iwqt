#include <QWidget>

namespace Utils{
    void iwdNotUp(QWidget *parent = nullptr);
    void networkConfigure(const std::string &type, QWidget *parent = nullptr);
}

constexpr auto SORT_SETTING = "sort_type";
constexpr auto AVOID_SCANS_SETTING = "avoid_scans";

constexpr auto STRONG_ICON_PATH = ":/images/good.png";
constexpr auto MODERATE_ICON_PATH = ":/images/mid.png";
constexpr auto WEAK_ICON_PATH = ":/images/bad.png";
constexpr auto DISCONNECTED_ICON_PATH = ":/images/no.png";
constexpr auto FAILURE_ICON_PATH = ":/images/x.png";

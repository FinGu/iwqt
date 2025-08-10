#include <QWidget>

namespace Utils{
    void adapterNotFound(QWidget *parent = nullptr);

    void deviceNotFound(QWidget *parent = nullptr);
 
    void networkConfigure(const std::string &type, QWidget *parent = nullptr);

    QPixmap getIcon(const char *name);
}

constexpr auto SORT_SETTING = "sort_type";
constexpr auto AVOID_SCANS_SETTING = "avoid_scans";

constexpr auto EXCELLENT_ICON_PATH = ":/images/wireless-4.svg";
constexpr auto GOOD_ICON_PATH = ":/images/wireless-3.svg";
constexpr auto FAIR_ICON_PATH = ":/images/wireless-2.svg";
constexpr auto WEAK_ICON_PATH = ":/images/wireless-1.svg";
constexpr auto POOR_ICON_PATH = ":/images/wireless-0.svg";

constexpr auto DISCONNECTED_ICON_PATH = ":/images/wireless-disabled.svg";
constexpr auto FAILURE_ICON_PATH = ":/images/wireless-none.svg";

constexpr auto TRAY_ICON_SCALE = QSize(24,24);

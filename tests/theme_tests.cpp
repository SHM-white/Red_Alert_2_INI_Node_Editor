#include "../modern/ThemeStore.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QSettings>
#include <QTemporaryDir>
#include <QDebug>
static void require(bool condition, const char* why) { if (!condition) qFatal("Theme test: %s", why); }
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QTemporaryDir temp;
    require(temp.isValid(), "temporary directory");
    QSettings settings(temp.filePath("settings.ini"), QSettings::IniFormat);
    ThemeStore store(settings);
    require(store.all().size() == 4, "4 built-ins");
    require(store.byId("night").nodes.value("Infantry") != store.byId("light").nodes.value("Infantry"),
            "theme switches category colors");
    ThemeProfile custom = store.byId("night");
    custom.id.clear(); custom.name = "Custom Example";
    custom.ui["accent"] = QColor("#2468ac");
    custom.nodes["Infantry"] = QColor("#13579b");
    require(store.saveCustom(custom), "save custom");
    require(store.all().size() == 5, "custom added");
    auto json = ThemeStore::toJson(store.all().last());
    ThemeProfile copy; QString error;
    require(ThemeStore::fromJson(json, &copy, &error), "round-trip JSON");
    require(copy.nodes.value("Infantry") == QColor("#13579b"), "custom node color preserved");
    const QString jsonPath = temp.filePath("theme.json");
    require(store.exportFile(store.all().last().id, jsonPath, &error), "export");
    QString importedId;
    require(store.importFile(jsonPath, &importedId, &error), "import");
    require(store.all().size() == 6, "import creates a distinct theme");
    require(importedId != store.all()[4].id, "import cannot overwrite existing id");
    require(store.byId(importedId).nodes["Infantry"] == QColor("#13579b"), "import retains per-node colors");
    json["nodes"] = QJsonObject{};
    require(!ThemeStore::fromJson(json, &copy, &error), "reject incomplete theme");
    ThemeStore loaded(settings);
    require(loaded.all().size() == 6, "persist imported and custom themes");
    require(loaded.deleteCustom(loaded.all().last().id), "remove custom");
    require(loaded.all().size() == 5, "deleted");
    require(!loaded.deleteCustom("night"), "cannot delete built-ins");
    qInfo() << "Theme tests passed";
}

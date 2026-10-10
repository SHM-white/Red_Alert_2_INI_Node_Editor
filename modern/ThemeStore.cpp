#include "ThemeStore.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>
#include <QUuid>
#include <QRegularExpression>

namespace {
ThemeProfile preset(const QString& id, const QString& name, const QStringList& uiColors,
                    const QStringList& nodeColors) {
    ThemeProfile p;
    p.id = id; p.name = name; p.builtIn = true;
    const QStringList uiNames = ThemeStore::uiKeys();
    const QStringList nodeNames = ThemeStore::nodeKeys();
    for (int i = 0; i < uiNames.size(); ++i) p.ui.insert(uiNames[i], QColor(uiColors[i]));
    for (int i = 0; i < nodeNames.size(); ++i) p.nodes.insert(nodeNames[i], QColor(nodeColors[i]));
    return p;
}
}

QStringList ThemeStore::uiKeys() {
    return {"background", "panel", "surface", "text", "muted", "border", "accent", "canvas"};
}
QStringList ThemeStore::nodeKeys() {
    return {"Infantry", "Vehicle", "Aircraft", "Building", "Weapon", "Warhead",
            "Projectile", "Animation", "Superweapon", "Registry", "Other"};
}

ThemeStore::ThemeStore(QSettings& settings) : settings_(settings) {
    themes_.append(preset("night", "VS 深色",
        {"#1e1e1e", "#252526", "#333337", "#dddddd", "#a1a1a6", "#444449", "#007acc", "#1e1e1e"},
        {"#79dcb7", "#f0c075", "#83bbf4", "#c7a7f5", "#ef8c9b", "#f3aecc", "#8cadf5", "#79cfc9", "#f1d785", "#b7c5df", "#acb8cb"}));
    themes_.append(preset("light", "VS 浅色",
        {"#f3f3f3", "#ffffff", "#fafafa", "#242424", "#636363", "#d2d2d2", "#0078d4", "#f3f3f3"},
        {"#3b9270", "#b47c2e", "#3e86c0", "#9362c4", "#c65368", "#bd588f", "#586cba", "#268f83", "#9e7b25", "#687eaa", "#707b8e"}));
    themes_.append(preset("ocean", "海洋青蓝",
        {"#0c202c", "#123141", "#1a4050", "#e5faff", "#a0c8d2", "#2e5a67", "#4cc5c8", "#0c2835"},
        {"#66dac0", "#efbb72", "#62d2ee", "#98b7f7", "#ed91a8", "#f6b18d", "#79bcff", "#73d5cd", "#f0d67a", "#a5cfe3", "#a7c3d1"}));
    themes_.append(preset("red", "战场红黑",
        {"#1a161a", "#2a2027", "#382b35", "#f9eaf0", "#c6acb8", "#57404f", "#e37a92", "#20161c"},
        {"#8fc99d", "#f1ad79", "#99bde4", "#bd9de0", "#fb8e8e", "#e7a6ca", "#b8a0e9", "#88cfcd", "#f0d27b", "#d0b0bf", "#c6afb9"}));

    // Compatibility with PR2's per-built-in node color preferences.
    for (int i = 0; i < 4; ++i) {
        for (const QString& category : nodeKeys()) {
            const auto saved = settings_.value("color/" + QString::number(i) + "/" + category);
            const QColor color(saved.toString());
            if (saved.isValid() && color.isValid()) themes_[i].nodes[category] = color;
        }
    }
    const QJsonDocument stored = QJsonDocument::fromJson(settings_.value("themes/customProfiles").toByteArray());
    if (!stored.isArray()) return;
    for (const QJsonValue& val : stored.array()) {
        if (!val.isObject()) continue;
        ThemeProfile p;
        if (!fromJson(val.toObject(), &p)) continue;
        if (!p.id.startsWith("custom-")) continue;
        bool duplicate = false;
        for (const auto& existing : themes_)
            if (existing.id == p.id) duplicate = true;
        if (!duplicate) { p.builtIn = false; themes_.append(p); }
    }
}
QJsonObject ThemeStore::toJson(const ThemeProfile& theme) {
    QJsonObject object{{"schema", 1}, {"name", theme.name}, {"id", theme.id}};
    QJsonObject ui, nodes;
    for (const QString& name : uiKeys()) ui.insert(name, theme.ui.value(name).name(QColor::HexRgb));
    for (const QString& name : nodeKeys()) nodes.insert(name, theme.nodes.value(name).name(QColor::HexRgb));
    object.insert("ui", ui);
    object.insert("nodes", nodes);
    return object;
}
bool ThemeStore::fromJson(const QJsonObject& object, ThemeProfile* out, QString* error) {
    auto fail = [error](const QString& why) {
        if (error) *error = why;
        return false;
    };
    if (!out) return fail("Missing theme destination");
    if (object.value("schema").toInt() != 1) return fail("Unsupported theme schema (expected 1)");
    const QString name = object.value("name").toString().trimmed();
    if (name.isEmpty() || name.size() > 80) return fail("Theme name must have 1–80 characters");
    if (!object.value("ui").isObject() || !object.value("nodes").isObject())
        return fail("Theme must contain ui and nodes color maps");
    const QJsonObject ui = object.value("ui").toObject(), nodes = object.value("nodes").toObject();
    ThemeProfile theme; theme.name = name; theme.id = object.value("id").toString();
    for (const QString& key : uiKeys()) {
        const QString v = ui.value(key).toString();
        QColor color(v);
        if (!QRegularExpression("^#[0-9A-Fa-f]{6}$").match(v).hasMatch() || !color.isValid())
            return fail("Invalid UI color: " + key);
        theme.ui.insert(key, color);
    }
    for (const QString& key : nodeKeys()) {
        const QString v = nodes.value(key).toString();
        QColor color(v);
        if (!QRegularExpression("^#[0-9A-Fa-f]{6}$").match(v).hasMatch() || !color.isValid())
            return fail("Invalid node color: " + key);
        theme.nodes.insert(key, color);
    }
    *out = theme;
    return true;
}
ThemeProfile ThemeStore::byId(const QString& id) const {
    for (const auto& t : themes_) if (t.id == id) return t;
    return themes_.first();
}
bool ThemeStore::saveCustom(ThemeProfile theme) {
    if (theme.name.trimmed().isEmpty()) return false;
    if (!theme.id.startsWith("custom-"))
        theme.id = "custom-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
    theme.builtIn = false;
    for (auto& old : themes_) {
        if (old.id != theme.id) continue;
        if (old.builtIn) return false;
        old = theme; persist(); return true;
    }
    themes_.append(theme); persist(); return true;
}
bool ThemeStore::deleteCustom(const QString& id) {
    for (int i = 0; i < themes_.size(); ++i) {
        if (themes_[i].id != id) continue;
        if (themes_[i].builtIn) return false;
        themes_.removeAt(i); persist(); return true;
    }
    return false;
}
void ThemeStore::persist() const {
    QJsonArray custom;
    for (const auto& t : themes_) if (!t.builtIn) custom.append(toJson(t));
    settings_.setValue("themes/customProfiles", QJsonDocument(custom).toJson(QJsonDocument::Compact));
}
bool ThemeStore::importFile(const QString& path, QString* newId, QString* error) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) { if (error) *error = f.errorString(); return false; }
    if (f.size() > 128 * 1024) { if (error) *error = "Theme file exceeds 128 KiB"; return false; }
    QJsonParseError issue;
    const QJsonDocument json = QJsonDocument::fromJson(f.readAll(), &issue);
    if (issue.error != QJsonParseError::NoError || !json.isObject()) {
        if (error) *error = "Invalid JSON: " + issue.errorString();
        return false;
    }
    ThemeProfile theme;
    if (!fromJson(json.object(), &theme, error)) return false;
    theme.id.clear(); // Imported files cannot overwrite an existing profile.
    if (!saveCustom(theme)) { if (error) *error = "Cannot save theme"; return false; }
    if (newId) *newId = themes_.last().id;
    return true;
}
bool ThemeStore::exportFile(const QString& id, const QString& path, QString* error) const {
    if (path.isEmpty()) return false;
    const ThemeProfile theme = byId(id);
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly)) { if (error) *error = f.errorString(); return false; }
    const QByteArray payload = QJsonDocument(toJson(theme)).toJson(QJsonDocument::Indented);
    if (f.write(payload) != payload.size()) {
        if (error) *error = f.errorString();
        f.cancelWriting(); return false;
    }
    if (!f.commit()) { if (error) *error = f.errorString(); return false; }
    return true;
}

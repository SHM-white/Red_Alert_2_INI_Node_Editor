#pragma once

#include <QColor>
#include <QHash>
#include <QJsonObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVector>

// A single theme owns BOTH its entire UI palette and all RA2 node-category colors.
struct ThemeProfile {
    QString id;
    QString name;
    QHash<QString, QColor> ui;
    QHash<QString, QColor> nodes;
    bool builtIn = false;
};

class ThemeStore {
public:
    explicit ThemeStore(QSettings& settings);
    static QStringList uiKeys();
    static QStringList nodeKeys();
    static QJsonObject toJson(const ThemeProfile& theme);
    static bool fromJson(const QJsonObject& object, ThemeProfile* out, QString* error = nullptr);
    const QVector<ThemeProfile>& all() const { return themes_; }
    ThemeProfile byId(const QString& id) const;
    bool saveCustom(ThemeProfile theme);
    bool deleteCustom(const QString& id);
    bool importFile(const QString& path, QString* newId, QString* error);
    bool exportFile(const QString& id, const QString& path, QString* error) const;
private:
    void persist() const;
    QSettings& settings_;
    QVector<ThemeProfile> themes_;
};

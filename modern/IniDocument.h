#pragma once
#include <QByteArray>
#include <QHash>
#include <QStringList>
#include <QVector>

struct IniEntry { QString key, value; int line = -1; };
struct IniSection { QString name; int headerLine = -1; QVector<IniEntry> entries; };

class IniDocument {
public:
    enum class Encoding { Utf8, System };
    bool load(const QString& path, Encoding encoding, QString* error = nullptr);
    bool save(const QString& path, QString* error = nullptr);
    void clear();
    void setLines(const QStringList& lines);
    const QStringList& lines() const { return lines_; }
    const QVector<IniSection>& sections() const { return sections_; }
    const QString& path() const { return path_; }
    void setEncoding(Encoding encoding) { encoding_ = encoding; }
    Encoding encoding() const { return encoding_; }
    bool modified() const { return modified_; }
    void markModified(bool state = true) { modified_ = state; }
    int indexOf(const QString& name) const;
    QString category(int index) const;
    QVector<int> outgoing(int index) const;
    QVector<int> incoming(int index) const;
    bool addSection(const QString& name);
    bool deleteSection(int section);
    bool renameSection(int section, const QString& name);
    bool setKey(int section, const QString& key, const QString& value);
    bool renameKey(int section, const QString& oldKey, const QString& newKey);
    bool removeKey(int section, const QString& key);
private:
    static QString norm(const QString& s) { return s.trimmed().toCaseFolded(); }
    void rebuild();
    QStringList lines_;
    QByteArray originalBytes_;
    QVector<IniSection> sections_;
    QHash<QString, int> byName_;
    QHash<QString, QVector<int>> inbound_;
    QHash<QString, QString> categoryByName_;
    QString path_;
    Encoding encoding_ = Encoding::Utf8;
    QString newline_ = "\n";
    bool bom_ = false, modified_ = false;
};

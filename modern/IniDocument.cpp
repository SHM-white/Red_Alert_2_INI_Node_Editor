#include "IniDocument.h"
#include <QFile>
#include <QStringConverter>
#include <QSaveFile>
#include <QRegularExpression>
#include <QSet>

namespace {
QString categoryForList(const QString& name) {
    const QString n = name.toCaseFolded();
    if (n == "infantrytypes") return "Infantry";
    if (n == "vehicletypes") return "Vehicle";
    if (n == "aircrafttypes") return "Aircraft";
    if (n == "buildingtypes") return "Building";
    if (n == "warheads") return "Warhead";
    if (n == "projectiles") return "Projectile";
    if (n == "animations") return "Animation";
    if (n == "superweapontypes") return "Superweapon";
    return {};
}
QStringList tokens(const QString& value) {
    static const QRegularExpression rx("[,\\s]+");
    return value.split(rx, Qt::SkipEmptyParts);
}
}

bool IniDocument::load(const QString& path, Encoding encoding, QString* error) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        if (error) *error = f.errorString();
        return false;
    }
    const QByteArray bytes = f.readAll();
    encoding_ = encoding;
    path_ = path;
    bom_ = bytes.startsWith("\xEF\xBB\xBF");
    const QByteArray payload = bom_ ? bytes.mid(3) : bytes;
    QStringDecoder utf8(QStringConverter::Utf8);
    const QString content = encoding == Encoding::Utf8 ? utf8.decode(payload) : QString::fromLocal8Bit(payload);
    if (encoding == Encoding::Utf8 && utf8.hasError()) {
        if (error) *error = "Invalid UTF-8; re-open with System/ANSI encoding.";
        return false;
    }
    originalBytes_ = bytes;
    newline_ = content.contains("\r\n") ? "\r\n" : "\n";
    lines_ = content.isEmpty() ? QStringList{} :
        content.split(QRegularExpression("\r\n|\n|\r"), Qt::KeepEmptyParts);
    modified_ = false;
    rebuild();
    return true;
}
bool IniDocument::save(const QString& path, QString* error) {
    if (path.isEmpty()) { if (error) *error = "No output path"; return false; }
    const QString content = lines_.join(newline_);
    QByteArray out = modified_ ? (encoding_ == Encoding::Utf8 ? content.toUtf8() : content.toLocal8Bit()) : originalBytes_;
    if (modified_ && bom_ && encoding_ == Encoding::Utf8) out.prepend("\xEF\xBB\xBF", 3);
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly)) { if (error) *error = f.errorString(); return false; }
    if (f.write(out) != out.size()) { if (error) *error = f.errorString(); f.cancelWriting(); return false; }
    if (!f.commit()) { if (error) *error = f.errorString(); return false; }
    path_ = path; originalBytes_ = out; modified_ = false;
    return true;
}
void IniDocument::clear() {
    lines_.clear(); sections_.clear(); byName_.clear(); inbound_.clear();
    categoryByName_.clear(); path_.clear(); originalBytes_.clear(); newline_ = "\n"; bom_ = false; modified_ = false;
}
void IniDocument::setLines(const QStringList& lines) {
    lines_ = lines; rebuild(); modified_ = true;
}
int IniDocument::indexOf(const QString& name) const { return byName_.value(norm(name), -1); }
void IniDocument::rebuild() {
    sections_.clear(); byName_.clear(); inbound_.clear(); categoryByName_.clear();
    int current = -1;
    for (int i = 0; i < lines_.size(); ++i) {
        const QString line = lines_.at(i).trimmed();
        if (line.isEmpty() || line.startsWith(';') || line.startsWith('#')) continue;
        if (line.startsWith('[')) {
            const int end = line.indexOf(']');
            if (end > 1) {
                const QString name = line.mid(1, end - 1).trimmed();
                if (name.isEmpty()) continue;
                current = sections_.size();
                sections_.append(IniSection{name, i, {}});
                if (!byName_.contains(norm(name))) byName_.insert(norm(name), current);
                continue;
            }
        }
        const int eq = line.indexOf('=');
        if (current < 0 || eq <= 0) continue;
        QString value = line.mid(eq + 1);
        const int comment = value.indexOf(';');
        if (comment >= 0) value.truncate(comment);
        const QString key = line.left(eq).trimmed();
        if (!key.isEmpty()) sections_[current].entries.append({key, value.trimmed(), i});
    }
    for (const auto& section : sections_) {
        const QString category = categoryForList(section.name);
        if (category.isEmpty()) continue;
        for (const auto& entry : section.entries)
            for (const auto& token : tokens(entry.value))
                categoryByName_.insert(norm(token), category);
    }
    for (int i = 0; i < sections_.size(); ++i) {
        QSet<int> seen;
        for (const auto& entry : sections_[i].entries) {
            for (const auto& token : tokens(entry.value)) {
                const int target = indexOf(token);
                if (target >= 0 && target != i && !seen.contains(target)) {
                    inbound_[norm(sections_[target].name)].append(i);
                    seen.insert(target);
                }
            }
        }
    }
}
QString IniDocument::category(int index) const {
    if (index < 0 || index >= sections_.size()) return "Other";
    const auto& s = sections_[index];
    QString result = categoryByName_.value(norm(s.name));
    if (!result.isEmpty()) return result;
    if (!categoryForList(s.name).isEmpty()) return "Registry";
    QSet<QString> keys;
    for (const auto& entry : s.entries) keys.insert(norm(entry.key));
    if (keys.contains("verses") || keys.contains("cellspread")) return "Warhead";
    if (keys.contains("proximity") || keys.contains("rot")) return "Projectile";
    if (keys.contains("rof") && keys.contains("damage")) return "Weapon";
    if (keys.contains("power") || keys.contains("constructionyard")) return "Building";
    return "Other";
}
QVector<int> IniDocument::outgoing(int index) const {
    QVector<int> result;
    if (index < 0 || index >= sections_.size()) return result;
    QSet<int> seen;
    for (const auto& entry : sections_[index].entries)
        for (const auto& token : tokens(entry.value)) {
            const int target = indexOf(token);
            if (target >= 0 && target != index && !seen.contains(target)) {
                result.append(target); seen.insert(target);
            }
        }
    return result;
}
QVector<int> IniDocument::incoming(int index) const {
    if (index < 0 || index >= sections_.size()) return {};
    return inbound_.value(norm(sections_[index].name));
}
bool IniDocument::addSection(const QString& name) {
    const QString n = name.trimmed();
    if (n.isEmpty() || n.contains('[') || n.contains(']') || n.contains('\n') || indexOf(n) >= 0) return false;
    if (!lines_.isEmpty() && !lines_.last().isEmpty()) lines_.append("");
    lines_.append("[" + n + "]");
    rebuild(); modified_ = true;
    return true;
}
bool IniDocument::deleteSection(int section) {
    if (section < 0 || section >= sections_.size()) return false;
    const int begin = sections_[section].headerLine;
    const int end = section + 1 < sections_.size() ? sections_[section + 1].headerLine : lines_.size();
    for (int i = end - 1; i >= begin; --i) lines_.removeAt(i);
    rebuild(); modified_ = true; return true;
}
bool IniDocument::renameSection(int section, const QString& name) {
    const QString n = name.trimmed();
    if (section < 0 || section >= sections_.size() || n.isEmpty()
        || n.contains('[') || n.contains(']') || n.contains('\n')) return false;
    const int duplicate = indexOf(n);
    if (duplicate >= 0 && duplicate != section) return false;
    const int i = sections_[section].headerLine;
    const int start = lines_[i].indexOf('['), end = lines_[i].indexOf(']', start + 1);
    if (start < 0 || end < 0) return false;
    lines_[i].replace(start + 1, end - start - 1, n);
    rebuild(); modified_ = true; return true;
}
bool IniDocument::setKey(int section, const QString& key, const QString& value) {
    const QString k = key.trimmed();
    if (section < 0 || section >= sections_.size() || k.isEmpty() || k.contains('=')
        || k.contains('\n') || value.contains('\n')) return false;
    for (const auto& entry : sections_[section].entries) {
        if (norm(entry.key) != norm(k)) continue;
        const int i = entry.line, eq = lines_[i].indexOf('=');
        if (eq < 0) return false;
        const int semi = lines_[i].indexOf(';', eq + 1);
        const QString comment = semi < 0 ? QString() : lines_[i].mid(semi);
        lines_[i] = lines_[i].left(eq + 1) + value + (comment.isEmpty() ? "" : " " + comment);
        rebuild(); modified_ = true; return true;
    }
    int pos = section + 1 < sections_.size() ? sections_[section + 1].headerLine : lines_.size();
    if (pos == lines_.size() && !lines_.isEmpty() && lines_.last().isEmpty()) --pos;
    lines_.insert(pos, k + "=" + value);
    rebuild(); modified_ = true; return true;
}
bool IniDocument::renameKey(int section, const QString& oldKey, const QString& newKey) {
    const QString k = newKey.trimmed();
    if (section < 0 || section >= sections_.size() || k.isEmpty() || k.contains('=') || k.contains('\n')) return false;
    for (const auto& entry : sections_[section].entries)
        if (norm(entry.key) == norm(k) && norm(oldKey) != norm(k)) return false;
    for (const auto& entry : sections_[section].entries) {
        if (norm(entry.key) != norm(oldKey)) continue;
        const int i = entry.line, eq = lines_[i].indexOf('=');
        if (eq < 0) return false;
        const QString left = lines_[i].left(eq);
        int spaces = 0;
        while (spaces < left.size() && left.at(spaces).isSpace()) ++spaces;
        lines_[i] = left.left(spaces) + k + lines_[i].mid(eq);
        rebuild(); modified_ = true; return true;
    }
    return false;
}
bool IniDocument::removeKey(int section, const QString& key) {
    if (section < 0 || section >= sections_.size()) return false;
    for (const auto& entry : sections_[section].entries) {
        if (norm(entry.key) == norm(key)) {
            lines_.removeAt(entry.line); rebuild(); modified_ = true; return true;
        }
    }
    return false;
}

#include "IniDocument.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace {
std::string_view trim(std::string_view v) {
    while (!v.empty() && (v.front() == ' ' || v.front() == '\t')) v.remove_prefix(1);
    while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.remove_suffix(1);
    return v;
}

bool validUtf8(std::string_view s) {
    for (std::size_t i = 0; i < s.size();) {
        const auto c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { ++i; continue; }
        std::size_t n = c >= 0xC2 && c <= 0xDF ? 2 : c >= 0xE0 && c <= 0xEF ? 3 : c >= 0xF0 && c <= 0xF4 ? 4 : 0;
        if (!n || i + n > s.size()) return false;
        for (std::size_t j = 1; j < n; ++j)
            if ((static_cast<unsigned char>(s[i+j]) & 0xC0) != 0x80) return false;
        const auto b1 = static_cast<unsigned char>(s[i+1]);
        if ((c == 0xE0 && b1 < 0xA0) || (c == 0xED && b1 >= 0xA0) ||
            (c == 0xF0 && b1 < 0x90) || (c == 0xF4 && b1 >= 0x90)) return false;
        i += n;
    }
    return true;
}

bool legalName(std::string_view name) {
    if (trim(name).empty()) return false;
    for (char c : name) if (c == '\r' || c == '\n' || c == '[' || c == ']' || c == '=') return false;
    return true;
}

bool legalValue(std::string_view value) {
    return value.find('\r') == std::string_view::npos && value.find('\n') == std::string_view::npos;
}
}

std::string IniDocument::lower(std::string_view text) {
    std::string out(text);
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

void IniDocument::parse(std::string bytes) {
    lines_.clear(); sections_.clear(); bom_.clear(); eol_ = "\n";
    if (bytes.rfind("\xEF\xBB\xBF", 0) == 0) {
        bom_ = bytes.substr(0, 3); bytes.erase(0, 3);
    }
    utf8_ = !bom_.empty() || validUtf8(bytes);
    for (std::size_t i = 0; i < bytes.size();) {
        const auto next = bytes.find_first_of("\r\n", i);
        if (next == std::string::npos) {
            lines_.push_back({bytes.substr(i), ""}); break;
        }
        const bool crlf = bytes[next] == '\r' && next + 1 < bytes.size() && bytes[next+1] == '\n';
        const std::size_t len = crlf ? 2 : 1;
        std::string ending = bytes.substr(next, len);
        if (eol_ == "\n" && ending != "\n") eol_ = ending;
        lines_.push_back({bytes.substr(i, next-i), std::move(ending)});
        i = next + len;
    }
    reindex();
}

std::string IniDocument::serialize() const {
    std::size_t reserve = bom_.size();
    for (const auto& l : lines_) reserve += l.text.size() + l.eol.size();
    std::string out; out.reserve(reserve); out += bom_;
    for (const auto& l : lines_) { out += l.text; out += l.eol; }
    return out;
}

void IniDocument::reindex() {
    sections_.clear();
    for (std::size_t i = 0; i < lines_.size(); ++i) {
        const std::string& line = lines_[i].text;
        const auto text = trim(line);
        if (text.empty() || text.front() == ';' || text.front() == '#') continue;
        if (text.front() == '[') {
            const auto close = text.find(']');
            if (close != std::string_view::npos && close > 1) {
                const auto name = trim(text.substr(1, close-1));
                if (!name.empty()) {
                    if (!sections_.empty()) sections_.back().endLine = i;
                    sections_.push_back({std::string(name), i, lines_.size(), {}});
                    continue;
                }
            }
        }
        if (sections_.empty()) continue;
        const auto equal = line.find('=');
        if (equal == std::string::npos) continue;
        const auto key = trim(std::string_view(line).substr(0, equal));
        if (key.empty()) continue;
        std::size_t start = equal + 1;
        while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) ++start;
        std::size_t end = line.size();
        // In RA2 INI, semicolon comments are only recognized following whitespace.
        for (std::size_t n = start; n < line.size(); ++n) {
            if (line[n] == ';' && (n == start || line[n-1] == ' ' || line[n-1] == '\t')) {
                end = n; break;
            }
        }
        while (end > start && (line[end-1] == ' ' || line[end-1] == '\t')) --end;
        sections_.back().entries.push_back({std::string(key), line.substr(start, end-start), i, start, end});
    }
}

std::size_t IniDocument::findSection(std::string_view name) const {
    const auto wanted = lower(name);
    for (std::size_t i = 0; i < sections_.size(); ++i)
        if (lower(sections_[i].name) == wanted) return i;
    return sections_.size();
}

void IniDocument::insertLine(std::size_t at, std::string line) {
    if (at > 0 && lines_[at-1].eol.empty()) lines_[at-1].eol = eol_;
    const bool last = at == lines_.size();
    lines_.insert(lines_.begin() + static_cast<std::ptrdiff_t>(at), {std::move(line), last ? "" : eol_});
}

bool IniDocument::addSection(std::string name) {
    name = std::string(trim(name));
    if (!legalName(name) || findSection(name) != sections_.size()) return false;
    if (!lines_.empty() && !lines_.back().text.empty()) insertLine(lines_.size(), "");
    insertLine(lines_.size(), "[" + name + "]");
    reindex(); return true;
}

bool IniDocument::renameSection(std::size_t section, std::string name) {
    name = std::string(trim(name));
    if (!validSection(section) || !legalName(name)) return false;
    const auto existing = findSection(name);
    if (existing < sections_.size() && existing != section) return false;
    auto& line = lines_[sections_[section].headerLine].text;
    const auto open = line.find('['), close = line.find(']', open);
    if (open == std::string::npos || close == std::string::npos) return false;
    line.replace(open+1, close-open-1, name);
    reindex(); return true;
}

bool IniDocument::removeSection(std::size_t section) {
    if (!validSection(section)) return false;
    const auto begin = sections_[section].headerLine, end = sections_[section].endLine;
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(begin),
                 lines_.begin() + static_cast<std::ptrdiff_t>(end));
    reindex(); return true;
}

bool IniDocument::addKey(std::size_t section, std::string key, std::string value) {
    key = std::string(trim(key));
    if (!validSection(section) || !legalName(key) || !legalValue(value)) return false;
    for (const auto& entry : sections_[section].entries)
        if (lower(entry.key) == lower(key)) return false;
    insertLine(sections_[section].endLine, key + "=" + value);
    reindex(); return true;
}

bool IniDocument::renameKey(std::size_t section, std::size_t entry, std::string key) {
    key = std::string(trim(key));
    if (!validEntry(section, entry) || !legalName(key)) return false;
    const auto& sectionEntries = sections_[section].entries;
    for (std::size_t i = 0; i < sectionEntries.size(); ++i)
        if (i != entry && lower(sectionEntries[i].key) == lower(key)) return false;
    auto& line = lines_[sectionEntries[entry].line].text;
    const auto equal = line.find('=');
    const auto first = line.find_first_not_of(" \t");
    if (equal == std::string::npos || first == std::string::npos) return false;
    const auto last = line.find_last_not_of(" \t", equal-1);
    line.replace(first, last - first + 1, key);
    reindex(); return true;
}

bool IniDocument::setValue(std::size_t section, std::size_t entry, std::string value) {
    if (!validEntry(section, entry) || !legalValue(value)) return false;
    const auto& item = sections_[section].entries[entry];
    lines_[item.line].text.replace(item.valueStart, item.valueEnd-item.valueStart, value);
    reindex(); return true;
}

bool IniDocument::removeKey(std::size_t section, std::size_t entry) {
    if (!validEntry(section, entry)) return false;
    const auto line = sections_[section].entries[entry].line;
    lines_.erase(lines_.begin() + static_cast<std::ptrdiff_t>(line));
    reindex(); return true;
}

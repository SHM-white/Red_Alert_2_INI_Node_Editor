#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Lossless, line-oriented RA2 INI model. Original bytes, comments, line endings,
// and key ordering are preserved until the affected line is edited.
class IniDocument {
public:
    struct Entry {
        std::string key;
        std::string value;
        std::size_t line = 0;
        std::size_t valueStart = 0;
        std::size_t valueEnd = 0;
    };
    struct Section {
        std::string name;
        std::size_t headerLine = 0;
        std::size_t endLine = 0; // exclusive
        std::vector<Entry> entries;
    };

    void parse(std::string bytes);
    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] const std::vector<Section>& sections() const { return sections_; }
    [[nodiscard]] bool utf8() const { return utf8_; }
    [[nodiscard]] std::size_t lineCount() const { return lines_.size(); }
    [[nodiscard]] std::size_t findSection(std::string_view name) const;

    bool addSection(std::string name);
    bool renameSection(std::size_t section, std::string name);
    bool removeSection(std::size_t section);
    bool addKey(std::size_t section, std::string key, std::string value);
    bool renameKey(std::size_t section, std::size_t entry, std::string key);
    bool setValue(std::size_t section, std::size_t entry, std::string value);
    bool removeKey(std::size_t section, std::size_t entry);

    static std::string lower(std::string_view text);

private:
    struct Line { std::string text; std::string eol; };
    std::vector<Line> lines_;
    std::vector<Section> sections_;
    std::string bom_;
    std::string eol_ = "\n";
    bool utf8_ = true;
    void reindex();
    void insertLine(std::size_t at, std::string line);
    [[nodiscard]] bool validSection(std::size_t index) const { return index < sections_.size(); }
    [[nodiscard]] bool validEntry(std::size_t sec, std::size_t idx) const {
        return validSection(sec) && idx < sections_[sec].entries.size();
    }
};

#include "pch.hpp"
#include "ini.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <mutex>
#include <vector>

namespace {
    namespace fs = std::filesystem;

    std::mutex g_iniMutex;

    enum class LineKind { Blank, Comment, Section, KeyValue, Other };

    struct ParsedLine {
        LineKind kind = LineKind::Other;
        std::string name;
        size_t valueStart = 0;
    };

    struct ValueParts {
        std::string value;
        std::string comment;
    };

    struct Document {
        std::vector<std::string> lines;
        std::string original;
        bool crlf = true;
        bool bom = false;
    };

    struct Location {
        bool sectionFound = false;
        size_t keyLine = std::string::npos;
        size_t insertAt = 0;
    };

    std::string Trim(const std::string& s) {
        const size_t first = s.find_first_not_of(" \t");
        if (first == std::string::npos) return {};
        return s.substr(first, s.find_last_not_of(" \t") - first + 1);
    }

    bool EqualsNoCase(const std::string& a, const std::string& b) {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](unsigned char x, unsigned char y) {
            return std::tolower(x) == std::tolower(y);
        });
    }

    ParsedLine Parse(const std::string& line) {
        ParsedLine p;
        const size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos) {
            p.kind = LineKind::Blank;
            return p;
        }

        const char c = line[first];
        if (c == ';' || c == '#') {
            p.kind = LineKind::Comment;
        } else if (c == '[') {
            if (const size_t close = line.find(']', first); close != std::string::npos) {
                p.kind = LineKind::Section;
                p.name = Trim(line.substr(first + 1, close - first - 1));
            }
        } else if (const size_t eq = line.find('=', first); eq != std::string::npos) {
            p.kind = LineKind::KeyValue;
            p.name = Trim(line.substr(first, eq - first));
            p.valueStart = eq + 1;
        }
        return p;
    }

    ValueParts SplitValue(const std::string& raw) {
        ValueParts parts;
        const size_t cut = raw.find_first_of(";#");
        parts.value = Trim(raw.substr(0, cut));
        if (cut != std::string::npos) parts.comment = Trim(raw.substr(cut));

        if (parts.value.size() >= 2 && (parts.value.front() == '"' || parts.value.front() == '\'') && parts.value.front() == parts.value.back()) {
            parts.value = parts.value.substr(1, parts.value.size() - 2);
        }
        return parts;
    }

    std::string MakeComment(const char* comment) {
        std::string text = comment;
        std::replace(text.begin(), text.end(), '\r', ' ');
        std::replace(text.begin(), text.end(), '\n', ' ');
        text = Trim(text);
        return text.empty() ? std::string{} : "# " + text;
    }

    std::string FormatLine(const std::string& key, const std::string& value, const std::string& comment) {
        std::string line = key + "=" + value;
        if (!comment.empty()) line += " " + comment;
        return line;
    }

    Document Load(const fs::path& path) {
        Document doc;

        std::ifstream in(path, std::ios::binary);
        if (!in) return doc;

        doc.original.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());

        std::string text = doc.original;
        if (text.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            doc.bom = true;
            text.erase(0, 3);
        }

        if (const size_t nl = text.find('\n'); nl != std::string::npos) {
            doc.crlf = nl > 0 && text[nl - 1] == '\r';
        }

        size_t pos = 0;
        while (pos < text.size()) {
            size_t end = text.find('\n', pos);
            if (end == std::string::npos) end = text.size();

            size_t lineEnd = end;
            if (lineEnd > pos && text[lineEnd - 1] == '\r') --lineEnd;

            doc.lines.emplace_back(text, pos, lineEnd - pos);
            pos = end + 1;
        }
        return doc;
    }

    bool Save(const fs::path& path, const Document& doc) {
        const std::string eol = doc.crlf ? "\r\n" : "\n";

        std::string text = doc.bom ? "\xEF\xBB\xBF" : "";
        for (const auto& line : doc.lines) {
            text += line;
            text += eol;
        }

        if (text == doc.original) return true;

        auto writeFile = [&](const fs::path& target) {
            std::ofstream out(target, std::ios::binary | std::ios::trunc);
            if (!out) return false;
            out.write(text.data(), static_cast<std::streamsize>(text.size()));
            out.close();
            return !out.fail();
        };

        fs::path temp = path;
        temp += ".tmp";

        std::error_code ec;
        if (writeFile(temp)) {
            fs::rename(temp, path, ec);
            if (!ec) return true;
        }
        fs::remove(temp, ec);

        return writeFile(path);
    }

    Location Find(const Document& doc, const std::string& section, const std::string& key) {
        Location loc;
        bool inSection = false;

        for (size_t i = 0; i < doc.lines.size(); ++i) {
            const ParsedLine p = Parse(doc.lines[i]);

            if (p.kind == LineKind::Section) {
                if (inSection) break;
                if (EqualsNoCase(p.name, section)) {
                    inSection = true;
                    loc.sectionFound = true;
                    loc.insertAt = i + 1;
                }
                continue;
            }

            if (!inSection || p.kind != LineKind::KeyValue) continue;

            loc.insertAt = i + 1;
            if (EqualsNoCase(p.name, key)) {
                loc.keyLine = i;
                break;
            }
        }
        return loc;
    }

    void EnsureSectionSpacing(Document& doc) {
        for (size_t i = 1; i < doc.lines.size(); ++i) {
            if (Parse(doc.lines[i]).kind != LineKind::Section) continue;

            size_t start = i;
            while (start > 0 && Parse(doc.lines[start - 1]).kind == LineKind::Comment) --start;

            if (start > 0 && Parse(doc.lines[start - 1]).kind != LineKind::Blank) {
                doc.lines.insert(doc.lines.begin() + static_cast<std::ptrdiff_t>(start), std::string{});
                ++i;
            }
        }
    }

    void AlignComments(Document& doc) {
        constexpr size_t kGap = 4;

        struct Entry {
            size_t line;
            size_t cut;
            size_t leftLen;
        };

        std::vector<Entry> entries;
        size_t widest = 0;

        for (size_t i = 0; i < doc.lines.size(); ++i) {
            const ParsedLine p = Parse(doc.lines[i]);
            if (p.kind != LineKind::KeyValue) continue;

            const size_t cut = doc.lines[i].find_first_of(";#", p.valueStart);
            if (cut == std::string::npos) continue;

            const size_t leftLen = doc.lines[i].find_last_not_of(" \t", cut - 1) + 1;
            entries.push_back({ i, cut, leftLen });
            widest = std::max(widest, leftLen);
        }

        for (const Entry& e : entries) {
            std::string& line = doc.lines[e.line];
            line = line.substr(0, e.leftLen) + std::string(widest + kGap - e.leftLen, ' ') + line.substr(e.cut);
        }
    }

    void SetValue(Document& doc, const std::string& section, const std::string& key, const std::string& value, const char* comment) {
        const Location loc = Find(doc, section, key);

        if (loc.keyLine != std::string::npos) {
            const ParsedLine p = Parse(doc.lines[loc.keyLine]);
            const std::string newComment = comment ? MakeComment(comment) : SplitValue(doc.lines[loc.keyLine].substr(p.valueStart)).comment;
            doc.lines[loc.keyLine] = FormatLine(p.name, value, newComment);
        } else {
            const std::string line = FormatLine(key, value, comment ? MakeComment(comment) : std::string{});

            if (loc.sectionFound) {
                doc.lines.insert(doc.lines.begin() + static_cast<std::ptrdiff_t>(loc.insertAt), line);
            } else {
                if (!doc.lines.empty() && Parse(doc.lines.back()).kind != LineKind::Blank) doc.lines.emplace_back();
                doc.lines.push_back("[" + section + "]");
                doc.lines.push_back(line);
            }
        }

        EnsureSectionSpacing(doc);
        AlignComments(doc);
    }
}

std::string Ini::ReadOrCreate(const std::string& path, const std::string& section, const std::string& key, const std::string& defaultValue, const char* comment) {
    std::lock_guard<std::mutex> lock(g_iniMutex);

    const fs::path filePath(path);
    Document doc = Load(filePath);

    const Location loc = Find(doc, section, key);
    if (loc.keyLine != std::string::npos) {
        const std::string& line = doc.lines[loc.keyLine];
        return SplitValue(line.substr(Parse(line).valueStart)).value;
    }

    SetValue(doc, section, key, defaultValue, comment);
    Save(filePath, doc);
    return defaultValue;
}

bool Ini::Write(const std::string& path, const std::string& section, const std::string& key, const std::string& value, const char* comment) {
    std::lock_guard<std::mutex> lock(g_iniMutex);

    const fs::path filePath(path);
    Document doc = Load(filePath);
    SetValue(doc, section, key, value, comment);
    return Save(filePath, doc);
}

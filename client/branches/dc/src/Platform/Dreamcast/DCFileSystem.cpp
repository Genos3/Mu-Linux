// DCFileSystem.cpp - file access over KOS's filesystems (ISO9660 / VMU / SD / RAM).
//
// The GD-ROM (ISO9660) is case-insensitive-ish at level 2 but file extensions in
// the MU data set are mixed case, so Open() retries the leaf in lowercase and
// uppercase before giving up. Writable state (Config.ini, debug.log, HWID
// secret) lives on VMU or SD; the IFileSystem contract takes an explicit path
// anyway, so callers choose the root.
//
// TODO(fs):
//   * ReadIni*: full Win32 INI semantics (comments, quoted values, duplicate
//     keys -> last wins) - this is a small but careful parser; adapt the one in
//     Platform/SDL3/SDLFileSystem.cpp once KOS's iso9660 behaviour is pinned.
//   * WriteIniString: VMU/SD replace-in-place with tmp+rename once a writable
//     root is mounted.
//   * EnsureDirectory: vmufs has no dirs; SD (libkosfat) does - only meaningful there.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/IFileSystem.h"
#include <cctype>
#include <string>
#include <unordered_map>
#include <mutex>

namespace Platform {
namespace {
std::string Lower(std::string value) {
    for (char& c : value) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
    return value;
}
std::string Upper(std::string value) {
    for (char& c : value) if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 32);
    return value;
}
std::string Trim(std::string value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}
std::string Fold(std::string value) {   // backslashes -> forward slashes
    for (char& c : value) if (c == '\\') c = '/';
    // KOS: paths must start with the mount point. Bare "Data/..." paths
    // (all call sites in the game use those) resolve against /cd, the
    // GD-ROM mount point.
    if (!value.empty() && value[0] != '/') value = "/cd/" + value;
    return value;
}

struct IniDocument {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections;
    bool loaded = false;
};

// Case-insensitive section lookup (Config.ini sections are lowercase there).
bool ParseIni(const char* path, IniDocument& out, std::string& error)
{
    std::FILE* file = std::fopen(path, "rb");
    if (!file) { error = path; return false; }
    std::string section;
    char line[512];
    while (std::fgets(line, sizeof(line), file)) {
        std::string text = Trim(line);
        if (text.empty() || text[0] == ';' || text[0] == '#') continue;
        if (text.front() == '[' && text.back() == ']') {
            section = Lower(Trim(text.substr(1, text.size() - 2)));
            continue;
        }
        size_t eq = text.find('=');
        if (eq == std::string::npos) continue;
        std::string key = Lower(Trim(text.substr(0, eq)));
        std::string value = Trim(text.substr(eq + 1));
        out.sections[section][key] = value;
    }
    std::fclose(file);
    out.loaded = true;
    return true;
}

std::mutex iniMutex;
std::unordered_map<std::string, IniDocument> iniCache;
const IniDocument* IniFor(const char* path)
{
    std::lock_guard<std::mutex> lock(iniMutex);
    std::string key = Fold(path ? path : "");
    auto it = iniCache.find(key);
    if (it != iniCache.end()) return it->second.loaded ? &it->second : nullptr;
    IniDocument document;
    std::string error;
    if (!ParseIni(key.c_str(), document, error)) {
        iniCache[key] = IniDocument{};   // negative cache
        return nullptr;
    }
    iniCache[key] = std::move(document);
    return &iniCache[key];
}

class DCFileSystem final : public IFileSystem {
    std::string workingDirectory_ = "/cd";   // GD-ROM mount point in KOS examples
public:
    FILE* Open(const char* path, const char* mode) override
    {
        if (!path || !mode) return nullptr;
        std::string normalized = Fold(path);
        if (std::FILE* file = std::fopen(normalized.c_str(), mode)) return file;
        // ISO9660: retry the leaf lowercased, then the full path lowercased.
        size_t slash = normalized.find_last_of('/');
        std::string leaf = Lower(slash == std::string::npos ? normalized : normalized.substr(slash + 1));
        if (slash != std::string::npos) leaf = normalized.substr(0, slash + 1) + leaf;
        if (std::FILE* file = std::fopen(leaf.c_str(), mode)) return file;
        if (std::FILE* file = std::fopen(Lower(normalized).c_str(), mode)) return file;
        // ISO9660 level-2 (non-Joliet) names are stored UPPERCASE, often with
        // a ";1" version suffix that fopen hides. Try the uppercase variants.
        std::string upLeaf = slash == std::string::npos ? Upper(normalized)
                            : normalized.substr(0, slash + 1) + Upper(normalized.substr(slash + 1));
        if (std::FILE* file = std::fopen(upLeaf.c_str(), mode)) return file;
        if (std::FILE* file = std::fopen(Upper(normalized).c_str(), mode)) return file;
        return nullptr;
    }

    bool SetWorkingDirectory(const char* path) override { workingDirectory_ = Fold(path ? path : "/cd"); return true; }
    uint32_t WorkingDirectory(uint32_t capacity, char* output) override {
        if (output && capacity) { ::snprintf(output, capacity, "%s", workingDirectory_.c_str()); }
        return static_cast<uint32_t>(workingDirectory_.size() + 1);
    }
    bool EnsureDirectory(const char*) override { DC_NYI("EnsureDirectory (no mkdir on vmufs/iso9660)"); return false; }
    bool Exists(const char* path) override {
        if (std::FILE* file = Open(path, "rb")) { std::fclose(file); return true; }
        return false;
    }
    uint32_t ExecutablePath(char* output, uint32_t capacity) override {
        static const char kPath[] = "/cd/mu.elf";   // actual name chosen at image build time
        if (output && capacity) ::snprintf(output, capacity, "%s", kPath);
        return static_cast<uint32_t>(sizeof(kPath));
    }
    uint32_t ReadIniString(const char* section, const char* key, const char* fallback,
                           char* output, uint32_t capacity, const char* path) override
    {
        if (!output || capacity == 0) return 0;
        output[0] = 0;
        const IniDocument* document = IniFor(path);
        if (document) {
            auto sit = document->sections.find(Lower(section ? section : ""));
            if (sit != document->sections.end()) {
                auto kit = sit->second.find(Lower(key ? key : ""));
                if (kit != sit->second.end()) {
                    const std::string& value = kit->second;
                    size_t copy = value.size() < capacity - 1 ? value.size() : capacity - 1;
                    std::memcpy(output, value.c_str(), copy); output[copy] = 0;
                    return static_cast<uint32_t>(copy + 1);
                }
            }
        }
        if (fallback) {
            size_t copy = std::strlen(fallback);
            if (copy >= capacity) copy = capacity - 1;
            std::memcpy(output, fallback, copy); output[copy] = 0;
        }
        return static_cast<uint32_t>(std::strlen(output) + 1);
    }
    uint32_t ReadIniSection(const char* section, char* output, uint32_t capacity, const char* path) override
    {
        if (!output || capacity == 0) return 0;
        output[0] = 0;
        const IniDocument* document = IniFor(path);
        if (!document) return 0;
        auto sit = document->sections.find(Lower(section ? section : ""));
        if (sit == document->sections.end()) return 0;
        uint32_t position = 0;
        auto AppendPair = [&](const std::string& key, const std::string& value) {
            std::string pair = key; pair.push_back('='); pair += value;
            if (position + pair.size() + 1 > capacity) return;
            std::memcpy(output + position, pair.c_str(), pair.size());
            position += static_cast<uint32_t>(pair.size());
            output[position++] = '\0';
        };
        // Same flat format as Win32: key=value\0key2=value2\0...\0
        for (const auto& pair : sit->second) AppendPair(pair.first, pair.second);
        if (position) output[position++] = '\0';   // final terminator
        return position;
    }
    bool WriteIniString(const char*, const char*, const char*, const char*) override {
        DC_NYI("WriteIniString (writable Config.ini on VMU/SD not mounted yet)");
        return false;
    }
};
} // namespace

IFileSystem& GetFileSystem()
{
    static DCFileSystem fileSystem;
    return fileSystem;
}
} // namespace Platform

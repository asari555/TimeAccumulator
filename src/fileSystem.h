#ifndef TIME_ACCUMULATOR_FILESYSTEM_H
#define TIME_ACCUMULATOR_FILESYSTEM_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <filesystem>
#include <cmath>
#include <vector>
#include <mutex>
#include <thread>

#define EXTENSION_NAME "msgpack"

#define UNAME_MAX 64
#define MAIN_FOLDER_NAME_LENGTH 17
#define ASCII_ZERO 48
#define REQUIRED_FILE_NAME_LENGTH 5
#define REQUIRED_FILE_NAME_LAST_ELEMENT_INDEX 4
#define EXTENSION_NAME_LENGTH 7

namespace backend {

    template<class InputIterator, class T>
    uint32_t findIndex(InputIterator first, InputIterator last, const T &value) noexcept {

        uint32_t index = 0;
        while (first != last) {
            if (*first == value) { return index; }
            ++first;
            ++index;
        }

        return UINT32_MAX;
    }

    class FileSystem {
    private:
        /// @definition Folder name standard.
        const char* _programFolderName = ".time-accumulator";

        /// @definition Temporary variable for inner folders and files path.
        std::string _absolutePath{};
        /// @definition Temporary variable for path.
        char _tempPath[PATH_MAX]{};

        char _path[PATH_MAX]{};
        char _homeName[PATH_MAX]{};

        struct stat _sb{};

        std::mutex commonMutex;

    private:
        void _cleanCharString(char string[] = {}, uint32_t stringLength = 0) const noexcept;

        void _removeExtension(std::vector<std::string>& fileList) const noexcept;

        void _removeExtension(std::string& fileName) const noexcept;

        void _attachStringToBasicTempPath(const std::string &stringAttached) noexcept;

        void _appendCharString(
                char *targetString = {},
                uint32_t targetStringLength = 0,
                const char *sourceCharString = {},
                uint32_t sourceCharStringLength = 0,
                const char &bond = '\0'
                ) const noexcept;

        void _appendCharString(
                char *targetString = {},
                uint32_t targetStringLength = 0,
                const std::string &sourceString = {},
                const char &bond = '\0'
                ) const noexcept;

        void _modifyAbsolutePath(const std::string& folderName, const std::string& fileName) noexcept;

        [[nodiscard]] std::string _findMatchedSimilarName(const std::string& givenName) const noexcept;

        std::string _defaultData(const std::string& folderName, const std::string& fileName);

        void _fillFileNameList(std::vector<std::string>& list, const std::string& folderName = {}) const noexcept;

        void _removeExcessFolderPathFromList(std::vector<std::string>& list) const noexcept;

        uint16_t _getFileNumber(const std::string* fileName) const noexcept;

        [[nodiscard]] uint16_t _giveLatestFileName(
                std::string& latestFileName,
                std::vector<std::string> fileNameList
                ) const noexcept;

        void _deleteFile(const std::string& fileName = {}, const std::string& folderName = {}) noexcept;

        bool _checkFolder(const std::string& folderName = {}) noexcept;

        bool _checkFile(const std::string& folderName, const std::string& fileName) noexcept;

        void _createFolder(const std::string& folderName = {}) noexcept;

        void _createFile(const std::string& folderName, const std::string &fileName) noexcept;

        bool _is_empty(std::ifstream& pFile) noexcept;

    public:
        /// @definition Folder name standard.
        const char* yearlyFolderNameStd = "year";
        /// @definition File name standard.
        const char* weeklyFileNameStd = "week";
        /// @definition File name standard.
        const char* weeklySummaryFileNameStd = "weekly_summary";
        /// @definition File name standard.
        const char* monthlySummaryFileNameStd = "monthly_summary";
        /// @definition File name standard.
        const char* yearlySummaryFileNameStd = "yearly_summary";
        /// @definition File name standard.
        const char* settingsFileNameStd = "settings";

    public:
        void setup();

        FileSystem();

//        void deleteDossier(const std::string& fileName = {}, const std::string& folderName = {}) noexcept;

        void saveFile(
                const std::string& transferredString,
                const std::string& fileName,
                const std::string& folderName = {},
                bool clearAndWrite = false
                ) noexcept;

        void loadFile(
                std::string& loadedString,
                const std::string& fileName,
                const std::string& folderName = {}
                ) noexcept;

        [[nodiscard]] std::string giveTheLatestYearFolder() noexcept;

        [[nodiscard]] std::string giveTheLatestWeekFile(const std::string& folderName) noexcept;

        void createNewYearFolder(std::string& latestFolderName);

        void createNewWeekFile(std::string& latestFolderName, std::string& latestFileName) noexcept;

        void deleteProgramFolder() noexcept;

//        void deleteAllWeekFiles(const std::string& latestFolderName) noexcept;
    };
}

#endif //TIME_ACCUMULATOR_FILESYSTEM_H

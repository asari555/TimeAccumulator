#include "fileSystem.h"

void backend::FileSystem::_cleanCharString(char* string, uint32_t stringLength) const noexcept {
    std::fill(string, string + stringLength, '\0');
}

void backend::FileSystem::_removeExtension(std::vector<std::string>& fileList) const noexcept {
    std::string tempFileName{};
    for (auto& fileName : fileList) {
        for (const auto& character : fileName) {
            if (character == '.') { break; }
            tempFileName.push_back(character);
        }
        fileName = tempFileName;
        tempFileName = {};
    }
}

void backend::FileSystem::_removeExtension(std::string& fileName) const noexcept {
    std::string tempFileName{};
    for (const auto& character : fileName) {
        if (character == '.') { break; }
        tempFileName.push_back(character);
    }

    fileName = tempFileName;
    tempFileName = {};
}

/// @details The variables do not need mutex in this function.
void backend::FileSystem::_attachStringToBasicTempPath(const std::string& stringAttached) noexcept {

    _cleanCharString(_tempPath, PATH_MAX);

    std::string tempString = _path;
    if (!stringAttached.empty()) {
        tempString.push_back('/');
        tempString.append(stringAttached);
    }

    uint32_t count = 0;
    for (const auto& character : tempString) {

        if (character == '\0') { break; }
        _tempPath[count++] = character;
    }
}

/// Appends char string as a source to target char array with option to take forward-slash for a path.
void backend::FileSystem::_appendCharString(
        char* targetString,
        uint32_t targetStringLength,
        const char* sourceCharString,
        uint32_t sourceCharStringLength,
        const char& bond
) const noexcept {

    if (targetStringLength == 0) { return; }
    uint32_t startIndexForAppending = findIndex(targetString,targetString + targetStringLength,'\0');

    if (sourceCharStringLength == 0) { return; }
    bool shiftByOne = false;
    char tempChar;

    if (bond != '\0') {
        targetString[startIndexForAppending] = bond;
        shiftByOne = true;
    }
    for (uint32_t i = 0; i < sourceCharStringLength; ++i) {

        tempChar = sourceCharString[i];
        if (tempChar == '\0') { break;}

        targetString[startIndexForAppending + i + shiftByOne] = tempChar;
    }
}

/// Appends standard string as a source to target char array with option to take forward-slash for a path.
void backend::FileSystem::_appendCharString(
        char* targetString,
        uint32_t targetStringLength,
        const std::string& sourceString,
        const char& bond
) const noexcept {

    if (targetStringLength == 0) { return; }
    uint32_t startIndexForAppending = findIndex(targetString,targetString + targetStringLength,'\0');

    if (sourceString.empty()) { return; }
    uint32_t targetIndex = 0;

    if (bond != '\0') {
        targetString[startIndexForAppending + targetIndex] = bond;
        ++targetIndex;
    }

    for (auto& character : sourceString) {
        targetString[startIndexForAppending + targetIndex] = character;
        ++targetIndex;
    }
}

void backend::FileSystem::_modifyAbsolutePath(const std::string& folderName, const std::string& fileName) noexcept {

    _absolutePath = {};
    _absolutePath = _path;

    if (!folderName.empty()) {
        _absolutePath.append('/' + folderName);
    }
    if (!fileName.empty()) {
        _absolutePath.append('/' + fileName);

        bool noExtension = true;
        std::string temp = {};
        std::for_each(fileName.begin(), fileName.end(),[&](const auto& character) {
            if (character == '.') { noExtension = false; }
        });
        if (noExtension) { _absolutePath.append(".msgpack"); }
    }
}

[[nodiscard]] std::string backend::FileSystem::_findMatchedSimilarName(const std::string& givenName) const noexcept {

    std::string tempName = givenName;
    _removeExtension(tempName);

    bool erasable = false;
    uint16_t index;
    char currentChar;
    for (auto& character : tempName) {
        index = &character - &tempName[0];
        currentChar = tempName[index];
        if (currentChar == '.' || currentChar == '-') { erasable = true; }
        if (!erasable) { continue; }
        if (index >= tempName.size()) { continue; }
        tempName.erase(index);
    }

    if (tempName == yearlyFolderNameStd) { return yearlyFolderNameStd; }
    if (tempName == yearlySummaryFileNameStd) { return yearlySummaryFileNameStd; }
    if (tempName == weeklyFileNameStd) { return weeklyFileNameStd; }
    if (tempName == weeklySummaryFileNameStd) { return weeklySummaryFileNameStd; }
    if (tempName == monthlySummaryFileNameStd) { return monthlySummaryFileNameStd; }

    return "";
}

std::string backend::FileSystem::_defaultData(const std::string& folderName, const std::string& fileName) {

    std::string matchedDefaultFileName = _findMatchedSimilarName(fileName);

    if (!_checkFile(yearlyFolderNameStd, matchedDefaultFileName)) {
        std::stringstream tempString;
        tempString << "Unable to read file ";
        tempString << fileName << " at " << folderName << " folder.";

        throw std::runtime_error(tempString.str());
    }

    _modifyAbsolutePath(yearlyFolderNameStd, matchedDefaultFileName);

    std::string input{};

    std::ifstream file(_absolutePath);
    if (!file.is_open()) {
        throw std::runtime_error("Unable To Read Default File");
    }

    char c;
    while (file.get(c)) {
        input.push_back(c);
    }
    _absolutePath = {};
    file.close();

    return input;
}

void backend::FileSystem::_fillFileNameList(
        std::vector<std::string>& list,
        const std::string& folderName
) const noexcept {

    std::string localPath;
    if (folderName.empty()) { localPath = _path; }
    else {
        localPath = _path;
        localPath.push_back('/');
        localPath.append(folderName);
    }

    for (const auto& directoryEntry : std::filesystem::directory_iterator(localPath)) {
        list.emplace_back(directoryEntry.path());
    }
}

void backend::FileSystem::_removeExcessFolderPathFromList(std::vector<std::string>& list) const noexcept {

    uint32_t lastElementIndex;
    uint32_t elementSize;
    std::string tempString{};
    for (auto& element : list) {
        elementSize = element.size();
        lastElementIndex = elementSize - 1;
        for (uint32_t i = lastElementIndex; i >= 0; --i) {
            if (element[i] == '/') { break; }
            tempString.push_back(element[i]);
        }
        std::reverse(tempString.begin(), tempString.end());
        element = tempString;
        tempString = {};
    }
}

uint16_t backend::FileSystem::_getFileNumber(const std::string* fileName) const noexcept {

    std::string tempFileName = *fileName;
    uint16_t currentValue = 0;

    _removeExtension(tempFileName);
    std::reverse(tempFileName.begin(), tempFileName.end());
    uint16_t decimalLevel = 1;

    for (const auto& character : tempFileName) {

        if (character == '-') { break; }
        if (character < 48 || character > 57) {
            currentValue = 0;
            break;
        }

        currentValue += (character - ASCII_ZERO) * decimalLevel;
        decimalLevel *= 10;
    }

    return currentValue;
}

[[nodiscard]] uint16_t backend::FileSystem::_giveLatestFileName(
        std::string& latestFileName,
        std::vector<std::string> fileNameList
) const noexcept {

    uint32_t elementSize;
    uint16_t currentValue;
    uint16_t maximum = 0;
    for (auto& fileName : fileNameList) {
        elementSize = fileName.size();

        if (elementSize < REQUIRED_FILE_NAME_LENGTH || fileName[REQUIRED_FILE_NAME_LAST_ELEMENT_INDEX] != '-') {
            continue;
        }

        currentValue = _getFileNumber(&fileName);

        if (currentValue > maximum) {
            std::reverse(fileName.begin(), fileName.end());
            latestFileName = fileName;
            maximum = currentValue;
        }
        currentValue = 0;
    }

    if (!latestFileName.empty()) {
        std::reverse(latestFileName.begin(), latestFileName.end());
    }

    return maximum;
}

void backend::FileSystem::_deleteFile(const std::string& fileName, const std::string& folderName) noexcept {

    _cleanCharString(_tempPath);
    _attachStringToBasicTempPath(folderName);

    if (!fileName.empty()) {
        _appendCharString(_tempPath, PATH_MAX, fileName, '/');
        _appendCharString(_tempPath, PATH_MAX, EXTENSION_NAME, EXTENSION_NAME_LENGTH, '.');
    }

    if (!std::filesystem::remove_all(_tempPath)) {
        std::cerr << "Error: " << _tempPath << " File Not Deleted" << std::endl;
    }
    _cleanCharString(_tempPath);
}

bool backend::FileSystem::_checkFolder(const std::string& folderName) noexcept {

    if (folderName.empty()) {
        if (stat(_path, &_sb) == 0 && S_ISDIR(_sb.st_mode)) {
            return true;
        }
        return false;
    }
    _attachStringToBasicTempPath(folderName);

    if (stat(_tempPath, &_sb) == 0 && S_ISDIR(_sb.st_mode)) {
        _cleanCharString(_tempPath, PATH_MAX);
        return true;
    }
    return false;
}

bool backend::FileSystem::_checkFile(const std::string& folderName, const std::string& fileName) noexcept {

    _modifyAbsolutePath(folderName, fileName);

    std::ifstream file(_absolutePath);
    if (!file.is_open()) { return false; }
    file.close();
    _absolutePath = {};

    return true;
}

void backend::FileSystem::_createFolder(const std::string& folderName) noexcept {

    if (folderName.empty() && mkdir(_path, 0755) != -1) { return; }
    std::cerr << "Error : " << _path << " Folder Not Created Nor Found" << std::endl;

    _attachStringToBasicTempPath(folderName);

    if (!std::filesystem::create_directory(_tempPath)) {
        std::cerr << "Error : " << _tempPath << " Folder Not Created Nor Found" << std::endl;
        _cleanCharString(_tempPath, PATH_MAX);
    }

    _cleanCharString(_tempPath, PATH_MAX);
}

void backend::FileSystem::_createFile(const std::string& folderName, const std::string& fileName) noexcept {

    if (!_checkFolder(folderName)) { _createFolder(folderName); }

    _modifyAbsolutePath(folderName, fileName);

    std::ofstream file(_absolutePath);
    if (!file.is_open()) {
        std::cerr << "Error: " << _absolutePath << " File Not Created" << std::endl;
    }
    _absolutePath = {};
}

bool backend::FileSystem::_is_empty(std::ifstream& pFile) noexcept {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void backend::FileSystem::setup() {

    std::string homename = std::getenv("HOME");
    char _homename[PATH_MAX];

    _cleanCharString(_homename, PATH_MAX);

    char tempChar;
    for (int i = 0; i < PATH_MAX; ++i) {
        tempChar = homename[i];

        if (tempChar == '\0') { break; }

        _homename[i] = tempChar;
    }

    _cleanCharString(_tempPath, PATH_MAX);
    _cleanCharString(_path, PATH_MAX);

    // Main folder setup
    _appendCharString(_path, PATH_MAX, _homename, PATH_MAX);
    _appendCharString(_path, PATH_MAX, _programFolderName, MAIN_FOLDER_NAME_LENGTH, '/');

    if (!_checkFolder()) { _createFolder(); }

    // Default year folder setup
    if (!_checkFolder(yearlyFolderNameStd)) { _createFolder(yearlyFolderNameStd); }

    // Default weekly file setup
    if (!_checkFile(yearlyFolderNameStd, weeklyFileNameStd)) {
        _createFile(yearlyFolderNameStd, weeklyFileNameStd);
    }

    // Default weekly summary setup
    if (!_checkFile(yearlyFolderNameStd, weeklySummaryFileNameStd)) {
        _createFile(yearlyFolderNameStd, weeklySummaryFileNameStd);
    }

    // Default monthly summary setup
    if (!_checkFile(yearlyFolderNameStd, monthlySummaryFileNameStd)) {
        _createFile(yearlyFolderNameStd, monthlySummaryFileNameStd);
    }

    // Default yearly summary setup
    if (!_checkFile("", yearlySummaryFileNameStd)) {
        _createFile("", yearlySummaryFileNameStd);
    }

    // Default customised color file setup
    if (!_checkFile("", settingsFileNameStd)) {
        _createFile("", settingsFileNameStd);
    }
}

backend::FileSystem::FileSystem() {
    setup();
}

//void backend::FileSystem::deleteDossier(
//        const std::string& fileName,
//        const std::string& folderName
//        ) noexcept {
//
//    std::lock_guard<std::mutex> guard(commonMutex);
//    _deleteFile(fileName, folderName);
//}

/// @details folderName can be empty. In this situation, the function saves required file to the program folder.
void backend::FileSystem::saveFile(
        const std::string& transferredString,
        const std::string& fileName,
        const std::string& folderName,
        bool clearAndWrite
) noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    _cleanCharString(_tempPath);
    _attachStringToBasicTempPath(folderName);
    _appendCharString(_tempPath, PATH_MAX, fileName, '/');
    _appendCharString(_tempPath, PATH_MAX, EXTENSION_NAME, EXTENSION_NAME_LENGTH, '.');

    if (transferredString.empty()) return;

    std::ifstream readOnlyFile(_tempPath);
    if (readOnlyFile.is_open()) {

        if (_is_empty(readOnlyFile) || clearAndWrite) {
            readOnlyFile.close();

            /// OUT MODE
            std::ofstream writableFile(_tempPath, std::ios::out);
            writableFile << transferredString << '\n';
            writableFile.close();
            return;
        }
        readOnlyFile.close();

        /// APP MODE
        std::ofstream appendableFile(_tempPath, std::ios::app);
        appendableFile << transferredString << '\n';
        appendableFile.close();
    }
}

/// @details Empty folder name allows to reach files located in the main folder.
void backend::FileSystem::loadFile(
        std::string& loadedString,
        const std::string& fileName,
        const std::string& folderName
) noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    loadedString = {};
    if (!_checkFile(folderName, fileName)) {
        try {
            loadedString = _defaultData(folderName, fileName);
            return;
        }
        catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
            std::terminate();
        }
    }

    _modifyAbsolutePath(folderName, fileName);
    std::ifstream readableFile(_absolutePath);
    if (!readableFile.is_open()) {
        try {
            loadedString = _defaultData(folderName, fileName);
            return;
        }
        catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
        }
    }

    char c;
    while (readableFile.get(c)) {
        loadedString.push_back(c);
    }
    readableFile.close();
    _absolutePath = {};
}

[[nodiscard]] std::string backend::FileSystem::giveTheLatestYearFolder() noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    std::vector<std::string> folderPathList{};
    _fillFileNameList(folderPathList);

    if (folderPathList.empty()) { return yearlyFolderNameStd; }

    _removeExcessFolderPathFromList(folderPathList);

    _removeExtension(folderPathList);

    std::string latest{};
    uint16_t maximum = _giveLatestFileName(latest, folderPathList);

    if (maximum == 0) { return yearlyFolderNameStd; }
    return std::move(latest);
}

[[nodiscard]] std::string backend::FileSystem::giveTheLatestWeekFile(const std::string& folderName) noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    std::vector<std::string> fileNameList{};
    _fillFileNameList(fileNameList, folderName);

    if (fileNameList.empty()) { return weeklyFileNameStd; }

    _removeExcessFolderPathFromList(fileNameList);

    _removeExtension(fileNameList);

    std::string latest{};
    uint16_t maximum = _giveLatestFileName(latest, fileNameList);

    if (maximum == 0) { return weeklyFileNameStd; }
    return std::move(latest);
}

/// @definition non-const latestFolderName parameter is updated after creating a new file.
void backend::FileSystem::createNewYearFolder(std::string& latestFolderName) {

    std::lock_guard<std::mutex> guard(commonMutex);

    uint16_t fileNumber = _getFileNumber(&latestFolderName);
    ++fileNumber;

    std::string tempFileName = _findMatchedSimilarName(latestFolderName);
    tempFileName.push_back('-');
    tempFileName.append(std::to_string(fileNumber));

    _createFolder(tempFileName);
    _createFile(tempFileName, weeklyFileNameStd);
    _createFile(tempFileName, weeklySummaryFileNameStd);
    _createFile(tempFileName, monthlySummaryFileNameStd);
    latestFolderName = tempFileName;
}

/// @definition non-const latestFileName parameter is updated after
/// creating a new file.
void backend::FileSystem::createNewWeekFile(std::string& latestFolderName, std::string& latestFileName) noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    uint16_t fileNumber = _getFileNumber(&latestFileName);
    ++fileNumber;

    if (fileNumber > 52) {
        createNewYearFolder(latestFolderName);
        fileNumber = 1;
    }
    std::string tempFile = _findMatchedSimilarName(latestFileName);
    tempFile.push_back('-');
    tempFile.append(std::to_string(fileNumber));

    _createFile(latestFolderName, tempFile);
    latestFileName = tempFile;
}

void backend::FileSystem::deleteProgramFolder() noexcept {

    std::lock_guard<std::mutex> guard(commonMutex);

    _deleteFile();
    setup();
}

//void backend::FileSystem::deleteAllWeekFiles(const std::string& latestFolderName) noexcept {
//
//    std::vector<std::string> weekFileList;
//    _fillFileNameList(weekFileList, latestFolderName);
//
//    _removeExcessFolderPathFromList(weekFileList);
//
//    _removeExtension(weekFileList);
//
//    auto weekName = "week-";
//    std::string tempFileName = {};
//    std::vector<std::string> tempWeekFileList;
//    auto weekNamesMatched = false;
//    for (const auto& weekFile : weekFileList) {
//
//        if (weekFile.size() < 5) { continue; }
//
//        for (uint8_t i = 0; i < 5; ++i) {
//            tempFileName.push_back(weekFile.at(i));
//        }
//
//        weekNamesMatched = tempFileName == weekName;
//        if (weekNamesMatched) {
//            tempWeekFileList.emplace_back(weekFile);
//        }
//        tempFileName = {};
//    }
//
//    std::for_each(tempWeekFileList.begin(), tempWeekFileList.end(),
//                  [&](const auto& weekFile){
//
//                      _deleteFile(weekFile, latestFolderName);
//                  });
//}
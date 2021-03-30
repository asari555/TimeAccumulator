#include "intermediateSystem.h"

tm* backend::IntermediateSystem::_getDailyInformation() noexcept {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    return localTime;
}

/// for settings operations.
std::unique_ptr<backend::Settings> backend::IntermediateSystem::_loadSettings() noexcept {
    auto settingData = std::make_unique<Settings>();
    msgpack::object_handle handledObject;

    std::string input;
    _loadFileToString(input, settingsFileNameStd, "");

    if (input.empty()) { return nullptr; }

    const char* inputAsCharString = input.c_str();
    std::size_t sizeOfInput = input.size();
    handledObject = msgpack::unpack(inputAsCharString, sizeOfInput);

    msgpack::object obj = handledObject.get();
    auto convertedObject = obj.convert();

    *settingData = convertedObject;

    settingData->textColor = _convertIfMaxValueToZero(settingData->textColor);
    settingData->backgroundColor = _convertIfMaxValueToZero(settingData->backgroundColor);
    settingData->language = _convertIfMaxValueToZero(settingData->language);

    return std::move(settingData);
}

uint8_t backend::IntermediateSystem::_getLatestAbstractYearNumber() noexcept {
    auto folderName = giveTheLatestYearFolder();
    std::string number;
    bool numberUntilNow = false;

    for (const auto& character : folderName) {
        if (numberUntilNow) {
            number.push_back(character);
        }
        if (character == '-') {
            numberUntilNow = true;
        }
    }
    if (number.empty()) { return 0; }

    return std::stoi(number);
}

void backend::IntermediateSystem::_setProgramStartDayAndTime() noexcept {
    auto localTime = _getDailyInformation();
    DayAndTime programStartingPoint;

    programStartingPoint.weekDay = localTime->tm_wday;
    programStartingPoint.day = localTime->tm_mday;
    programStartingPoint.month = MONTH_OFFSET + localTime->tm_mon;
    programStartingPoint.year = YEAR_OFFSET + localTime->tm_year;
    programStartingPoint.abstractYear = _getLatestAbstractYearNumber();

    programStartingPoint.hour = localTime->tm_hour;
    programStartingPoint.minute = localTime->tm_min;
    programStartingPoint.second = localTime->tm_sec;

    _programStartDayAndTime = programStartingPoint;
}

std::unique_ptr<backend::DayAndTime> backend::IntermediateSystem::_getCurrentDailyInformation() noexcept {
    auto localTime = _getDailyInformation();
    DayAndTime currentDailyInformation;

    currentDailyInformation.weekDay = localTime->tm_wday;
    currentDailyInformation.day = localTime->tm_mday;
    currentDailyInformation.month = MONTH_OFFSET + localTime->tm_mon;
    currentDailyInformation.year = YEAR_OFFSET + localTime->tm_year;
    currentDailyInformation.abstractYear = _getLatestAbstractYearNumber();

    currentDailyInformation.hour = localTime->tm_hour;
    currentDailyInformation.minute = localTime->tm_min;
    currentDailyInformation.second = localTime->tm_sec;

    return std::make_unique<DayAndTime>(currentDailyInformation);
}

void backend::IntermediateSystem::_generateStringWithExtension(std::string& pureFileName) noexcept {
    pureFileName.push_back('.');
    pureFileName.append(EXTENSION_NAME);
}

void backend::IntermediateSystem::_loadFileToString(
        std::string& charString,
        const std::string& latestFile,
        const std::string& latestFolder
        ) noexcept {

    std::string tempLatestFile = latestFile;
    _generateStringWithExtension(tempLatestFile);
    loadFile(charString, tempLatestFile, latestFolder);
}

/// For all _load function family
void backend::IntermediateSystem::_listObjects(
        const std::string& latestFile,
        const std::string& latestFolder,
        std::vector<msgpack::object_handle>& handledObjectList,
        const uint16_t& operationId
        ) noexcept {

    std::string input{};
    _loadFileToString(input, latestFile, latestFolder);

    size_t inputSize = input.size();

    char inputCharString[inputSize];
    std::fill(inputCharString, inputCharString + inputSize, '\0');

    uint8_t nextLineCount = 0;
    uint32_t count = 0;
    uint32_t currentIndex;
    uint32_t previousIndex = 0;
    uint8_t operationMode = 0;
    bool operationModeByte = true;
    for (const auto& item : input) {

        currentIndex = &item - &input[0];
        if (operationModeByte) {
            operationMode = item;
            operationModeByte = false;
            continue;
        }

        if (item == '\n') {
            ++nextLineCount;

            if (nextLineCount == 1 || nextLineCount == 2) {
                previousIndex = currentIndex;
                inputCharString[count++] = item;
                continue;
            }

            if (nextLineCount != 3 && currentIndex != previousIndex + 1) {
                inputCharString[count++] = item;
                nextLineCount = 0;
                continue;
            }

            for (nextLineCount = 0; nextLineCount < 2; ++nextLineCount) {
                inputCharString[--count] = '\0';
            }

            if (operationMode != 0 && operationMode == operationId) {
                handledObjectList.emplace_back(msgpack::unpack(inputCharString, count + INDEX_OFFSET_TWO));
            }

            std::fill(inputCharString, inputCharString + count,'\0');
            count = 0;
            nextLineCount = 0;
            operationModeByte = true;
            continue;
        }

        nextLineCount = 0;
        inputCharString[count++] = item;
    }
}

/// for settings functions.
void backend::IntermediateSystem::_saveAsMsgpack(const std::string& fileName, std::unique_ptr<Settings> data) noexcept {

    msgpack::sbuffer buffer;
    msgpack::packer<msgpack::sbuffer> packer(&buffer);
    packer.pack(*data);

    std::stringstream bufferStream;
    bufferStream << buffer.data();

    saveFile(bufferStream.str(), fileName, "", true);
}

/// checks date of given daily data and current date whether are
/// the same.
bool backend::IntermediateSystem::_compareToCurrentDate(const DailyData& dailyData) noexcept {
    auto today = std::move(_getCurrentDailyInformation());
    return today->day == dailyData.day && today->month == dailyData.month && today->year == dailyData.year;
}

/// @parameters dailyDataList and lastDailyData are common parameters
/// @parameters remainingSeconds is required by summary
/// @parameters _totalMinutes is modified to have accumulated time
/// @parameters summaryOrNot
/// @return total session number
uint16_t backend::IntermediateSystem::_calculateAccumulatedTime(
        std::unique_ptr<std::vector<DailyData>> dailyDataList,
        const DailyData& lastDailyData,
        int8_t& remainingSeconds,
        int16_t& totalMinutes,
        bool summary
        ) noexcept {

    uint8_t beginHour = 0, beginMinute = 0, beginSecond = 0;
    uint8_t endHour, endMinute, endSecond;
    uint16_t totalSessionNumber = 0;
    totalMinutes = 0;

    bool theDayAndLastDayIsTheSame;
    if (!summary) {
        auto today = std::move(_getCurrentDailyInformation());

        theDayAndLastDayIsTheSame = _compareToCurrentDate(lastDailyData);

        if (!theDayAndLastDayIsTheSame) { return 0; }
    }

    for (const auto& dailyData : (*dailyDataList)) {

        theDayAndLastDayIsTheSame =
                dailyData.day == lastDailyData.day &&
                dailyData.month == lastDailyData.month &&
                dailyData.year == lastDailyData.year;

        if (!theDayAndLastDayIsTheSame) { continue; }

        if (dailyData.modificationMode & BEGIN_MODE) {
            beginHour = dailyData.hour;
            beginMinute = dailyData.minute;
            beginSecond = dailyData.second;
            ++totalSessionNumber;
        }

        if (dailyData.modificationMode & END_MODE) {
            endHour = dailyData.hour;
            endMinute = dailyData.minute;
            endSecond = dailyData.second;
            totalMinutes += ((endHour - beginHour) * AN_HOUR_AS_MINUTES);
            totalMinutes += (endMinute - beginMinute);
            remainingSeconds += (endSecond - beginSecond);
            if (remainingSeconds < 0) {
                --totalMinutes;
                remainingSeconds += A_MINUTE_AS_SECONDS;
            }
            if (remainingSeconds >= A_MINUTE_AS_SECONDS) {
                remainingSeconds -= A_MINUTE_AS_SECONDS;
                ++totalMinutes;
            }
            beginHour = 0, beginMinute = 0, beginSecond = 0;
        }

        if (dailyData.modificationMode & ADD_MODE) {
            totalMinutes += dailyData.totalMinute;
            ++totalSessionNumber;
        }

        if (dailyData.modificationMode & DELETE_MODE) {

            const auto tempDailyDataMinute = dailyData.totalMinute;
            if (tempDailyDataMinute >= totalMinutes) {
                totalMinutes = 0;
                continue;
            }

            totalMinutes -= tempDailyDataMinute;
        }
    }

    return totalSessionNumber;
}

void backend::IntermediateSystem::_pushBackToDailyReportContainer(
        uint8_t mode,
        const DailyData& data,
        std::vector<DailyReportInterface>& container
        ) const noexcept {

    DailyReportInterface tempDailyReport;
    tempDailyReport.commandCode = mode;
    tempDailyReport.hour = data.hour;
    tempDailyReport.minute = data.minute;
    tempDailyReport.second = data.second;
    tempDailyReport.totalMinute = data.totalMinute;

    tempDailyReport.remainingSecond = mode == BEGIN_MODE ? data.remainingSeconds : 0;
    container.push_back(tempDailyReport);
}

bool backend::IntermediateSystem::_dataBelongsToLastDay(
        const DailyData& data,
        std::unique_ptr<DayAndTime>& now
        ) noexcept {

    return data.day == now->day && data.month == now->month && data.year == now->year;
}

/// calculates differences between daily data containing end and begin
/// modes in terms of time as minutes.
backend::CalculatedTime backend::IntermediateSystem::_calculateTime(
        const DailyData& dailyData,
        const DailyData& previousDailyData
        ) noexcept {

    int8_t remainingSecond = 0;
    uint16_t totalMinute = 0;

    totalMinute += (dailyData.hour - previousDailyData.hour) * AN_HOUR_AS_MINUTES;
    totalMinute += dailyData.minute - previousDailyData.minute;
    remainingSecond = dailyData.second - previousDailyData.second;

    if (remainingSecond < 0) {
        --totalMinute;
        remainingSecond += A_MINUTE_AS_SECONDS;
    }

    CalculatedTime calculatedTime;
    calculatedTime.minutes = totalMinute;
    calculatedTime.seconds = remainingSecond;

    return calculatedTime;
}

uint8_t backend::IntermediateSystem::_getFileNumber(const std::string& fileName) noexcept {

    std::string tempString = {};
    bool charIsNumber = false;
    if (fileName == weeklyFileNameStd || fileName == yearlyFolderNameStd) { return 0; }

    for (const auto& character : fileName) {

        if (charIsNumber) { tempString.push_back(character); }

        if (character == '-') { charIsNumber = true; }
    }

    try {
        return std::stoi(tempString);
    }
    catch(const std::exception& exception) {
        std::cerr << "Unable to convert number. Error : " << exception.what() << std::endl;
        std::terminate();
    }
}

std::unique_ptr<backend::DayAndTime> backend::IntermediateSystem::getProgramStartDayAndTime() noexcept {
    // any change in language changes weekDay
    return std::make_unique<backend::DayAndTime>(_programStartDayAndTime);
}

uint8_t backend::IntermediateSystem::getProgramStartDay() noexcept {
    return _programStartDayAndTime.weekDay;
}

std::unique_ptr<std::vector<backend::DailyData>> backend::IntermediateSystem::loadDailyData(
        const std::string& folderName,
        const std::string& fileName
        ) noexcept {

    auto dailyDataList = std::make_unique<std::vector<DailyData>>();
    std::vector<msgpack::object_handle> handledObjects;

    _listObjects(fileName, folderName, handledObjects, DAILY_DATA_ID);

    dailyDataList = _combOutRequiredData(std::move(dailyDataList), handledObjects);

    if (dailyDataList->empty()) { return dailyDataList; }

    for (auto& dailyData : *dailyDataList) {
        dailyData.hour = _convertIfMaxValueToZero(dailyData.hour);
        dailyData.minute = _convertIfMaxValueToZero(dailyData.minute);
        dailyData.second = _convertIfMaxValueToZero(dailyData.second);
        dailyData.totalMinute = _convertIfMaxValueToZero(dailyData.totalMinute);
        dailyData.remainingSeconds = _convertIfMaxValueToZero(dailyData.remainingSeconds);
        dailyData.weekDay = _convertIfMaxValueToZero(dailyData.weekDay);
    }

    return dailyDataList;
}

bool backend::IntermediateSystem::saveDailyData(
        const std::string& folderName,
        const std::string& fileName,
        const uint8_t command,
        uint16_t addableOrDeletableMinutes
        ) noexcept {

    if (folderName == yearlyFolderNameStd) { return false; }
    if (fileName == weeklyFileNameStd) { return false; }

    auto dailyInformation = std::move(_getCurrentDailyInformation());
    DailyData dailyData;
    dailyData.modificationMode = command;
    dailyData.day = dailyInformation->day;
    dailyData.month = dailyInformation->month;
    dailyData.year = dailyInformation->year;
    dailyData.abstractYear = dailyInformation->abstractYear;

    auto dailyDataList = loadDailyData(folderName, fileName);

    if (!dailyDataList->empty()) {
        auto lastDailyData = dailyDataList->back();
        auto lastModificationMode = lastDailyData.modificationMode;
        if (lastModificationMode == BEGIN_MODE && command != END_MODE) { return false; }
        if (lastModificationMode == END_MODE && command == END_MODE) { return false; }
    }
    else if (command == END_MODE) { return false; }

    if (command & ADD_MODE || command & DELETE_MODE) {
        dailyData.totalMinute = _convertIfZeroToMaxValue(addableOrDeletableMinutes);
    }
    else { dailyData.totalMinute = UINT16_MAX; }

    dailyData.hour = _convertIfZeroToMaxValue(dailyInformation->hour);
    dailyData.minute = _convertIfZeroToMaxValue(dailyInformation->minute);
    dailyData.second = _convertIfZeroToMaxValue(dailyInformation->second);
    dailyData.second = _convertIfZeroToMaxValue(dailyInformation->second);
    dailyData.weekDay = _convertIfZeroToMaxValue(dailyInformation->weekDay);
    dailyData.remainingSeconds = UINT8_MAX;

    _saveAsMsgpack(folderName, fileName, dailyData);

    return true;
}

void backend::IntermediateSystem::saveDailyDataWithEndCommand(
        const std::string& folderName,
        const std::string& fileName,
        const std::unique_ptr<DayAndTime> dailyInformation
        ) noexcept {

    if (folderName == yearlyFolderNameStd) { return; }
    if (fileName == weeklyFileNameStd) { return; }

    DailyData dailyData;
    dailyData.modificationMode = END_MODE;
    dailyData.day = dailyInformation->day;
    dailyData.month = dailyInformation->month;
    dailyData.year = dailyInformation->year;
    dailyData.abstractYear = dailyInformation->abstractYear;
    dailyData.hour = _convertIfZeroToMaxValue(dailyInformation->hour);
    dailyData.minute = _convertIfZeroToMaxValue(dailyInformation->minute);
    dailyData.second = _convertIfZeroToMaxValue(dailyInformation->second);
    dailyData.weekDay = _convertIfZeroToMaxValue(dailyInformation->weekDay);
    dailyData.totalMinute = UINT16_MAX;
    dailyData.remainingSeconds = UINT8_MAX;

    _saveAsMsgpack(folderName, fileName, dailyData);
}

void backend::IntermediateSystem::saveMaintainerDailyDataOfTodayWithCommand(
        const std::string& folderName,
        const std::string& fileName,
        const uint8_t command,
        const uint8_t hour,
        const uint8_t minute
        ) noexcept {

    if (folderName == yearlyFolderNameStd) { return; }
    if (fileName == weeklyFileNameStd) { return; }

    auto dailyInformation = std::move(_getCurrentDailyInformation());

    DailyData dailyData;
    dailyData.modificationMode = command;
    dailyData.day = dailyInformation->day;
    dailyData.month = dailyInformation->month;
    dailyData.year = dailyInformation->year;
    dailyData.abstractYear = dailyInformation->abstractYear;
    dailyData.hour = _convertIfZeroToMaxValue(hour);
    dailyData.minute = _convertIfZeroToMaxValue(minute);
    dailyData.second = UINT8_MAX;
    dailyData.weekDay = _convertIfZeroToMaxValue(dailyInformation->weekDay);
    dailyData.totalMinute = UINT16_MAX;
    dailyData.remainingSeconds = UINT8_MAX;

    _saveAsMsgpack(folderName, fileName, dailyData);
}

void backend::IntermediateSystem::saveDailyDataWithDeleteCommand(
        const std::string& folderName,
        const std::string& fileName,
        const uint16_t minute,
        const std::unique_ptr<DayAndTime> dailyInformation
) noexcept {

    if (folderName == yearlyFolderNameStd) { return; }
    if (fileName == weeklyFileNameStd) { return; }

    DailyData dailyData;
    dailyData.modificationMode = DELETE_MODE;
    dailyData.day = dailyInformation->day;
    dailyData.month = dailyInformation->month;
    dailyData.year = dailyInformation->year;
    dailyData.abstractYear = dailyInformation->abstractYear;
    dailyData.hour = 23;
    dailyData.minute = 59;
    dailyData.second = 59;
    dailyData.weekDay = _convertIfZeroToMaxValue(dailyInformation->weekDay);
    dailyData.totalMinute = _convertIfZeroToMaxValue(minute);;
    dailyData.remainingSeconds = UINT8_MAX;

    _saveAsMsgpack(folderName, fileName, dailyData);
}

std::unique_ptr<std::vector<backend::DailySummaryData>> backend::IntermediateSystem::loadDailySummaryData(
        const std::string& folderName,
        const std::string& fileName
        ) noexcept {

    auto dailySummaryDataList = std::make_unique<std::vector<DailySummaryData>>();
    std::vector<msgpack::object_handle> handledObjects;

    _listObjects(fileName, folderName, handledObjects, DAILY_SUMMARY_ID);

    dailySummaryDataList = _combOutRequiredData(std::move(dailySummaryDataList), handledObjects);

    if (dailySummaryDataList->empty()) { std::move(dailySummaryDataList); }

    for (auto& dailyDataSummary : *dailySummaryDataList) {
        dailyDataSummary.dailyTotalMinutes = _convertIfMaxValueToZero(dailyDataSummary.dailyTotalMinutes);
        dailyDataSummary.dailyRemainingSeconds = _convertIfMaxValueToZero(dailyDataSummary.dailyRemainingSeconds);
        dailyDataSummary.weekDay = _convertIfMaxValueToZero(dailyDataSummary.weekDay);
    }

    return std::move(dailySummaryDataList);
}

bool backend::IntermediateSystem::saveDailySummaryData(
        const std::string& folderName,
        const std::string& fileName
        ) noexcept {

    if (folderName == yearlyFolderNameStd) { return false; }
    if (fileName == weeklyFileNameStd) { return false; }

    auto dailyDataList = loadDailyData(folderName, fileName);
    auto dailySummaryList = loadDailySummaryData(folderName, fileName);

    auto now = _getCurrentDailyInformation();
    uint8_t today = now->day, thisMonth = now->month;
    uint16_t thisYear = now->year;

    int8_t remainingSeconds = 0;
    int16_t totalMinutes = 0;
    uint16_t totalSessionNumber = 0;

    bool thereIsDailySummary = !dailySummaryList->empty();

    /// back() function causes segmentation fault in case of
    /// empty vector. "if" block is used here avoiding the failure.
    if (thereIsDailySummary) {
        auto lastDailySummaryData = dailySummaryList->back();
        bool multipleControls =
                lastDailySummaryData.day == today &&
                lastDailySummaryData.month == thisMonth &&
                lastDailySummaryData.year == thisYear;

        if (multipleControls) { return false; }
    }

    backend::DailyData theLastDailyData;
    bool precedingDataAndSummaryDateControl = false;
    bool thereIsDailyData = !dailyDataList->empty();

    /// back() function causes segmentation fault in case of
    /// empty vector. "if" block is used here avoiding the failure.
    if (!thereIsDailyData) { return false; }
    theLastDailyData = dailyDataList->back();

    /// Check the last command "begin".
    bool lastDayIsToday =
            theLastDailyData.day == today &&
            theLastDailyData.month == thisMonth &&
            theLastDailyData.year == thisYear;

    if (theLastDailyData.modificationMode & BEGIN_MODE) {
        if (lastDayIsToday) {
            saveDailyData(folderName, fileName, END_MODE);
        }
        else { return false; }
    }

    if (thereIsDailySummary) {
        auto theLastDailyDataSummary = dailySummaryList->back();
        precedingDataAndSummaryDateControl =
                theLastDailyData.day == theLastDailyDataSummary.day &&
                theLastDailyData.month == theLastDailyDataSummary.month &&
                theLastDailyData.year == theLastDailyDataSummary.year;
    }

    if (precedingDataAndSummaryDateControl) { return false; }

    totalSessionNumber = _calculateAccumulatedTime(
                                std::move(dailyDataList),
                                theLastDailyData,
                                remainingSeconds,
                                totalMinutes,
                                true
                                );

    DailySummaryData dailySummaryData;

    dailySummaryData.universalMode = DAILY_SUMMARY_ID;
    dailySummaryData.day = theLastDailyData.day;
    dailySummaryData.month = theLastDailyData.month;
    dailySummaryData.year = theLastDailyData.year;
    dailySummaryData.abstractYear = theLastDailyData.abstractYear;

    /// To avoid msgpack::v1::type_error each zero value has to be
    /// converted UINT32_MAX and UINT16_MAX at saving stage.
    dailySummaryData.dailyTotalSessionNumber = _convertIfZeroToMaxValue(totalSessionNumber);
    dailySummaryData.dailyTotalMinutes = _convertIfZeroToMaxValue(totalMinutes);
    dailySummaryData.dailyRemainingSeconds = UINT16_MAX;

    dailySummaryData.weekDay = _convertIfZeroToMaxValue(theLastDailyData.weekDay);

    _saveAsMsgpack(folderName, fileName, dailySummaryData);

    return true;
}

std::unique_ptr<std::vector<backend::WeeklySummaryData>> backend::IntermediateSystem::loadWeeklySummaryData(
        const std::string& folderName
        ) noexcept {

    auto weeklySummaryDataList = std::make_unique<std::vector<WeeklySummaryData>>();
    std::vector<msgpack::object_handle> handledObjects;

    _listObjects(weeklySummaryFileNameStd, folderName, handledObjects, WEEKLY_SUMMARY_DATA_ID);

    weeklySummaryDataList = _combOutRequiredData(std::move(weeklySummaryDataList), handledObjects);

    bool weeklySummaryDataListIsEmpty = weeklySummaryDataList->empty();

    if (weeklySummaryDataListIsEmpty) { return weeklySummaryDataList; }

    for (auto& weeklySummaryData : *weeklySummaryDataList) {
        weeklySummaryData.weeklyTotalMinutes = _convertIfMaxValueToZero(weeklySummaryData.weeklyTotalMinutes);
    }

    return weeklySummaryDataList;
}

bool backend::IntermediateSystem::saveWeeklySummaryData(
        const std::string& folderName,
        const std::string& fileName
        ) noexcept {

    if (folderName == yearlyFolderNameStd) { return false; }
    if (fileName == weeklySummaryFileNameStd) { return false; }

    auto dailySummaryDataList = loadDailySummaryData(folderName, fileName);
    auto weeklySummaryDataList = loadWeeklySummaryData(folderName);

    if (dailySummaryDataList->empty()) { return false; }
    if (dailySummaryDataList->size() != 7) { return false; }

    auto firstDailySummaryData = dailySummaryDataList->front();
    auto lastDailySummaryData = dailySummaryDataList->back();

    if (!weeklySummaryDataList->empty()) {
        WeeklySummaryData lastWeeklySummaryData = weeklySummaryDataList->back();

        bool multipleControls =
                lastWeeklySummaryData.lastDay == lastDailySummaryData.day &&
                lastWeeklySummaryData.lastMonth == lastDailySummaryData.month &&
                lastWeeklySummaryData.lastYear == lastDailySummaryData.year;

        if (multipleControls) { return false; }
    }

    uint16_t weeklyTotalMinutes = 0;
    uint32_t weeklyTotalSessionNumber = 0;
    for (auto& dailySummaryData : *dailySummaryDataList) {
        weeklyTotalMinutes += _convertIfMaxValueToZero(dailySummaryData.dailyTotalMinutes);
        weeklyTotalSessionNumber += _convertIfMaxValueToZero(dailySummaryData.dailyTotalSessionNumber);
    }

    weeklyTotalMinutes = _convertIfZeroToMaxValue(weeklyTotalMinutes);
    weeklyTotalSessionNumber = _convertIfZeroToMaxValue(weeklyTotalSessionNumber);

    WeeklySummaryData weeklySummaryData;
    weeklySummaryData.firstDay = firstDailySummaryData.day;
    weeklySummaryData.lastDay = lastDailySummaryData.day;
    weeklySummaryData.weeklyTotalMinutes = weeklyTotalMinutes;
    weeklySummaryData.weeklyTotalSessionNumber = weeklyTotalSessionNumber;

    // Year, firstMonth and weekName are specific for record time of
    // the weekly summary
    weeklySummaryData.firstYear = firstDailySummaryData.year;
    weeklySummaryData.lastYear = lastDailySummaryData.year;
    weeklySummaryData.abstractYear = _getLatestAbstractYearNumber();
    weeklySummaryData.firstMonth = firstDailySummaryData.month;
    weeklySummaryData.lastMonth = lastDailySummaryData.month;
    weeklySummaryData.weekName = fileName;

    _saveAsMsgpack(folderName, weeklySummaryFileNameStd, weeklySummaryData);

    return true;
}

std::unique_ptr<std::vector<backend::MonthlySummaryData>> backend::IntermediateSystem::loadMonthlySummaryData(
        const std::string& folderName
        ) noexcept {

    auto monthlySummaryDataList = std::make_unique<std::vector<backend::MonthlySummaryData>>();
    std::vector<msgpack::object_handle> handledObjects;

    _listObjects(monthlySummaryFileNameStd, folderName, handledObjects, MONTHLY_SUMMARY_DATA_ID);

    monthlySummaryDataList = _combOutRequiredData(std::move(monthlySummaryDataList), handledObjects);

    if (!monthlySummaryDataList->empty()) {
        for (auto& monthlySummaryData : *monthlySummaryDataList) {
            monthlySummaryData.monthlyTotalMinutes = _convertIfMaxValueToZero(monthlySummaryData.monthlyTotalMinutes);
        }
    }

    return monthlySummaryDataList;
}

bool backend::IntermediateSystem::saveMonthlySummaryData(const std::string& folderName, const std::string& fileName) {

    if (folderName == yearlyFolderNameStd) { return false; }
    if (fileName == monthlySummaryFileNameStd) { return false; }

    auto weeklySummaryDataList = loadWeeklySummaryData(folderName);
    auto monthlySummaryDataList = loadMonthlySummaryData(folderName);

    if (monthlySummaryDataList->size() >= 12) { return false; }
    if (weeklySummaryDataList->empty()) { return false; }
    if (weeklySummaryDataList->size() < WEEK_NUMBER_IN_A_MONTH) { return false; }

    uint8_t indexOfLastData = weeklySummaryDataList->size() - 1;
    WeeklySummaryData lastWeeklySummaryData = weeklySummaryDataList->back();

    auto actualIndexOfLastData = indexOfLastData - RANGE_BETWEEN_FIRST_DATA;
    WeeklySummaryData firstWeeklySummaryData = weeklySummaryDataList->operator[](actualIndexOfLastData);

    MonthlySummaryData monthlySummaryData;

    if (monthlySummaryDataList->empty()) {
        monthlySummaryData.monthNumber = 1;
    }
    else {
        MonthlySummaryData lastMonthlySummaryData = monthlySummaryDataList->back();

        bool multipleControls =
                lastMonthlySummaryData.lastWeek == lastWeeklySummaryData.weekName &&
                lastMonthlySummaryData.firstWeek == firstWeeklySummaryData.weekName &&
                lastMonthlySummaryData.year == lastWeeklySummaryData.lastYear;

        if (multipleControls) { return false; }

        uint8_t monthNumber = lastMonthlySummaryData.monthNumber;
        if (monthNumber > MONTH_NUMBER_IN_A_YEAR) {
            throw std::logic_error("Unable To Make Monthly Summary, Over The Limit");
        }

        monthlySummaryData.monthNumber = monthNumber + 1;
    }

    uint8_t weekNumber = 1;

    uint16_t tempTotalMinutes = 0;
    uint16_t tempTotalSessionNumber = 0;
    for (auto& weeklySummaryData : *weeklySummaryDataList) {
        if (weekNumber == FIRST_WEEK) {
            monthlySummaryData.firstWeek = weeklySummaryData.weekName;
        }
        if (weekNumber == LAST_WEEK) {
            monthlySummaryData.lastWeek = weeklySummaryData.weekName;
        }

        tempTotalMinutes += _convertIfMaxValueToZero(weeklySummaryData.weeklyTotalMinutes);

        tempTotalSessionNumber += _convertIfMaxValueToZero(weeklySummaryData.weeklyTotalSessionNumber);

        ++weekNumber;
    }

    monthlySummaryData.year = weeklySummaryDataList->back().lastYear;
    monthlySummaryData.abstractYear = _getLatestAbstractYearNumber();

    monthlySummaryData.monthlyTotalMinutes = _convertIfZeroToMaxValue(tempTotalMinutes);
    monthlySummaryData.monthlyTotalSessionNumber = _convertIfZeroToMaxValue(tempTotalSessionNumber);

    _saveAsMsgpack(folderName, monthlySummaryFileNameStd, monthlySummaryData);

    return true;
}

std::unique_ptr<std::vector<backend::YearlySummaryData>> backend::IntermediateSystem::loadYearlySummaryData() noexcept {

    auto yearlySummaryDataList = std::make_unique<std::vector<backend::YearlySummaryData>>();
    std::vector<msgpack::object_handle> handledObjects;

    _listObjects(yearlySummaryFileNameStd, "", handledObjects, YEARLY_SUMMARY_DATA_ID);

    yearlySummaryDataList = _combOutRequiredData(std::move(yearlySummaryDataList), handledObjects);

    bool yearlySummaryDataListIsNotEmpty = !yearlySummaryDataList->empty();

    if (yearlySummaryDataListIsNotEmpty) {

        uint32_t tempTotalMinutes = 0;
        uint32_t tempTotalSession = 0;
        for (auto& yearlySummaryData : *yearlySummaryDataList) {

            tempTotalMinutes = _convertIfMaxValueToZero(yearlySummaryData.yearlyTotalMinutes);
            tempTotalSession = _convertIfMaxValueToZero(yearlySummaryData.yearlyTotalSessionNumber);

            yearlySummaryData.yearlyTotalMinutes = tempTotalMinutes;
            yearlySummaryData.yearlyTotalSessionNumber = tempTotalSession;
        }
    }

    return yearlySummaryDataList;
}

bool backend::IntermediateSystem::saveYearlySummaryData(const std::string& folderName) noexcept {

    if (folderName == yearlyFolderNameStd) { return false; }

    auto monthlySummaryDataList = loadMonthlySummaryData(folderName);
    auto yearlySummaryDataList = loadYearlySummaryData();

    uint16_t monthlySummaryDataListSize = monthlySummaryDataList->size();

    if (monthlySummaryDataList->empty()) { return false; }
    if (monthlySummaryDataListSize != MONTH_NUMBER_IN_A_YEAR) { return false; }

    auto firstMonthlySummaryData = monthlySummaryDataList->front();
    auto lastMonthlySummaryData = monthlySummaryDataList->back();

    if (!yearlySummaryDataList->empty()) {
        auto lastYearlySummaryData = yearlySummaryDataList->back();

        bool multipleControls =
                lastYearlySummaryData.imaginaryYear ==folderName &&
                lastYearlySummaryData.year == lastMonthlySummaryData.year;

        if (multipleControls) { return false; }
    }

    YearlySummaryData yearlySummaryData;
    uint32_t tempTotalMinutes = 0;
    uint32_t tempTotalSessionNumber = 0;

    for (auto &monthlySummaryData : *monthlySummaryDataList) {
        tempTotalMinutes += _convertIfMaxValueToZero(monthlySummaryData.monthlyTotalMinutes);
        tempTotalSessionNumber += _convertIfMaxValueToZero(monthlySummaryData.monthlyTotalSessionNumber);
    }

    yearlySummaryData.year = lastMonthlySummaryData.year;
    yearlySummaryData.abstractYear = _getLatestAbstractYearNumber();
    yearlySummaryData.imaginaryYear = folderName;

    yearlySummaryData.yearlyTotalMinutes = _convertIfZeroToMaxValue(tempTotalMinutes);
    yearlySummaryData.yearlyTotalSessionNumber = _convertIfZeroToMaxValue(tempTotalSessionNumber);

    _saveAsMsgpack("", yearlySummaryFileNameStd, yearlySummaryData);

    return true;
}

/// for settings operations.
bool backend::IntermediateSystem::isSettingsFileEmpty() noexcept {
    std::string input;
    _loadFileToString(input, settingsFileNameStd, "");
    if (input.empty()) { return true; }
    return false;
}

/// for settings operations.
std::unique_ptr<backend::Settings> backend::IntermediateSystem::loadSettings() noexcept {
    return _loadSettings();
}

/// for settings operations.
void backend::IntermediateSystem::saveSettings(const uint8_t& language) noexcept {

    std::unique_ptr<Settings> loadedData = loadSettings();

    if(loadedData == nullptr) {
        loadedData = std::make_unique<Settings>();
    }

    loadedData->textColor = _convertIfZeroToMaxValue(loadedData->textColor);
    loadedData->backgroundColor = _convertIfZeroToMaxValue(loadedData->backgroundColor);
    loadedData->language = _convertIfZeroToMaxValue(language);

    _saveAsMsgpack(settingsFileNameStd, std::move(loadedData));
}

/// for settings operations.
void backend::IntermediateSystem::saveSettings(
        const uint16_t& textColor,
        const uint16_t& backgroundColor
        ) noexcept {

    std::unique_ptr<Settings> loadedData = loadSettings();

    if (loadedData == nullptr) {
        loadedData = std::make_unique<Settings>();
    }

    loadedData->textColor = _convertIfZeroToMaxValue(textColor);
    loadedData->backgroundColor = _convertIfZeroToMaxValue(backgroundColor);
    loadedData->language = _convertIfZeroToMaxValue(loadedData->language);

    _saveAsMsgpack(settingsFileNameStd, std::move(loadedData));
}

/// for settings operations.
/// @details Initializer function in the case of emtpy settings file.
void backend::IntermediateSystem::saveSettings(
        const uint16_t& textColor,
        const uint16_t& backgroundColor,
        const uint8_t& language
        ) noexcept {

    auto initialData = std::make_unique<Settings>();

    initialData->textColor = _convertIfZeroToMaxValue(textColor);
    initialData->backgroundColor = _convertIfZeroToMaxValue(backgroundColor);
    initialData->language = _convertIfZeroToMaxValue(language);

    _saveAsMsgpack(settingsFileNameStd, std::move(initialData));
}

bool backend::IntermediateSystem::lastDailyDataIsBegin() noexcept {

    std::string latestYearFolder = giveTheLatestYearFolder();
    std::string latestWeekFile = giveTheLatestWeekFile(latestYearFolder);

    auto dailyDataList = loadDailyData(latestYearFolder, latestWeekFile);
    auto dailySummaryDataList = loadDailySummaryData(latestYearFolder, latestWeekFile);

    if (dailyDataList->empty()) { return false; }

    auto lastDailyData = dailyDataList->back();
    if (!dailySummaryDataList->empty()) {
        auto lastDailySummaryData = dailySummaryDataList->back();
        bool multipleControls =
                lastDailyData.day == lastDailySummaryData.day &&
                lastDailyData.month == lastDailySummaryData.month &&
                lastDailyData.year == lastDailySummaryData.year;

        if (multipleControls) { return false; }
    }

    return lastDailyData.modificationMode == BEGIN_MODE;
}

/// organises what is left from previous days, weeks and months.
void backend::IntermediateSystem::tidyUpLastDayData(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto currentDailyInformation = _getCurrentDailyInformation();
    auto dailyDataList = loadDailyData(lastFolderName, lastFileName);

    if (dailyDataList->empty()) { return; }

    auto lastDailyData = dailyDataList->back();
    if (lastDailyData.modificationMode == BEGIN_MODE) { return; }

    /// Date of the last daily data is controlled here for
    /// understanding whether last daily data is past or not.
    bool multipleControls =
            lastDailyData.year == currentDailyInformation->year &&
            lastDailyData.month == currentDailyInformation->month &&
            lastDailyData.day == currentDailyInformation->day;

    /// Date of daily data is the same with the current date.
    if (multipleControls) { return; }

    saveDailySummaryData(lastFolderName, lastFileName);
}

/// controls and if possible makes a monthly summary.
void backend::IntermediateSystem::tidyUpMonthlySummaries(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto weeklySummaries = loadWeeklySummaryData(lastFolderName);
    auto monthlySummaries = loadMonthlySummaryData(lastFolderName);
    if (weeklySummaries == nullptr) { return; }
    if (weeklySummaries->empty()) { return; }

    auto numberOfWeeklySummaries = weeklySummaries->size();
    if (numberOfWeeklySummaries < WEEK_NUMBER_IN_A_MONTH) { return; }
    if (monthlySummaries == nullptr) {
        saveMonthlySummaryData(lastFolderName, lastFileName);
    }
    else if (monthlySummaries->empty()) {
        saveMonthlySummaryData(lastFolderName, lastFileName);
    }

    auto numberOfMonthlySummaries = monthlySummaries->size();
    if (numberOfMonthlySummaries == MONTH_NUMBER_IN_A_YEAR) { return; }
    auto requiredMonthlySummaryNumber = numberOfWeeklySummaries / WEEK_NUMBER_IN_A_MONTH;

    if (numberOfMonthlySummaries == requiredMonthlySummaryNumber) { return; }

    saveMonthlySummaryData(lastFolderName, lastFileName);
}

bool backend::IntermediateSystem::lastDailyDataDateIsToday(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto dailyDataList = loadDailyData(lastFolderName, lastFileName);

    if (dailyDataList->empty()) { return false; }

    auto lastDailyData = dailyDataList->back();
    auto currentDate = _getCurrentDailyInformation();

    bool datesAreTheSame =
            lastDailyData.day == currentDate->day &&
            lastDailyData.month == currentDate->month &&
            lastDailyData.year == currentDate->year;

    return datesAreTheSame;
}

bool backend::IntermediateSystem::lastDailySummaryDataDateIsToday(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto dailySummaryDataList = loadDailySummaryData(lastFolderName, lastFileName);

    if (dailySummaryDataList->empty()) { return false; }

    auto lastDailySummaryData = dailySummaryDataList->back();
    auto currentDate = _getCurrentDailyInformation();

    bool datesAreTheSame =
            lastDailySummaryData.day == currentDate->day &&
            lastDailySummaryData.month == currentDate->month &&
            lastDailySummaryData.year == currentDate->year;

    return datesAreTheSame;
}

/// @details nullptr is returned in case of absence of no daily data.
std::unique_ptr<backend::DailySummaryData> backend::IntermediateSystem::getLastDailySummary(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto dailySummaryList = loadDailySummaryData(lastFolderName, lastFileName);

    if (!dailySummaryList->empty()) {
        auto lastDailySummary = dailySummaryList->back();
        return std::make_unique<DailySummaryData>(lastDailySummary);
    }

    auto lastFolderNumber = _getFileNumber(lastFolderName);
    auto lastFileNumber = _getFileNumber(lastFileName);

    std::string previousFolderName = {};
    std::vector<std::string> fileList;

    std::string tempString;
    if (lastFolderNumber != 0) {

        uint8_t previousFolderNumber = lastFolderNumber - 1;
        if (previousFolderNumber != 0) {
            previousFolderName = generateYearFolderName(previousFolderNumber);
        }
    }

    if (lastFileNumber != 0) {

        for (int i = 1; i < lastFileNumber; ++i) {
            tempString = generateWeekFileName(i);
            fileList.push_back(tempString);
        }
    }

    std::string latestFileName;
    if (fileList.empty()) {

        if (previousFolderName.empty()) { return nullptr; }

        latestFileName = giveTheLatestWeekFile(previousFolderName);
        if (latestFileName == weeklyFileNameStd) { return nullptr; }

        return getLastDailySummary(previousFolderName, latestFileName);
    }

    std::reverse(fileList.begin(), fileList.end());
    for (const auto& fileName : fileList) {
        dailySummaryList = loadDailySummaryData(lastFolderName, fileName);

        if (!dailySummaryList->empty()) {
            auto lastDailySummary = dailySummaryList->back();
            return std::make_unique<DailySummaryData>(lastDailySummary);
        }
    }

    if (previousFolderName.empty()) { return nullptr; }
    latestFileName = giveTheLatestWeekFile(previousFolderName);
    if (latestFileName == weeklyFileNameStd) { return nullptr; }
    return getLastDailySummary(previousFolderName, latestFileName);

    return nullptr;
}

/// @details nullptr is returned in case of empty weekly summary data
/// list.
std::unique_ptr<backend::WeeklySummaryData> backend::IntermediateSystem::getLastWeeklySummaryData(
        const std::string& lastFolderName
        ) noexcept {

    auto lastFolderNumber = _getFileNumber(lastFolderName);
    if (lastFolderNumber == 0) { return nullptr; }

    auto weeklySummaryDataList = loadWeeklySummaryData(lastFolderName);
    WeeklySummaryData lastWeeklySummaryData;

    if (!weeklySummaryDataList->empty()) {
        lastWeeklySummaryData = weeklySummaryDataList->back();
        return std::make_unique<backend::WeeklySummaryData>(lastWeeklySummaryData);
    }

    std::vector<std::string> folderNameList;
    {
        std::string tempString;
        for (int i = 1; i < lastFolderNumber; ++i) {
            tempString = generateYearFolderName(i);
            folderNameList.push_back(tempString);
        }
    }

    if (folderNameList.empty()) { return nullptr; }

    std::reverse(folderNameList.begin(), folderNameList.end());
    for (const auto& folderName : folderNameList) {
        weeklySummaryDataList = loadWeeklySummaryData(folderName);

        if (!weeklySummaryDataList->empty()) {
            lastWeeklySummaryData = weeklySummaryDataList->back();
            return std::make_unique<WeeklySummaryData>(lastWeeklySummaryData);
        }
    }

    return nullptr;
}

/// @details nullptr is returned in case of empty monthly summary data
/// list.
std::unique_ptr<backend::MonthlySummaryData> backend::IntermediateSystem::getLastMonthlySummaryData(
        const std::string& lastFolderName
        ) noexcept {

    auto lastFolderNumber = _getFileNumber(lastFolderName);
    if (lastFolderNumber == 0) { return nullptr; }

    auto monthlySummaryDataList = std::move(loadMonthlySummaryData(lastFolderName));
    MonthlySummaryData lastMonthlySummaryData;

    if (!monthlySummaryDataList->empty()) {
        lastMonthlySummaryData = monthlySummaryDataList->back();
        return std::make_unique<MonthlySummaryData>(lastMonthlySummaryData);
    }

    monthlySummaryDataList = nullptr;

    std::vector<std::string> folderNameList;
    {
        std::string tempString;
        for (int i = 1; i < lastFolderNumber; ++i) {
            tempString = generateYearFolderName(i);
            folderNameList.push_back(tempString);
        }
    }

    if (folderNameList.empty()) { return nullptr; }

    std::reverse(folderNameList.begin(), folderNameList.end());
    for (const auto& folderName : folderNameList) {
        monthlySummaryDataList = std::move(loadMonthlySummaryData(folderName));

        if (!monthlySummaryDataList->empty()) {
            lastMonthlySummaryData = monthlySummaryDataList->back();
            return std::make_unique<MonthlySummaryData>(lastMonthlySummaryData);
        }
    }

    return nullptr;
}

/// @details nullptr is returned in case of empty yearly summary data
/// list.
std::unique_ptr<backend::YearlySummaryData> backend::IntermediateSystem::getLastYearlySummaryData() noexcept {

    auto yearlySummaryDataList = loadYearlySummaryData();

    if (yearlySummaryDataList->empty()) { return nullptr; }

    auto lastYearlySummaryData = yearlySummaryDataList->back();
    auto lastYearlyData = std::make_unique<backend::YearlySummaryData>(lastYearlySummaryData);

    return lastYearlyData;
}

/// @details There is no guarantee for having the same date
/// information of last data and today.
/// @return Total minutes of finished sessions of current day is
/// returned.
/// @parameters As usual in this program, last folder and file names
/// are required.
uint16_t backend::IntermediateSystem::getDailyTotalMinutes(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto dailyDataList = loadDailyData(lastFolderName, lastFileName);
    if (dailyDataList->empty()) { return 0; }

    auto theLastDailyData = dailyDataList->back();

    int16_t totalMinutes = 0;
    int8_t remainingSeconds = 0;

    _calculateAccumulatedTime(std::move(dailyDataList), theLastDailyData, remainingSeconds, totalMinutes);

    return totalMinutes;
}

uint16_t backend::IntermediateSystem::getDailyTotalMinutes(
        const std::string& lastFolderName,
        const std::string& lastFileName,
        const bool notCurrentDay
        ) noexcept {

    auto dailyDataList = loadDailyData(lastFolderName, lastFileName);
    if (dailyDataList->empty()) { return 0; }

    auto theLastDailyData = dailyDataList->back();

    int16_t totalMinutes = 0;
    int8_t remainingSeconds = 0;

    _calculateAccumulatedTime(
            std::move(dailyDataList),
            theLastDailyData,
            remainingSeconds,
            totalMinutes,
            notCurrentDay
            );

    return totalMinutes;
};

/// @details There is no guarantee for having the same date
/// information of last data and today.
uint16_t backend::IntermediateSystem::getDailyTotalNumberOfSessions(
        const std::string& lastFolderName,
        const std::string& lastFileName
        ) noexcept {

    auto dailyDataList = loadDailyData(lastFolderName, lastFileName);

    if (dailyDataList->empty()) { return 0; }
    auto lastDailyData = dailyDataList->back();

    bool todayAndLastDayIsTheSame = _compareToCurrentDate(lastDailyData);
    if (!todayAndLastDayIsTheSame) { return 0; }

    uint16_t totalNumberOfSessions = 0;
    bool multipleControls = false;
    for (const auto& dailyData : *dailyDataList) {
        multipleControls =
                (dailyData.modificationMode == BEGIN_MODE || dailyData.modificationMode == ADD_MODE) &&
                dailyData.day == lastDailyData.day &&
                dailyData.month == lastDailyData.month &&
                dailyData.year == lastDailyData.year;

        if (multipleControls) { ++totalNumberOfSessions; }
    }

    return totalNumberOfSessions;
}

std::unique_ptr<backend::DayAndTime> backend::IntermediateSystem::getPresentTime() noexcept {
    return std::move(_getCurrentDailyInformation());
}

std::unique_ptr<backend::DailyData> backend::IntermediateSystem::getActiveSessionInformation() noexcept {

    auto latestFolder = giveTheLatestYearFolder();
    auto latestFile = giveTheLatestWeekFile(latestFolder);
    auto dailyDataList = loadDailyData(latestFolder, latestFile);
    if (dailyDataList->empty()) { return nullptr; }
    if (dailyDataList->back().modificationMode != BEGIN_MODE) { return nullptr; }

    auto lastDailyData = dailyDataList->back();
    lastDailyData.hour = _convertIfMaxValueToZero(lastDailyData.hour);
    lastDailyData.minute = _convertIfMaxValueToZero(lastDailyData.minute);
    lastDailyData.second = _convertIfMaxValueToZero(lastDailyData.second);
    lastDailyData.weekDay = _convertIfMaxValueToZero(lastDailyData.weekDay);

    return std::make_unique<DailyData>(lastDailyData);
}

std::string backend::IntermediateSystem::generateWeekFileName(uint8_t nthWeek) noexcept {
    std::string weekName = "week-";
    weekName.append(std::to_string(nthWeek));
    return weekName;
}

std::string backend::IntermediateSystem::generateYearFolderName(uint8_t nthYear) noexcept {
    std::string yearName = "year-";
    yearName.append(std::to_string(nthYear));
    return yearName;
}

/// @return nullptr is returned when there is empty daily data list.
std::unique_ptr<std::vector<backend::DailyReportInterface>>
backend::IntermediateSystem::getDailyReport(
        const std::string& latestFolder,
        const std::string& latestFile
        ) noexcept {

    auto dailyDataList = loadDailyData(latestFolder, latestFile);

    if (dailyDataList->empty()) { return nullptr; }

    bool beginIsOn = false;
    auto currentDate = _getCurrentDailyInformation();
    std::vector<DailyReportInterface> dailyReportContainer;
    DailyData tempDailyData;
    CalculatedTime calculatedTime;

    for (const auto& dailyData : *(dailyDataList)) {
        if (!_dataBelongsToLastDay(dailyData, currentDate)) { continue; }

        if (dailyData.modificationMode == BEGIN_MODE) {
            tempDailyData = dailyData;
        }
        else if (dailyData.modificationMode == END_MODE) {
            calculatedTime = _calculateTime(dailyData, tempDailyData);
            tempDailyData.totalMinute = calculatedTime.minutes;
            tempDailyData.remainingSeconds = calculatedTime.seconds;

            _pushBackToDailyReportContainer(BEGIN_MODE, tempDailyData, dailyReportContainer);
        }
        else if (dailyData.modificationMode == ADD_MODE) {
            _pushBackToDailyReportContainer(ADD_MODE, dailyData,dailyReportContainer);
        }
        else if (dailyData.modificationMode == DELETE_MODE) {
            _pushBackToDailyReportContainer(DELETE_MODE, dailyData,dailyReportContainer);
        }
    }

    if (dailyReportContainer.empty()) { return nullptr; }

    return std::make_unique<std::vector<DailyReportInterface>>(dailyReportContainer);
}

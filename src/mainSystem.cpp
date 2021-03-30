#include "mainSystem.h"

extern std::unique_ptr<mount::Time> mount::getCurrentTime() noexcept {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    auto currentTime = std::make_unique<Time>();
    currentTime->hour = localTime->tm_hour;
    currentTime->minutes = localTime->tm_min;
    currentTime->seconds = localTime->tm_sec;
    return currentTime;
}

void mount::MainSystem::_updateLatestFiles() noexcept {
    _latestYearFolder = imSystem->giveTheLatestYearFolder();
    _latestWeekFile = imSystem->giveTheLatestWeekFile(_latestYearFolder);
}

bool mount::MainSystem::_makeDailySummary()  noexcept {
    _updateLatestFiles();
    auto dailySummaryData = imSystem->loadDailySummaryData(_latestYearFolder, _latestWeekFile);

    uint8_t size = dailySummaryData->size();
    if (size == DAY_NUMBER_IN_A_WEEK) { return true; }
    if (imSystem->lastDailyDataDateIsToday(_latestYearFolder, _latestWeekFile)) { return false; }
    if (imSystem->lastDailyDataIsBegin()) { return false; }

    return imSystem->saveDailySummaryData(_latestYearFolder, _latestWeekFile);
}

bool mount::MainSystem::_makeWeeklySummary() noexcept {
    _updateLatestFiles();
    auto weeklySummaryDataList = imSystem->loadWeeklySummaryData(_latestYearFolder);

    uint8_t size = weeklySummaryDataList->size();
    if (size == WEEK_NUMBER_IN_AN_ABSTRACT_YEAR) { return true; }

    imSystem->saveWeeklySummaryData(_latestYearFolder, _latestWeekFile);
    return false;
}

bool mount::MainSystem::_makeMonthlySummary() noexcept {
    _updateLatestFiles();
    auto monthlySummaryDataList = imSystem->loadMonthlySummaryData(_latestYearFolder);
    uint8_t size = monthlySummaryDataList->size();
    if (size == MONTH_NUMBER_IN_A_YEAR) { return true; }
    try {
        imSystem->saveMonthlySummaryData(_latestYearFolder, _latestWeekFile);
    }
    catch (const std::exception& exception) {
        std::cout << "Error : " << exception.what() << std::endl;
    }

    return false;
}

void mount::MainSystem::_makeYearlySummary() noexcept {
    imSystem->saveYearlySummaryData(_latestYearFolder);
}

void mount::MainSystem::_yearFolderOperation() noexcept {
    _updateLatestFiles();

    if (_latestYearFolder == imSystem->yearlyFolderNameStd) {
        imSystem->createNewYearFolder(_latestYearFolder);
        _updateLatestFiles();
        return;
    }

    imSystem->tidyUpLastDayData(_latestYearFolder, _latestWeekFile);
}

void mount::MainSystem::_monthlySummaryFileOperation() noexcept {
    _updateLatestFiles();

    auto monthlySummaryDataList = imSystem->loadMonthlySummaryData(_latestYearFolder);

    bool numberOfMonthlySummaryIsAtTheLimit = _makeMonthlySummary();
    if (numberOfMonthlySummaryIsAtTheLimit) {
        _makeYearlySummary();
        imSystem->createNewYearFolder(_latestYearFolder);
        _updateLatestFiles();
        return;
    }
}

bool mount::MainSystem::_weeklySummaryFileOperation() noexcept {
    _updateLatestFiles();

    auto weeklySummaryDataList = imSystem->loadWeeklySummaryData(_latestYearFolder);

    bool numberWeeklySummaryIsAtTheLimit = _makeWeeklySummary();
    if (numberWeeklySummaryIsAtTheLimit) {
        /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        /// In case of presence of 48 weekly summary files, the system
        /// automatically delete all 48 week-... files for decreasing
        /// memory cost.
//                imSystem->deleteAllWeekFiles(_latestYearFolder);
        _monthlySummaryFileOperation();
        return true;
    }

    return numberWeeklySummaryIsAtTheLimit;
}

void mount::MainSystem::_tidyUpUnabridgedData() noexcept {
    _updateLatestFiles();
    imSystem->tidyUpLastDayData(_latestYearFolder, _latestWeekFile);
}

/// checks and makes a monthly summary if necessary.
void mount::MainSystem::_makeAMonthlySummary() noexcept {
    _updateLatestFiles();
    imSystem->tidyUpMonthlySummaries(_latestYearFolder, _latestWeekFile);
}

void mount::MainSystem::generalFileOperation() {
    _yearFolderOperation();

    if (_latestWeekFile == imSystem->weeklyFileNameStd) {
        imSystem->createNewWeekFile(_latestYearFolder,_latestWeekFile);
        _updateLatestFiles();
        return;
    }

    _updateLatestFiles();

    auto dailySummaryDataList = imSystem->loadDailySummaryData(_latestYearFolder, _latestWeekFile);

    bool numberOfDailySummaryIsAtLimit = _makeDailySummary();

    if (numberOfDailySummaryIsAtLimit) {

        if (_weeklySummaryFileOperation()) {
            _updateLatestFiles();
            imSystem->createNewWeekFile(_latestYearFolder, _latestWeekFile);
            _updateLatestFiles();
            return;
        }

        imSystem->createNewWeekFile(_latestYearFolder, _latestWeekFile);
    }

    _tidyUpUnabridgedData(); /// delete begin  part at the end
}

mount::MainSystem::MainSystem() {
    imSystem = std::make_shared<backend::IntermediateSystem>();
    _tidyUpUnabridgedData();
    generalFileOperation();
    _makeAMonthlySummary();
    if (imSystem->isSettingsFileEmpty()) {
        imSystem->saveSettings(DEFAULT_TEXT_COLOR, DEFAULT_BACKGROUND_COLOR, DEFAULT_LANGUAGE);
    }
}

/// @return true when command is achieved, otherwise false.
bool mount::MainSystem::doWhatCommandIs(const uint8_t command, uint16_t minute) noexcept {
    generalFileOperation();
    _updateLatestFiles();

    if (imSystem->lastDailySummaryDataDateIsToday(_latestYearFolder, _latestWeekFile)) { return false; }
    if (command == 0) {
        return imSystem->saveDailyData(_latestYearFolder, _latestWeekFile, BEGIN_MODE);
    }
    if (command == 1) {
        return imSystem->saveDailyData(_latestYearFolder, _latestWeekFile, END_MODE);
    }
    if (command == 2 && minute != 0) {
        return imSystem->saveDailyData(_latestYearFolder, _latestWeekFile, ADD_MODE, minute);
    }
    if (command == 3 && minute != 0) {
        return imSystem->saveDailyData(_latestYearFolder, _latestWeekFile, DELETE_MODE, minute);
    }

    return false;
}

void mount::MainSystem::reset() noexcept {
    imSystem->deleteProgramFolder();
}

std::unique_ptr<backend::DayAndTime>
mount::MainSystem::getProgramStartInformation() noexcept {
    return std::move(imSystem->getProgramStartDayAndTime());
}

uint8_t mount::MainSystem::getDayOfProgramStart() noexcept {
    return imSystem->getProgramStartDay();
}

/// @return nullptr : no active session, *DailyData : an active session
std::unique_ptr<backend::DailyData> mount::MainSystem::getActiveSession() noexcept {
    return imSystem->getActiveSessionInformation();
}

/// calculates passed time from the beginning of a session.
uint16_t mount::MainSystem::calculateAccumulatedTime(const backend::DailyData& dailyData) noexcept {

    auto now = imSystem->getPresentTime();
    uint8_t hour = dailyData.hour;
    int8_t minute = dailyData.minute;
    int8_t second = dailyData.second;

    hour = now->hour - hour;
    minute = now->minute - minute;
    second = now->second - second;

    if (second < 0) {
        --minute;
        second += A_MINUTE_AS_SECONDS;
    }

    if (minute < 0) {
        --hour;
        minute += AN_HOUR_AS_MINUTES;
    }

    return hour * AN_HOUR_AS_MINUTES + minute;
}

uint16_t mount::MainSystem::getTotalMinutes() noexcept {
    _updateLatestFiles();
    return imSystem->getDailyTotalMinutes(_latestYearFolder, _latestWeekFile);
}

uint16_t mount::MainSystem::getPreviousDayTotalMinutes() noexcept {
    _updateLatestFiles();
    return imSystem->getDailyTotalMinutes(_latestYearFolder, _latestWeekFile, true);
}

uint16_t mount::MainSystem::getDailyTotalSessionNumber() noexcept {
    _updateLatestFiles();
    return imSystem->getDailyTotalNumberOfSessions(_latestYearFolder, _latestWeekFile);
}

std::unique_ptr<std::vector<backend::DailyReportInterface>> mount::MainSystem::getDailyReport() noexcept {
    _updateLatestFiles();
    return imSystem->getDailyReport(_latestYearFolder, _latestWeekFile);
}

/// @parameter dailySummary is an indicator for rendering
/// functions. true : there is a summary, false : there is not.
mount::DailySummary mount::MainSystem::getLatestDailySummary(bool& dailySummary) noexcept {
    _updateLatestFiles();
    DailySummary lastDailySummary = imSystem->getLastDailySummary(_latestYearFolder, _latestWeekFile);
    dailySummary = lastDailySummary != nullptr;

    return std::move(lastDailySummary);
}

/// @parameter weeklySummary is an indicator for rendering
/// functions. true : there is a summary, false : there is not.
mount::WeeklySummary mount::MainSystem::getLatestWeeklySummary(bool& weeklySummary) noexcept {
    _updateLatestFiles();
    auto lastWeeklySummary = imSystem->getLastWeeklySummaryData(_latestYearFolder);
    weeklySummary = lastWeeklySummary != nullptr;

    return std::move(lastWeeklySummary);
}

/// @parameter monthlySummary is an indicator for rendering
/// functions. true : there is a summary, false : there is not.
mount::MonthlySummary mount::MainSystem::getLatestMonthlySummary(bool& monthlySummary) noexcept {
    _updateLatestFiles();
    auto lastMonthlySummary = imSystem->getLastMonthlySummaryData(_latestYearFolder);
    monthlySummary = lastMonthlySummary != nullptr;

    return std::move(lastMonthlySummary);
}

/// @parameter yearlySummary is an indicator for rendering
/// functions. true : there is a summary, false : there is not.
mount::YearlySummary mount::MainSystem::getLatestYearlySummary(bool& yearlySummary) noexcept {
    _updateLatestFiles();
    auto lastYearlySummary = imSystem->getLastYearlySummaryData();
    yearlySummary = lastYearlySummary != nullptr;

    return std::move(lastYearlySummary);
}

std::unique_ptr<backend::Settings> mount::MainSystem::getSettings() noexcept {
    return imSystem->loadSettings();
}

void mount::MainSystem::setLanguageAdjustment(const uint8_t language) noexcept {
    imSystem->saveSettings(language);
}

void mount::MainSystem::setColorAdjustment(const uint16_t text, const uint16_t background) const noexcept {
    imSystem->saveSettings(text, background);
}

bool mount::MainSystem::doesLastDailyDataBelongToToday() noexcept {
    return imSystem->lastDailyDataDateIsToday(_latestYearFolder, _latestWeekFile);
}

[[nodiscard]] bool mount::MainSystem::isLastDailyDataModeBegin() const noexcept {
    return imSystem->lastDailyDataIsBegin();
}

/// is used if there is active session.
bool mount::MainSystem::willBeTotalMinutesAtLimit(
        const uint16_t accumulatedMinutes,
        const uint16_t totalMinutes
        ) noexcept {

    return (accumulatedMinutes + totalMinutes) >= MAXIMUM_MINUTES_IN_A_DAY;
}

bool mount::MainSystem::isTotalMinutesAtLimit() noexcept {
    _updateLatestFiles();
    auto totalMinutes = imSystem->getDailyTotalMinutes(_latestYearFolder, _latestWeekFile);
    return totalMinutes >= MAXIMUM_MINUTES_IN_A_DAY;
}

/// for forgottenActiveSessionInhibitor thread.
void mount::MainSystem::modifyDailyInformation(backend::DayAndTime &dailyInformation) noexcept {
    _updateLatestFiles();
    auto dailyDataList = imSystem->loadDailyData(_latestYearFolder, _latestWeekFile);
    auto lastDailyData = dailyDataList->back();

    dailyInformation.hour = 23; // Possible last hour.
    dailyInformation.minute = 59; // Possible last minute.
    dailyInformation.second = 58; // It is an arbitrarily chosen second value.
    dailyInformation.weekDay = lastDailyData.weekDay;
    dailyInformation.day = lastDailyData.day;
    dailyInformation.month = lastDailyData.month;
    dailyInformation.year = lastDailyData.year;
    dailyInformation.abstractYear = lastDailyData.abstractYear;
}

/// for forgottenActiveSessionInhibitor thread.
void mount::MainSystem::applyDailyDataWithEndCommand(backend::DayAndTime &dailyInformation) noexcept {
    _updateLatestFiles();
    auto dailyInformationCarrier = std::make_unique<backend::DayAndTime>(dailyInformation);

    imSystem->saveDailyDataWithEndCommand(
            _latestYearFolder,
            _latestWeekFile,
            std::move(dailyInformationCarrier)
    );
}

/// for forgottenActiveSessionInhibitor thread.
void mount::MainSystem::applyDailyDataWithDeleteCommand(
        backend::DayAndTime &dailyInformation,
        uint16_t &totalMinutes
        ) noexcept {

    _updateLatestFiles();
    dailyInformation.second = 59;
    auto dailyInformationCarrier = std::make_unique<backend::DayAndTime>(dailyInformation);

    if (totalMinutes <= MAXIMUM_MINUTES_IN_A_DAY) { return; }
    totalMinutes -= MAXIMUM_MINUTES_IN_A_DAY;
    imSystem->saveDailyDataWithDeleteCommand(
            _latestYearFolder,
            _latestWeekFile,
            totalMinutes,
            std::move(dailyInformationCarrier)
    );
}

/// for forgottenActiveSessionInhibitor thread.
/// @details This function starts a session at the beginning of day to continue an active session
/// successor of currently closed session of previous day.
void mount::MainSystem::applyMaintainerData() noexcept {
    _updateLatestFiles();
    uint8_t hour = 0;
    uint8_t minute = 0;
    imSystem->saveMaintainerDailyDataOfTodayWithCommand(
            _latestYearFolder,
            _latestWeekFile,
            BEGIN_MODE,
            hour,
            minute
    );
};

/// for forgottenActiveSessionInhibitor thread.
void mount::MainSystem::updateRequiredFileNames() noexcept {
    _updateLatestFiles();
}
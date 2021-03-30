#ifndef TIME_ACCUMULATOR_INTERMEDIATESYSTEM_H
#define TIME_ACCUMULATOR_INTERMEDIATESYSTEM_H

/// Flag to save start time and the rest of data.
#define BEGIN_MODE 0b0001u
/// Flag to save finish time and the rest of data.
#define END_MODE 0b0010u
/// Flag to save addition of entered minutes and
/// the rest of data.
#define ADD_MODE 0b0100u
/// Flag to save subtraction of entered minutes and
/// the rest of data.
#define DELETE_MODE 0b1000u

/// @For_Superstructure Flag explicitly utilized for daily data parsing.
#define DAILY_DATA_ID 0b0001u
/// @For_Superstructure Flag explicitly utilized for daily summary data
/// parsing.
#define DAILY_SUMMARY_ID 0b0010u
/// @For_Superstructure Flag explicitly utilized for weekly summary data
/// parsing.
#define WEEKLY_SUMMARY_DATA_ID 0b0100u
/// @For_Superstructure Flag explicitly utilized for monthly summary data
/// parsing.
#define MONTHLY_SUMMARY_DATA_ID 0b1000u
/// @For_Superstructure Flag explicitly utilized for yearly summary data
/// parsing.
#define YEARLY_SUMMARY_DATA_ID 0b10000u

#define MONTH_OFFSET 1
#define YEAR_OFFSET 1900
#define INDEX_OFFSET_TWO 2
#define AN_HOUR_AS_MINUTES 60
#define A_MINUTE_AS_SECONDS 60
#define WEEK_NUMBER_IN_A_MONTH 4
#define DAY_NUMBER_IN_A_WEEK 7
#define DAY_NUMBER_IN_A_MONTH 28
#define MONTH_NUMBER_IN_A_YEAR 12
#define WEEK_NUMBER_IN_A_YEAR 48
#define DAY_NUMBER_IN_A_YEAR 336
#define FIRST_WEEK 1
#define LAST_WEEK 4
#define RANGE_BETWEEN_FIRST_DATA 3u

#include <ctime>
#include <cstdio>
#include "language.h"
#include "fileSystem.h"
#include "msgpack.hpp"

namespace backend {
    struct DayAndTime {

        uint8_t weekDay;
        uint8_t day;
        uint8_t month;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t abstractYear;
        uint16_t year;

        DayAndTime() : weekDay(UINT8_MAX), day(UINT8_MAX), month(UINT8_MAX), hour(UINT8_MAX), minute(UINT8_MAX),
                       second(UINT8_MAX), abstractYear(UINT8_MAX), year(UINT16_MAX) {};
    };

    struct DailyData {
        /// @id Daily Data ID = 0b0001
        uint8_t universalMode;

        /// @flags Start Mode: BEGIN_MODE
        /// @flags Finish Mode: END_MODE
        /// @flags Add Mode: ADD_MODE
        /// @flags Delete Mode: DELETE_MODE
        uint8_t modificationMode;

        // Common variables for each mode
        uint8_t weekDay;
        uint8_t day;
        uint8_t month;
        uint8_t hour;
        uint8_t second;
        uint8_t abstractYear;
        uint8_t minute;
        uint8_t remainingSeconds;
        uint16_t totalMinute;
        uint16_t year;

        DailyData() : universalMode(DAILY_DATA_ID), modificationMode(UINT8_MAX), weekDay(UINT8_MAX), day(UINT8_MAX),
                      month(UINT8_MAX), hour(UINT8_MAX), second(UINT8_MAX), abstractYear(UINT8_MAX), minute(UINT8_MAX),
                      remainingSeconds(UINT8_MAX), totalMinute(UINT16_MAX), year(UINT16_MAX) {};
        MSGPACK_DEFINE (modificationMode, weekDay, day, month, year, abstractYear, hour, minute, remainingSeconds,
                        totalMinute, second);
    };

    struct DailySummaryData {
        /// @id DAILY_SUMMARY_ID
        uint8_t universalMode;
        uint8_t weekDay;
        uint8_t day;
        uint8_t month;
        uint8_t abstractYear;
        uint16_t year;
        uint16_t dailyRemainingSeconds;
        uint16_t dailyTotalMinutes;
        uint16_t dailyTotalSessionNumber;

        DailySummaryData() : universalMode(DAILY_SUMMARY_ID), weekDay(UINT8_MAX), day(UINT8_MAX), month(UINT8_MAX),
                             abstractYear(UINT8_MAX), year(UINT16_MAX), dailyRemainingSeconds(UINT16_MAX),
                             dailyTotalMinutes(UINT16_MAX), dailyTotalSessionNumber(UINT16_MAX) {};
        MSGPACK_DEFINE (weekDay, day, month, abstractYear, year, dailyRemainingSeconds, dailyTotalMinutes,
                        dailyTotalSessionNumber);
    };

    struct WeeklySummaryData {
        /// @id WEEKLY_SUMMARY_DATA_ID
        uint8_t universalMode;
        uint8_t firstDay;
        uint8_t lastDay;
        uint8_t firstMonth;
        uint8_t lastMonth;
        uint8_t abstractYear;
        uint16_t firstYear;
        uint16_t lastYear;
        uint16_t weeklyTotalMinutes;
        uint32_t weeklyTotalSessionNumber;
        std::string weekName{};

        WeeklySummaryData() : universalMode(WEEKLY_SUMMARY_DATA_ID), firstDay(UINT8_MAX), lastDay(UINT8_MAX),
                              firstMonth(UINT8_MAX), lastMonth(UINT8_MAX), abstractYear(UINT8_MAX),
                              firstYear(UINT16_MAX), lastYear(UINT16_MAX), weeklyTotalMinutes(UINT16_MAX),
                              weeklyTotalSessionNumber(UINT32_MAX) {};
        MSGPACK_DEFINE (firstDay, lastDay, firstMonth, lastMonth, abstractYear, firstYear, lastYear, weeklyTotalMinutes,
                        weeklyTotalSessionNumber, weekName);
    };

    struct MonthlySummaryData {
        /// @id MONTHLY_SUMMARY_DATA_ID
        uint8_t universalMode;
        uint8_t monthNumber;
        uint8_t abstractYear;
        uint16_t year;
        uint16_t monthlyTotalMinutes;
        uint32_t monthlyTotalSessionNumber;
        std::string firstWeek{};
        std::string lastWeek{};

        MonthlySummaryData() : universalMode(MONTHLY_SUMMARY_DATA_ID), monthNumber(UINT8_MAX), abstractYear(UINT8_MAX),
                               year(UINT16_MAX), monthlyTotalMinutes(UINT16_MAX),
                               monthlyTotalSessionNumber(UINT32_MAX) {};
        MSGPACK_DEFINE (monthNumber, abstractYear, year, monthlyTotalMinutes, monthlyTotalSessionNumber, firstWeek,
                        lastWeek);
    };

    struct YearlySummaryData {
        /// @id YEARLY_SUMMARY_DATA_ID
        uint8_t universalMode;
        uint8_t abstractYear;
        uint16_t year;
        uint32_t yearlyTotalMinutes;
        uint32_t yearlyTotalSessionNumber;
        std::string imaginaryYear{};

        YearlySummaryData() : universalMode(YEARLY_SUMMARY_DATA_ID), abstractYear(UINT8_MAX), year(UINT16_MAX),
                              yearlyTotalMinutes(UINT32_MAX), yearlyTotalSessionNumber(UINT32_MAX) {};
        MSGPACK_DEFINE (abstractYear, year, yearlyTotalMinutes, yearlyTotalSessionNumber, imaginaryYear);
    };

    struct Settings {
        uint16_t textColor;
        uint16_t backgroundColor;
        uint8_t language;

        Settings() : textColor(UINT16_MAX), backgroundColor(UINT16_MAX), language(UINT8_MAX) {};
        MSGPACK_DEFINE (textColor, backgroundColor, language);
    };

    struct DailyReportInterface {
        /// @flags Begin: 0b0001
        /// @flags Add: 0b0100
        /// @flags Delete: 0b1000
        uint8_t commandCode = 0;
        uint8_t hour = 0;
        int8_t minute = 0;
        int8_t second = 0;
        uint8_t remainingSecond = 0;
        uint16_t totalMinute = 0;
    };

    struct CalculatedTime {
        uint8_t seconds = 0;
        uint16_t minutes = 0;
    };

    template<typename T>
    concept Integral = std::is_integral<T>::value;

    class IntermediateSystem : public backend::FileSystem {
    private:
        DayAndTime _programStartDayAndTime;

    private:
        template<Integral T>
        T _convertIfZeroToMaxValue(const T& convertibleObject) {
            uint16_t sizeOfType = sizeof(convertibleObject);
            uint16_t sizeOfUint8, sizeOfUint16, sizeOfUint32;

            sizeOfUint8 = sizeof(uint8_t);
            sizeOfUint16 = sizeof(uint16_t);
            sizeOfUint32 = sizeof(uint32_t);

            T tempVariable = convertibleObject;
            if (tempVariable == 0) {
                if (sizeOfType == sizeOfUint8) { tempVariable = UINT8_MAX; }
                if (sizeOfType == sizeOfUint16) { tempVariable = UINT16_MAX; }
                if (sizeOfType == sizeOfUint32) { tempVariable = UINT32_MAX; }
            }

            return tempVariable;
        }

        template<Integral T>
        T _convertIfMaxValueToZero(const T& convertibleObject) {
            uint16_t sizeOfType = sizeof(convertibleObject);
            uint16_t sizeOfUint8, sizeOfUint16, sizeOfUint32;

            sizeOfUint8 = sizeof(uint8_t);
            sizeOfUint16 = sizeof(uint16_t);
            sizeOfUint32 = sizeof(uint32_t);

            T tempVariable = convertibleObject;

            if (sizeOfType == sizeOfUint8 && tempVariable == UINT8_MAX) {
                tempVariable = (uint8_t)0;
            }
            if (sizeOfType == sizeOfUint16 && tempVariable == UINT16_MAX) {
                tempVariable = 0;
            }
            if (sizeOfType == sizeOfUint32 && tempVariable == UINT32_MAX) {
                tempVariable = 0;
            }

            return tempVariable;
        }

        tm* _getDailyInformation() noexcept;

        std::unique_ptr<Settings> _loadSettings() noexcept;

        uint8_t _getLatestAbstractYearNumber() noexcept;

        void _setProgramStartDayAndTime() noexcept;

        std::unique_ptr<DayAndTime> _getCurrentDailyInformation() noexcept;

        void _generateStringWithExtension(std::string& pureFileName) noexcept;

        void _loadFileToString(
                std::string& charString,
                const std::string& latestFile,
                const std::string& latestFolder
                ) noexcept;

        void _listObjects(
                const std::string& latestFile,
                const std::string& latestFolder,
                std::vector<msgpack::object_handle>& handledObjectList,
                const uint16_t& operationId
                ) noexcept;

        /// For all _load function family.
        /// @details This function has to be used with _listObjectFunction.
        /// The given operationId parameter of the _listObjectFunction and
        /// type of dataList parameter of this function have to refer the same
        /// operation.
        template <typename T>
        std::unique_ptr<std::vector<T>> _combOutRequiredData(
                std::unique_ptr<std::vector<T>> dataList,
                std::vector<msgpack::object_handle>& handledObjectList
                ) noexcept {

            std::stringstream stringStream;

            /// Deserialization is made here.
            for (const auto& handledObject : handledObjectList) {
                msgpack::object obj = handledObject.get();
                auto convertedObject = obj.convert();
                dataList->emplace_back(convertedObject);
            }

            return dataList;
        }

        /// for all _save function family.
        template <typename T>
        void _saveAsMsgpack(const std::string& folderName, const std::string& fileName, const T& data) noexcept {

            msgpack::sbuffer buffer;
            msgpack::packer<msgpack::sbuffer> packer(&buffer);
            packer.pack(data);

            std::stringstream bufferStream;
            bufferStream << data.universalMode << buffer.data() << "\n\n";

            saveFile(bufferStream.str(), fileName, folderName);
        }

        void _saveAsMsgpack(const std::string& fileName, std::unique_ptr<Settings> data) noexcept;

        bool _compareToCurrentDate(const DailyData& dailyData) noexcept;

        uint16_t _calculateAccumulatedTime(
                std::unique_ptr<std::vector<DailyData>> dailyDataList,
                const DailyData& lastDailyData,
                int8_t& remainingSeconds,
                int16_t& totalMinutes,
                bool summary = false
                ) noexcept;

        void _pushBackToDailyReportContainer(
                uint8_t mode,
                const DailyData& data,
                std::vector<DailyReportInterface>& container
                ) const noexcept;

        bool _dataBelongsToLastDay(const DailyData& data, std::unique_ptr<DayAndTime>& now) noexcept;

        CalculatedTime _calculateTime(const DailyData& dailyData, const DailyData& previousDailyData) noexcept;

        uint8_t _getFileNumber(const std::string& fileName) noexcept;

    public:
        IntermediateSystem() {
            _setProgramStartDayAndTime();
        }

        std::unique_ptr<DayAndTime> getProgramStartDayAndTime() noexcept;

        uint8_t getProgramStartDay() noexcept;

        std::unique_ptr<std::vector<DailyData>> loadDailyData(
                const std::string& folderName,
                const std::string& fileName
                ) noexcept;

        bool saveDailyData(
                const std::string& folderName,
                const std::string& fileName,
                const uint8_t command,
                uint16_t addableOrDeletableMinutes = 0
                ) noexcept;

        void saveDailyDataWithEndCommand(
                const std::string& folderName,
                const std::string& fileName,
                const std::unique_ptr<DayAndTime> dailyInformation
                ) noexcept;

        void saveMaintainerDailyDataOfTodayWithCommand(
                const std::string& folderName,
                const std::string& fileName,
                const uint8_t command,
                const uint8_t hour,
                const uint8_t minute
                ) noexcept;

        void saveDailyDataWithDeleteCommand(
                const std::string& folderName,
                const std::string& fileName,
                const uint16_t minute,
                const std::unique_ptr<DayAndTime> dailyInformation
                ) noexcept;

        std::unique_ptr<std::vector<DailySummaryData>> loadDailySummaryData(
                const std::string& folderName,
                const std::string& fileName
                ) noexcept;

        bool saveDailySummaryData(const std::string& folderName, const std::string& fileName) noexcept;

        std::unique_ptr<std::vector<WeeklySummaryData>> loadWeeklySummaryData(const std::string& folderName) noexcept;

        bool saveWeeklySummaryData(const std::string& folderName, const std::string& fileName) noexcept;

        std::unique_ptr<std::vector<MonthlySummaryData>> loadMonthlySummaryData(const std::string& folderName) noexcept;

        bool saveMonthlySummaryData(const std::string& folderName, const std::string& fileName);

        std::unique_ptr<std::vector<YearlySummaryData>> loadYearlySummaryData() noexcept;

        bool saveYearlySummaryData(const std::string& folderName) noexcept;

        bool isSettingsFileEmpty() noexcept;

        std::unique_ptr<Settings> loadSettings() noexcept;

        void saveSettings(const uint8_t& language) noexcept;

        void saveSettings(const uint16_t& textColor, const uint16_t& backgroundColor) noexcept;

        void saveSettings(
                const uint16_t& textColor,
                const uint16_t& backgroundColor,
                const uint8_t& language
                ) noexcept;

        bool lastDailyDataIsBegin() noexcept;

        void tidyUpLastDayData(const std::string& lastFolderName, const std::string& lastFileName) noexcept;

        void tidyUpMonthlySummaries(const std::string& lastFolderName, const std::string& lastFileName) noexcept;

        bool lastDailyDataDateIsToday(const std::string& lastFolderName, const std::string& lastFileName) noexcept;

        bool lastDailySummaryDataDateIsToday(
                const std::string& lastFolderName,
                const std::string& lastFileName
                ) noexcept;

        std::unique_ptr<DailySummaryData> getLastDailySummary(
                const std::string& lastFolderName,
                const std::string& lastFileName
                ) noexcept;

        std::unique_ptr<WeeklySummaryData> getLastWeeklySummaryData(const std::string& lastFolderName) noexcept;

        std::unique_ptr<MonthlySummaryData> getLastMonthlySummaryData(const std::string& lastFolderName) noexcept;

        std::unique_ptr<YearlySummaryData> getLastYearlySummaryData() noexcept;

        uint16_t getDailyTotalMinutes(const std::string& lastFolderName, const std::string& lastFileName) noexcept;

        uint16_t getDailyTotalMinutes(
                const std::string& lastFolderName,
                const std::string& lastFileName,
                bool notCurrentDay
                ) noexcept;

        uint16_t getDailyTotalNumberOfSessions(
                const std::string& lastFolderName,
                const std::string& lastFileName
                ) noexcept;

        std::unique_ptr<DayAndTime> getPresentTime() noexcept;

        std::unique_ptr<DailyData> getActiveSessionInformation() noexcept;

        std::string generateWeekFileName(uint8_t nthWeek) noexcept;

        std::string generateYearFolderName(uint8_t nthYear) noexcept;

        std::unique_ptr<std::vector<DailyReportInterface>> getDailyReport(
                const std::string& latestFolder,
                const std::string& latestFile
                ) noexcept;
    };
}

#endif //TIME_ACCUMULATOR_INTERMEDIATESYSTEM_H

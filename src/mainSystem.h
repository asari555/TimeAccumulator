#ifndef TIME_ACCUMULATOR_MAINSYSTEM_H
#define TIME_ACCUMULATOR_MAINSYSTEM_H

#define DEFAULT_TEXT_COLOR 0
#define DEFAULT_BACKGROUND_COLOR 29
#define WEEK_NUMBER_IN_AN_ABSTRACT_YEAR 48
#define MAXIMUM_MINUTES_IN_A_DAY 1440

#include <atomic>
#include <thread>
#include "intermediateSystem.h"

namespace mount {
    struct Time {
        int hour = 0;
        int minutes = 0;
        int seconds = 0;
    };

    typedef std::unique_ptr<backend::DailySummaryData> DailySummary;
    typedef std::unique_ptr<backend::WeeklySummaryData> WeeklySummary;
    typedef std::unique_ptr<backend::MonthlySummaryData> MonthlySummary;
    typedef std::unique_ptr<backend::YearlySummaryData> YearlySummary;

    extern std::unique_ptr<Time> getCurrentTime() noexcept;

    class MainSystem {
    private:
        /// @definition Temporary variable for
        /// the latest version of lastYear folder.
        std::string _latestYearFolder{};
        /// @definition Temporary variable for
        /// the latest version of week file.
        std::string _latestWeekFile{};

    private:
        void _updateLatestFiles() noexcept;

        bool _makeDailySummary() noexcept;

        bool _makeWeeklySummary() noexcept;

        bool _makeMonthlySummary() noexcept;

        void _makeYearlySummary() noexcept;

        void _yearFolderOperation() noexcept;

        void _monthlySummaryFileOperation() noexcept;

        bool _weeklySummaryFileOperation() noexcept;

        void _tidyUpUnabridgedData() noexcept;

        void _makeAMonthlySummary() noexcept;

    public:
        std::shared_ptr<backend::IntermediateSystem> imSystem;

    public:
        void generalFileOperation();

        MainSystem();

        /// @return true when command is achieved, otherwise false.
        bool doWhatCommandIs(const uint8_t command, uint16_t minute = 0) noexcept;

        void reset() noexcept;

        std::unique_ptr<backend::DayAndTime> getProgramStartInformation() noexcept;

        uint8_t getDayOfProgramStart() noexcept;

        std::unique_ptr<backend::DailyData> getActiveSession() noexcept;

        uint16_t calculateAccumulatedTime(const backend::DailyData &dailyData) noexcept;

        uint16_t getTotalMinutes() noexcept;

        uint16_t getPreviousDayTotalMinutes() noexcept;

        uint16_t getDailyTotalSessionNumber() noexcept;

        std::unique_ptr<std::vector<backend::DailyReportInterface>> getDailyReport() noexcept;

        DailySummary getLatestDailySummary(bool& dailySummary) noexcept;

        WeeklySummary getLatestWeeklySummary(bool& weeklySummary) noexcept;

        MonthlySummary getLatestMonthlySummary(bool& monthlySummary) noexcept;

        YearlySummary getLatestYearlySummary(bool& yearlySummary) noexcept;

        std::unique_ptr<backend::Settings> getSettings() noexcept;

        void setLanguageAdjustment(const uint8_t language) noexcept;

        void setColorAdjustment(const uint16_t text, const uint16_t background) const noexcept;

        bool doesLastDailyDataBelongToToday() noexcept;

        [[nodiscard]] bool isLastDailyDataModeBegin() const noexcept;

        bool willBeTotalMinutesAtLimit(const uint16_t accumulatedMinutes, const uint16_t totalMinutes) noexcept;

        bool isTotalMinutesAtLimit() noexcept;

        void modifyDailyInformation(backend::DayAndTime &dailyInformation) noexcept;

        void applyDailyDataWithEndCommand(backend::DayAndTime &dailyInformation) noexcept;

        void applyDailyDataWithDeleteCommand(backend::DayAndTime &dailyInformation, uint16_t &totalMinutes) noexcept;

        void applyMaintainerData() noexcept;

        void updateRequiredFileNames() noexcept;
    };
}

#endif //TIME_ACCUMULATOR_MAINSYSTEM_H

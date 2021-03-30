#include "liveApplication.h"

std::condition_variable isThreadEnded;
std::mutex uniqueMutex;

int main() {
    auto mainSystem = std::make_shared<mount::MainSystem>();

    auto screen = ScreenInteractive::TerminalOutput();
    liveApplication::TimeAccumulator timeAccumulator(mainSystem);
    timeAccumulator.on_quit = screen.ExitLoopClosure();

    /// 1. Thread
    std::thread update([&]() {
        std::unique_lock<std::mutex> lock(uniqueMutex);

        while (isThreadEnded.wait_for(lock,std::chrono::milliseconds(500)) == std::cv_status::timeout) {

            screen.PostEvent(Event::Custom);
        }
    });

    /// 2. Thread
    std::thread dayChange([&]() {
        std::unique_lock<std::mutex> lock(uniqueMutex);
        auto currentTimeData = mount::getCurrentTime();
        int hour, minute, second;
        uint16_t waitInMilliseconds = 500;

        std::string latestFolder = {};
        std::string latestFile = {};

        while (
                isThreadEnded.wait_for(
                lock,
                std::chrono::milliseconds(waitInMilliseconds)
                ) == std::cv_status::timeout
                ) {

            currentTimeData = mount::getCurrentTime();
            hour = currentTimeData->hour;
            minute = currentTimeData->minutes;
            second = currentTimeData->seconds;

            if (hour == 23 && minute == 59 && second >= 45) { waitInMilliseconds = 500; }
            else if (hour == 0 && minute == 0 && second <= 10) { waitInMilliseconds = 500; }
            else { waitInMilliseconds = 1000; }

            mainSystem->updateRequiredFileNames();

            if (mainSystem->doesLastDailyDataBelongToToday()) { continue; }
            if (!mainSystem->isLastDailyDataModeBegin()) { continue; }

            backend::DayAndTime dailyInformation;

            mainSystem->modifyDailyInformation(dailyInformation);

            mainSystem->applyDailyDataWithEndCommand(dailyInformation);

            auto totalMinutes = mainSystem->getPreviousDayTotalMinutes();

            mainSystem->applyDailyDataWithDeleteCommand(dailyInformation, totalMinutes);

            mainSystem->generalFileOperation();

            mainSystem->applyMaintainerData();
        }
    });

    screen.Loop(&timeAccumulator);
    update.join();
    dayChange.join();
    return 0;
}


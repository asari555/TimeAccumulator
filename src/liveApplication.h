#ifndef TUIGALLERY_LIVEAPPLICATION_H
#define TUIGALLERY_LIVEAPPLICATION_H

#define MAX_SESSION_NUMBER 99
#define BACKGROUND_COLOR_MENU_OFFSET 16
#define ROOT 1
#define INDEX_OFFSET_ONE 1
#define TWO_ROWS 2

#include <atomic>
#include <algorithm>
#include <mutex>
#include <ftxui/component/menu.hpp>
#include <ftxui/screen/terminal.hpp>
#include <ftxui/component/input.hpp>
#include <ftxui/component/button.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/container.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/toggle.hpp>
#include "language.h"
#include "mainSystem.h"

using namespace ftxui;

extern uint8_t commonLanguage;
extern std::condition_variable isThreadEnded;

namespace liveApplication {
    enum commands { beginCommand, endCommand, addCommand, deleteCommand };

    extern std::wstring digitsToWstring(uint8_t number) noexcept;

    extern std::wstring fourDigitsToWstring(uint16_t number) noexcept;

    struct Depth {
        bool pushedReturn = false;
        /// @return Summary : 0001
        /// @return Help    : 0010
        /// @return About   : 0100
        uint8_t pageCode = 0;
    };

    struct MainPageSummaryInterface {
        /// @return 0001 : Last daily summary
        /// @return 0010 : Last weekly summary
        /// @return 0100 : Last monthly summary
        /// @return 1000 : Last yearly summary
        uint8_t selectedSummary = 0;
        bool summaryIsSelected = false;
    };

    class CommonColor {
    public:
        /// should be assigned firstly.
        std::shared_ptr<mount::MainSystem> mainSystem;

        const Color defaultTextColor = Color::Black;
        const Color defaultBackgroundColor = Color::White;

        Color textColor = defaultTextColor;
        Color backgroundColor = defaultBackgroundColor;

        std::vector<Color> textColorList = {
                Color::Black, Color::Red, Color::Green, Color::Yellow, Color::Blue, Color::Magenta, Color::Cyan,
                Color::GrayDark, Color::GrayLight, Color::RedLight, Color::GreenLight, Color::YellowLight,
                Color::BlueLight, Color::MagentaLight, Color::CyanLight, Color::White
        };

        std::vector<Color> backgroundColorList = {
                Color::Aquamarine1, Color::BlueViolet, Color::NavyBlue, Color::CadetBlue, Color::DarkTurquoise,
                Color::Violet, Color::DarkMagenta, Color::Yellow1, Color::GreenYellow, Color::DarkSeaGreen4,
                Color::DarkGreen, Color::HotPink3, Color::MediumPurple2Bis, Color::LightGoldenrod3, Color::Gold1,
                Color::NavajoWhite3, Color::Orange3, Color::OrangeRed1, Color::SandyBrown, Color::LightCoral,
                Color::LightGoldenrod1, Color::Wheat4, Color::MistyRose1, Color::MistyRose3, Color::DarkRed,
                Color::DarkRedBis, Color::Grey37, Color::LightSlateGrey, Color::Black, Color::White
        };

        const uint8_t textColorListSize = textColorList.size();
        const uint8_t backgroundColorListSize = backgroundColorList.size();

    public:
        void adjustColorPair() noexcept;

        void setDefaultColorPair() noexcept;

        void choseDefaultColorPair() noexcept;

        void savePreferredColorPair() const noexcept;

        [[nodiscard]] bool isThereSavedSettings() const noexcept;
    };

    class AddAndDelete : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        Container _container = Container::Horizontal();
        Element _inputName;
        Input _input;

    private:
        [[nodiscard]] bool _isInputAPositiveInteger(const std::wstring &input) const noexcept;

        /// for AddAndDelete class
        [[nodiscard]] bool _isInputValid(const std::wstring &input, const bool &mode) const noexcept;

    public:
        uint16_t enteredMinute = 0;
        /// @return true : add, false : delete
        bool addOrDelete = true;
        std::shared_ptr<mount::MainSystem> mainSystem;
        std::shared_ptr<CommonColor> commonColor;

    public:
        std::function<void()> closeWindow = [] {};

        explicit AddAndDelete(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        Element Render() noexcept final {
            if (addOrDelete) {
                _inputName = text(_languagePreference->getRequiredWord(1));
            }
            else {
                _inputName = text(_languagePreference->getRequiredWord(2));
            }

            return hbox({_inputName, _input.Render()})
                    | border | center
                    | size(HEIGHT, EQUAL, 3) // 3 units
                    | color(commonColor->textColor)
                    | bgcolor(commonColor->backgroundColor);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                _input.content = L"";
                closeWindow();
            }
            return Component::OnEvent(event);
        };
    };


    class Summaries : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        Container _container = Container::Horizontal();
        Menu _summaries;

    public:
        std::shared_ptr<CommonColor> commonColor;
        std::shared_ptr<MainPageSummaryInterface> interface;

    public:
        std::function<void()> closeWindow;

        explicit Summaries(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        Element Render() noexcept final {
            return window(text(_languagePreference->getRequiredWord(7)),
                          _summaries.Render() | center)
                   | color(commonColor->textColor) | bgcolor(commonColor->backgroundColor)
                   | size(WIDTH, EQUAL, 20) | size(HEIGHT, EQUAL, 10);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                closeWindow();
            }
            return Component::OnEvent(event);
        };
    };

    class Customise : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        Container _container = Container::Horizontal();
        Container _buttonContainer = Container::Vertical();
        Button _defaultButton;
        Button _saveButton;
        Menu _textColors;
        Menu _backgroundColors1;
        Menu _backgroundColors2;
        bool _initialColorsWereTaken = false;
        bool _aButtonWasPushed = false;

    private:
        void _assignPreviousCommonColor() noexcept;

    public:
        std::function<void()> closeWindow;
        std::shared_ptr<CommonColor> commonColor;
        CommonColor previousCommonColor;

    public:
        explicit Customise(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        Element Render() noexcept final {
            return window(
                    text(_languagePreference->getRequiredWord(56)),
                    hbox({
                                 vbox({
                                     hbox({
                                         text(_languagePreference->getRequiredWord(57)) | bold
                                         | size(WIDTH, EQUAL,20),
                                         text(_languagePreference->getRequiredWord(58)) | bold,
                                     }),
                                     hbox({
                                         _textColors.Render() | size(WIDTH, EQUAL,20),
                                         _backgroundColors1.Render() | size(WIDTH, EQUAL,18),
                                         _backgroundColors2.Render() | size(WIDTH, EQUAL,16)
                                     })
                                 }),
                                 vbox({
                                     _defaultButton.Render(),
                                     _saveButton.Render()
                                 }) | center
                    })
            ) | color(commonColor->textColor) | bgcolor(commonColor->backgroundColor);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                if (!_aButtonWasPushed && !commonColor->isThereSavedSettings()) {
                    commonColor->textColor = previousCommonColor.textColor;
                    commonColor->backgroundColor = previousCommonColor.backgroundColor;
                }
                _aButtonWasPushed = false;
                closeWindow();
            }
            return Component::OnEvent(event);
        };
    };

    class Language : public Component {
    private:
        std::shared_ptr<mount::MainSystem> _mainSystem;
        Container _container = Container::Vertical();
        Menu _language;
        std::vector<std::wstring> _requiredSentences =
                { L"Dil Tercihleri", L"Language Preferences",
                  L"Dildeki değişikliği görmek için lütfen programı yeniden"
                  " başlatın.",
                  L"Please restart the program to see the change in language." };

    private:
        [[nodiscard]] bool _areSettingsEmpty() const noexcept;

        void _setLanguageSetting(uint8_t preference) const noexcept;

        void _applyDefaultIfPossible() const noexcept;

        void _getLanguageSetting(uint8_t& language) const noexcept;

        [[nodiscard]] bool _compareCurrentPreferenceWithPreviousOne(const int& selected) const noexcept;

        [[nodiscard]] const std::wstring& _printWindowLabel() const noexcept;

        [[nodiscard]] const std::wstring& _printInformativeText() const noexcept;

    public:
        std::shared_ptr<CommonColor> commonColor;
        std::function<void()> closeWindow;

    public:
        explicit Language(std::shared_ptr<mount::MainSystem> mainSystem) noexcept;

        Element Render() noexcept override {
            return window(text(_printWindowLabel()),
                           vbox({
                                        text(L""),
                                        hflow(paragraph(_printInformativeText())) | bold,
                                        text(L""),
                                        _language.Render() | center | color(commonColor->textColor)
                                            | bgcolor(commonColor->backgroundColor),
                                        text(L"")
                                })
                    ) | color(commonColor->textColor) | bgcolor(commonColor->backgroundColor)
                      | size(WIDTH, EQUAL, 37) | size(HEIGHT, EQUAL, 10);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                closeWindow();
            }
            else if (event == Event::Return) {
                auto selectedLanguage = _language.selected;
                if (_compareCurrentPreferenceWithPreviousOne(selectedLanguage)) {
                    _setLanguageSetting(selectedLanguage);
                    closeWindow();
                }
            }
            return Component::OnEvent(event);
        }
    };

    class Reset : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        Container _container = Container::Horizontal();
        Button _noButton;
        Button _yesButton;

    public:
        std::shared_ptr<mount::MainSystem> mainSystem;
        std::shared_ptr<CommonColor> commonColor;
        std::function<void()> closeWindow;

    public:
        explicit Reset(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        Element Render() noexcept final {
            return window(
                    text(_languagePreference->getRequiredWord(61)),
                    vbox({
                                 text(_languagePreference->getRequiredWord(62)),
                                 hbox({
                                     _noButton.Render() | size(WIDTH, EQUAL, 7) | center,
                                     _yesButton.Render() | size(WIDTH, EQUAL, 7) | center,
                                     }) | center
                                 })
                    ) | color(commonColor->textColor) | bgcolor(commonColor->backgroundColor);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                closeWindow();
            }
            return Component::OnEvent(event);
        };
    };

    class AttentionWindow : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        Container _container = Container::Horizontal();
        Button _okButton;

    public:
        std::shared_ptr<CommonColor> commonColor;
        std::function<void()> closeWindow;
        bool thereIsActiveSession = false;
        bool totalMinutesAreAtLimit = false;
        bool sessionNumberIsAtLimit = false;

    public:
        explicit AttentionWindow(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        Element Render() noexcept final {
            Element dialogText;
            if (thereIsActiveSession) {
                dialogText = text(_languagePreference->getRequiredWord(64));
            }
            else if (totalMinutesAreAtLimit) {
                dialogText = text(_languagePreference->getRequiredWord(180));
            }
            else if (sessionNumberIsAtLimit) {
                dialogText = text(_languagePreference->getRequiredWord(65));
            }
            else {
                dialogText = text(_languagePreference->getRequiredWord(66));
            }

            return window(
                    text(_languagePreference->getRequiredWord(61)),
                    vbox({
                        dialogText | size(HEIGHT, EQUAL, 3), // 3 units
                        _okButton.Render() | center,
                        })) | color(commonColor->textColor) | bgcolor(commonColor->backgroundColor);
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                closeWindow();
            }
            return Component::OnEvent(event);
        };
    };

    class MainPage : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        // interactive part
        Container _container = Container::Horizontal();
        Menu _commandMenu;
        Menu _appendixMenu;
        Depth _depth;
        // window drawing
        Terminal::Dimensions _currentPageSize{};
        int _pageWidth = _currentPageSize.dimx,
            _mainPartWidth = 0,
            _remainingPartWidth1 = 0,
            _remainingPartWidth2 = 0,
            _remainingPartWidth3 = 0;
        int _pageHeight = _currentPageSize.dimy,
            _specialPageHeightForMainPage = 0,
            _remainingPageHeight = 0;
        std::wstring _weekDay = {};
        // active session
        std::unique_ptr<backend::DailyData> _activeSession;
        uint16_t _accumulatedMinutes = 0;
        // total variable
        uint16_t _totalMinutes = 0, _totalSessions = 0;
        // page scrolling at daily report
        uint16_t _cursor = 0;
        uint16_t _vectorSize = 0;
        uint8_t _screenCapacity = _pageHeight - TWO_ROWS;
        // summaries
        bool thereIsADailySummary = false;
        bool thereIsAWeeklySummary = false;
        bool thereIsAMonthlySummary = false;
        bool thereIsAYearlySummary = false;

    private:
        /// @abrivitions S: Session
        /// @abrivitions A: Add
        /// @abrivitions D: Delete
        enum ReportDatumHeaders {
            S = 1, A = 4, D = 8
        };

        [[nodiscard]] Elements _getDailyHistoryPage() const noexcept;

        [[nodiscard]] uint32_t _calculateAverage(uint32_t numerator, uint32_t denumerator) const noexcept;

        std::wstring _stows(const std::string& string) const noexcept;

        bool _isThereActiveSession() noexcept;

        void _applyActiveSessionSettingsIfThereIs() noexcept;

        bool _closeActiveSessionIfPossible() noexcept;

    public:
        std::shared_ptr<mount::MainSystem> mainSystem;
        std::shared_ptr<CommonColor> commonColor;
        std::unique_ptr<backend::DayAndTime> dayAndTime;
        std::shared_ptr<MainPageSummaryInterface> interface;
        std::function<void()> on_quit = [] {};
        std::function<void(bool)> determineAddOrDelete;
        std::function<void()> openSummaries;
        std::function<void()> openCustomise;
        std::function<void()> openLanguage;
        std::function<void()> openReset;
        std::function<void()> openAttentionWindow;
        bool beginIsOn = false;
        bool maxTotalMinutesReached = false;
        bool maxSessionNumberReached = false;

    public:
        explicit MainPage(std::shared_ptr<LanguagePreference> languagePreference) noexcept;

        void adjustCommonRenderVariables() noexcept;

        Element generalLeftPlate() noexcept;

        Element renderMainPageWithHugePlate(const std::wstring &windowName, Element &&document) noexcept;

        Element renderMainPageWithHugePlate(const std::wstring &windowName, Elements clauses) noexcept;

        Element renderRightPlate(const std::wstring& minuteHeader, const std::wstring& sessionHeader) noexcept;

        Element renderMainPage() noexcept;

        Element renderDailySummaryPage() noexcept;

        Element renderWeeklySummaryPage() noexcept;

        Element renderMonthlySummaryPage() noexcept;

        Element renderYearlySummaryPage() noexcept;

        Element renderHelpPage() noexcept;

        Element renderAboutPage() noexcept;

        Element Render() noexcept final {
            if (_depth.pushedReturn && _depth.pageCode == 0b0001) {
                if (!interface->summaryIsSelected) { return renderMainPage(); }

                if ((interface->selectedSummary & 0b0001) && thereIsADailySummary) {
                    auto dailySummary = renderDailySummaryPage();
                    return std::move(dailySummary);
                }
                if ((interface->selectedSummary & 0b0010) && thereIsAWeeklySummary) {
                    auto weeklySummary = renderWeeklySummaryPage();
                    return std::move(weeklySummary);
                }
                if ((interface->selectedSummary & 0b0100) && thereIsAMonthlySummary) {
                    auto monthlySummary = renderMonthlySummaryPage();
                    return std::move(monthlySummary);
                }
                if ((interface->selectedSummary & 0b1000) && thereIsAYearlySummary) {
                    auto yearlySummary = renderYearlySummaryPage();
                    return std::move(yearlySummary);
                }
            }

            if (_depth.pushedReturn && _depth.pageCode == 0b0010) {
                return renderHelpPage();
            }
            if (_depth.pushedReturn && _depth.pageCode == 0b0100) {
                return renderAboutPage();
            }

            return renderMainPage();
        }

        bool OnEvent(Event event) noexcept override {
            if (event == Event::Escape) {
                _depth.pushedReturn = false;
                interface->summaryIsSelected = false;
                return Component::OnEvent(event);
            }

            if (event == Event::Character('U') || event == Event::Character('u')) {
                if (_cursor > 0) { --_cursor; }
            }
            else if (event == Event::Character('D') || event == Event::Character('d')) {
                if (_vectorSize <= _screenCapacity) {
                    return Component::OnEvent(event);
                }
                if (_cursor < (_vectorSize - _screenCapacity)) { ++_cursor; }
            }

            return Component::OnEvent(event);
        };
    };

    class TimeAccumulator : public Component {
    private:
        std::shared_ptr<LanguagePreference> _languagePreference;
        std::shared_ptr<CommonColor> _commonColor = std::make_shared<CommonColor>();
        std::shared_ptr<MainPageSummaryInterface> _interface = std::make_shared<MainPageSummaryInterface>();
        Container _container = Container::Tab(nullptr);
        std::unique_ptr<MainPage> _mainPage;
        std::unique_ptr<AddAndDelete> _addAndDelete;
        std::unique_ptr<Summaries> _summaries;
        std::unique_ptr<Customise> _customise;
        std::unique_ptr<Language> _language;
        std::unique_ptr<Reset> _reset;
        std::unique_ptr<AttentionWindow> _attentionWindow;

    public:
        std::function<void()> on_quit = [] {};

        explicit TimeAccumulator(std::shared_ptr<mount::MainSystem> mainSystem) noexcept;

        Element Render() noexcept final {
            Element document = _mainPage->Render();
            if (_addAndDelete->Focused()) {
                document = dbox({
                                        document,
                                        _addAndDelete->Render()
                                        | clear_under | center,
                                });
            }
            else if (_summaries->Focused()) {
                document = dbox({
                                        document,
                                        _summaries->Render()
                                        | clear_under | center,
                                });
            }
            else if (_customise->Focused()) {
                document = dbox({
                                        document,
                                        _customise->Render()
                                        | clear_under | center,
                                });
            }
            else if (_language->Focused()) {
                document = dbox({
                                        document,
                                        _language->Render()
                                        | clear_under | center,
                                });
            }
            else if (_reset->Focused()) {
                document = dbox({
                                        document,
                                        _reset->Render()
                                        | clear_under | center,
                                });
            }
            else if (_attentionWindow->Focused()) {
                document = dbox({
                                        document,
                                        _attentionWindow->Render()
                                        | clear_under | center,
                                });
            }

            return document;
        }
    };
}

#endif //TUIGALLERY_LIVEAPPLICATION_H
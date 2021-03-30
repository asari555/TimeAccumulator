#include "liveApplication.h"

uint8_t commonLanguage;

// @detail A digit is filled with zero, if given number does not
// have enough digits.
std::wstring liveApplication::digitsToWstring(uint8_t number) noexcept {
    if (number < 10) {
        return std::wstring(1, '0').append(std::to_wstring(number));
    }

    return std::to_wstring(number);
}

// @detail A digit is filled with zero, if given number does not
// have enough digits.
std::wstring liveApplication::fourDigitsToWstring(uint16_t number) noexcept {
    if (number < 10) {
        return std::wstring(3, '0').append(std::to_wstring(number));
    }
    if (number < 100) {
        return std::wstring(2, '0').append(std::to_wstring(number));
    }
    if (number < 1000) {
        return std::wstring(1, '0').append(std::to_wstring(number));
    }

    return std::to_wstring(number);
}

/// for very beginning of program after the assignment of local
/// mainSystem.
void liveApplication::CommonColor::adjustColorPair() noexcept {
    auto settings = mainSystem->getSettings();
    if (settings == nullptr) {
        setDefaultColorPair();
        return;
    }
    textColor = textColorList[settings->textColor];
    backgroundColor = backgroundColorList[settings->backgroundColor];
}

/// just keeps and carries default color pair.
void liveApplication::CommonColor::setDefaultColorPair() noexcept {
    textColor = defaultTextColor;
    backgroundColor = defaultBackgroundColor;
}

/// is used when default button is pushed on customise page.
void liveApplication::CommonColor::choseDefaultColorPair() noexcept {
    setDefaultColorPair();
    mainSystem->setColorAdjustment(DEFAULT_TEXT_COLOR, DEFAULT_BACKGROUND_COLOR);
}

/// makes changing of color pair permanent.
void liveApplication::CommonColor::savePreferredColorPair() const noexcept {
    uint16_t textColorCode = 0;
    uint16_t backgroundColorCode = 0;

    for (uint8_t t = 0; t < textColorListSize; ++t) {
        if (textColor == textColorList[t]) {
            textColorCode = t;
            break;
        }
    }

    for (uint8_t b = 0; b < backgroundColorListSize; ++b) {
        if (backgroundColor == backgroundColorList[b]) {
            backgroundColorCode = b;
            break;
        }
    }
    mainSystem->setColorAdjustment(textColorCode, backgroundColorCode);
}

[[nodiscard]] bool liveApplication::CommonColor::isThereSavedSettings() const noexcept {
    auto settings = mainSystem->getSettings();
    return settings != nullptr;
}

[[nodiscard]] bool liveApplication::AddAndDelete::_isInputAPositiveInteger(const std::wstring &input) const noexcept {
    return std::ranges::all_of(input.begin(), input.end(),[&](const auto &character) {
        return character >= '0' && character <= '9';
    });
}

/// for AddAndDelete class
[[nodiscard]] bool liveApplication::AddAndDelete::_isInputValid(
        const std::wstring &input,
        const bool &mode
        ) const noexcept {

    if (!_isInputAPositiveInteger(input)) return false;

    uint16_t minute = std::stoi(input);
    uint16_t currentTotalDailyMinutes = mainSystem->getTotalMinutes();
    uint16_t inputMinuteLimit;
    /// if add
    if (mode) {
        inputMinuteLimit = MAXIMUM_MINUTES_IN_A_DAY - currentTotalDailyMinutes;
    }
    /// else if delete
    else inputMinuteLimit = currentTotalDailyMinutes;

    return minute <= inputMinuteLimit;
}

liveApplication::AddAndDelete::AddAndDelete(std::shared_ptr<LanguagePreference> languagePreference) noexcept {

    _languagePreference = std::move(languagePreference);
    Add(&_container);
    _container.Add(&_input);
    _input.placeholder = _languagePreference->getRequiredWord(0);
    _input.on_enter = [this] {
        if (_input.content.empty()) {
            closeWindow();
            return;
        }

        if (!_isInputValid(_input.content, addOrDelete)) { return; }
        enteredMinute = std::stoi(_input.content);

        if (addOrDelete) {
            mainSystem->doWhatCommandIs(addCommand, enteredMinute);
        }
        else {
            mainSystem->doWhatCommandIs(deleteCommand, enteredMinute);
        }

        _input.content = L"";
        closeWindow();
    };
}

liveApplication::Summaries::Summaries(std::shared_ptr<LanguagePreference> languagePreference) noexcept {

    _languagePreference = std::move(languagePreference);
    Add(&_container);
    _summaries.entries = {
            _languagePreference->getRequiredWord(3),
            _languagePreference->getRequiredWord(4),
            _languagePreference->getRequiredWord(5),
            _languagePreference->getRequiredWord(6)
    };

    _container.Add(&_summaries);
    _summaries.on_enter = [this] {
        interface->selectedSummary = ROOT;
        interface->selectedSummary <<= _summaries.selected;
        interface->summaryIsSelected = true;
        closeWindow();
    };
}

void liveApplication::Customise::_assignPreviousCommonColor() noexcept {
    if (_initialColorsWereTaken) { return; }

    previousCommonColor.textColor = commonColor->textColor;
    previousCommonColor.backgroundColor = commonColor->backgroundColor;
    _initialColorsWereTaken = true;
}

liveApplication::Customise::Customise(std::shared_ptr<LanguagePreference> languagePreference) noexcept {

    _languagePreference = std::move(languagePreference);
    _defaultButton = Button(_languagePreference->getRequiredWord(8));
    _saveButton = Button(_languagePreference->getRequiredWord(9));
    Add(&_container);
    _container.Add(&_textColors);
    _container.Add(&_backgroundColors1);
    _container.Add(&_backgroundColors2);
    _buttonContainer.Add(&_defaultButton);
    _buttonContainer.Add(&_saveButton);
    _container.Add(&_buttonContainer);

    _textColors.entries = {
            _languagePreference->getRequiredWord(10),
            _languagePreference->getRequiredWord(11),
            _languagePreference->getRequiredWord(12),
            _languagePreference->getRequiredWord(13),
            _languagePreference->getRequiredWord(14),
            _languagePreference->getRequiredWord(15),
            _languagePreference->getRequiredWord(16),
            _languagePreference->getRequiredWord(17),
            _languagePreference->getRequiredWord(18),
            _languagePreference->getRequiredWord(19),
            _languagePreference->getRequiredWord(20),
            _languagePreference->getRequiredWord(21),
            _languagePreference->getRequiredWord(22),
            _languagePreference->getRequiredWord(23),
            _languagePreference->getRequiredWord(24),
            _languagePreference->getRequiredWord(25),
    };

    _backgroundColors1.entries = {
            _languagePreference->getRequiredWord(26),
            _languagePreference->getRequiredWord(27),
            _languagePreference->getRequiredWord(28),
            _languagePreference->getRequiredWord(29),
            _languagePreference->getRequiredWord(30),
            _languagePreference->getRequiredWord(31),
            _languagePreference->getRequiredWord(32),
            _languagePreference->getRequiredWord(33),
            _languagePreference->getRequiredWord(34),
            _languagePreference->getRequiredWord(35),
            _languagePreference->getRequiredWord(36),
            _languagePreference->getRequiredWord(37),
            _languagePreference->getRequiredWord(38),
            _languagePreference->getRequiredWord(39),
            _languagePreference->getRequiredWord(40),
            _languagePreference->getRequiredWord(41)
    };

    _backgroundColors2.entries = {
            _languagePreference->getRequiredWord(42),
            _languagePreference->getRequiredWord(43),
            _languagePreference->getRequiredWord(44),
            _languagePreference->getRequiredWord(45),
            _languagePreference->getRequiredWord(46),
            _languagePreference->getRequiredWord(47),
            _languagePreference->getRequiredWord(48),
            _languagePreference->getRequiredWord(49),
            _languagePreference->getRequiredWord(50),
            _languagePreference->getRequiredWord(51),
            _languagePreference->getRequiredWord(52),
            _languagePreference->getRequiredWord(53),
            _languagePreference->getRequiredWord(54),
            _languagePreference->getRequiredWord(55)
    };

    _textColors.on_enter = [this] {
        _aButtonWasPushed = false;
        _assignPreviousCommonColor();
        /// setting text color
        commonColor->textColor = commonColor->textColorList[_textColors.selected];
    };
    _backgroundColors1.on_enter = [this] {
        _aButtonWasPushed = false;
        _assignPreviousCommonColor();
        /// setting background color from the first list
        commonColor->backgroundColor = commonColor->backgroundColorList[_backgroundColors1.selected];
    };
    _backgroundColors2.on_enter = [this] {
        _aButtonWasPushed = false;
        _assignPreviousCommonColor();
        /// setting background color from the second list
        commonColor->backgroundColor =
                commonColor->backgroundColorList[_backgroundColors2.selected + BACKGROUND_COLOR_MENU_OFFSET];
    };

    _defaultButton.on_click = [&] {
        commonColor->choseDefaultColorPair();
        _initialColorsWereTaken = false;
        _assignPreviousCommonColor();
        _textColors.selected = 0;
        _backgroundColors1.selected = 0;
        _backgroundColors2.selected = 0;
        _aButtonWasPushed = true;
    };
    _saveButton.on_click = [&] {
        commonColor->savePreferredColorPair();
        _initialColorsWereTaken = false;
        _assignPreviousCommonColor();
        _textColors.selected = 0;
        _backgroundColors1.selected = 0;
        _backgroundColors2.selected = 0;
        _aButtonWasPushed = true;
    };
}

[[nodiscard]] bool liveApplication::Language::_areSettingsEmpty() const noexcept {
    auto settings = _mainSystem->getSettings();
    return settings == nullptr;
}

void liveApplication::Language::_setLanguageSetting(uint8_t preference) const noexcept {
    _mainSystem->setLanguageAdjustment(preference);
}

void liveApplication::Language::_applyDefaultIfPossible() const noexcept {
    if (!_areSettingsEmpty()) { return; }
    /// Default is Turkish
    _setLanguageSetting(DEFAULT_LANGUAGE);
    commonLanguage = DEFAULT_LANGUAGE;
}

void liveApplication::Language::_getLanguageSetting(uint8_t& language) const noexcept {
    if (_areSettingsEmpty()) { return; }
    auto settings = _mainSystem->getSettings();
    language = settings->language;
}

/// returns false in the case of the same preferences.
[[nodiscard]] bool liveApplication::Language::_compareCurrentPreferenceWithPreviousOne(
        const int& selected
        ) const noexcept {

    uint8_t languageForControl;
    _getLanguageSetting(languageForControl);

    return languageForControl != selected;
}

/// Default is Turkish.
[[nodiscard]] const std::wstring& liveApplication::Language::_printWindowLabel() const noexcept {
    return _requiredSentences[commonLanguage];
}

/// Default is Turkish.
[[nodiscard]] const std::wstring& liveApplication::Language::_printInformativeText() const noexcept {
    return _requiredSentences[commonLanguage + 2];
}

liveApplication::Language::Language(std::shared_ptr<mount::MainSystem> mainSystem) noexcept {
    _mainSystem = std::move(mainSystem);
    _applyDefaultIfPossible();
    _getLanguageSetting(commonLanguage);

    Add(&_container);
    _container.Add(&_language);
    _language.entries = {L"Türkçe", L"English"};
}

liveApplication::Reset::Reset(std::shared_ptr<LanguagePreference> languagePreference) noexcept {

    _languagePreference = std::move(languagePreference);
    _noButton = Button(_languagePreference->getRequiredWord(59));
    _yesButton = Button(_languagePreference->getRequiredWord(60));
    Add(&_container);
    _container.Add(&_noButton);
    _container.Add(&_yesButton);

    _noButton.on_click = [this] {
        closeWindow();
    };
    _yesButton.on_click = [this] {
        mainSystem->reset();
        commonColor->setDefaultColorPair();
        closeWindow();
    };
}

liveApplication::AttentionWindow::AttentionWindow(std::shared_ptr<LanguagePreference> languagePreference) noexcept {
    _languagePreference = std::move(languagePreference);
    _okButton = Button(_languagePreference->getRequiredWord(63));

    Add(&_container);
    _container.Add(&_okButton);

    _okButton.on_click = [this] {
        closeWindow();
    };
}

[[nodiscard]] Elements liveApplication::MainPage::_getDailyHistoryPage() const noexcept {

    auto dailyReport = std::move(mainSystem->getDailyReport());
    Elements dailyHistory = { text(_languagePreference->getRequiredWord(67)) };

    if (dailyReport == nullptr) {
        return std::move(dailyHistory);
    }

    dailyHistory = {};
    std::wstring reportDatumHeader;
    uint16_t countSessions = 0;
    for (const auto& reportDatum : *(dailyReport)) {

        ++countSessions;
        Element tempElement;
        if (reportDatum.commandCode & S) {
            reportDatumHeader = _languagePreference->getRequiredWord(68);

            tempElement = hbox( text(digitsToWstring(countSessions)), text(L" -> "), text(reportDatumHeader),
                                text(digitsToWstring(reportDatum.hour)), text(L":"),
                                text(digitsToWstring(reportDatum.minute)), text(L" - "),
                                text(fourDigitsToWstring(reportDatum.totalMinute)),
                                text(_languagePreference->getRequiredWord(69)),
                                text(digitsToWstring(reportDatum.remainingSecond)),
                                text(_languagePreference->getRequiredWord(70))
                                );

            dailyHistory.push_back(tempElement);
            continue;
        }
        else if (reportDatum.commandCode & A) {
            reportDatumHeader = _languagePreference->getRequiredWord(71);
        }
        else if (reportDatum.commandCode & D) {
            reportDatumHeader = _languagePreference->getRequiredWord(72);
        }

        tempElement = hbox(
                            text(digitsToWstring(countSessions)),
                            text(L" -> "),
                            text(reportDatumHeader),
                            text(digitsToWstring(reportDatum.hour)),
                            text(L":"),
                            text(digitsToWstring(reportDatum.minute)),
                            text(L" - "),
                            text(fourDigitsToWstring(reportDatum.totalMinute)),
                            text(_languagePreference->getRequiredWord(73))
                            );

        dailyHistory.push_back(tempElement);
    }

    return std::move(dailyHistory);
}

[[nodiscard]] uint32_t liveApplication::MainPage::_calculateAverage(
        uint32_t numerator,
        uint32_t denumerator
        ) const noexcept {

    return numerator / denumerator;
}

/// string to wstring
std::wstring liveApplication::MainPage::_stows(const std::string& string) const noexcept {
    std::wstring tempWString;
    for (const auto& character : string) {
        tempWString.push_back(character);
    }
    return std::move(tempWString);
}

bool liveApplication::MainPage::_isThereActiveSession() noexcept {
    _activeSession = std::move(mainSystem->getActiveSession());
    return _activeSession != nullptr;
}

void liveApplication::MainPage::_applyActiveSessionSettingsIfThereIs() noexcept {
    if(!_isThereActiveSession()) { return; }
    beginIsOn = true;
}

/// returns false if there is no active session.
bool liveApplication::MainPage::_closeActiveSessionIfPossible() noexcept {
    if (!_isThereActiveSession()) { return false; }
    _accumulatedMinutes = mainSystem->calculateAccumulatedTime(*_activeSession);

    if (!mainSystem->doesLastDailyDataBelongToToday()) { return true; }
    if (!mainSystem->willBeTotalMinutesAtLimit(_accumulatedMinutes, _totalMinutes)) { return true; }
    mainSystem->doWhatCommandIs(endCommand);
    _activeSession = nullptr;
    _depth.pushedReturn = false;
    beginIsOn = false;
    return false;
}

liveApplication::MainPage::MainPage(std::shared_ptr<LanguagePreference> languagePreference) noexcept {
    _languagePreference = std::move(languagePreference);
    Add(&_container);
    _commandMenu.entries = {
            _languagePreference->getRequiredWord(74),
            _languagePreference->getRequiredWord(75),
            _languagePreference->getRequiredWord(76),
            _languagePreference->getRequiredWord(77),
            _languagePreference->getRequiredWord(78)
    };
    _appendixMenu.entries = {
            _languagePreference->getRequiredWord(79),
            _languagePreference->getRequiredWord(80),
            _languagePreference->getRequiredWord(81),
            _languagePreference->getRequiredWord(82),
            _languagePreference->getRequiredWord(83),
            _languagePreference->getRequiredWord(84)
    };
    _container.Add(&_commandMenu);
    _container.Add(&_appendixMenu);

    _commandMenu.on_enter = [this] {
        bool deleteSelected = _commandMenu.selected == 3;
        if (_totalMinutes == MAXIMUM_MINUTES_IN_A_DAY && !maxTotalMinutesReached) {
            maxTotalMinutesReached = true;
        }

        if (_totalSessions == MAX_SESSION_NUMBER && deleteSelected) {
            maxSessionNumberReached = true;
        }

        if (_commandMenu.selected == 0) {
            if (maxTotalMinutesReached) {
                openAttentionWindow();
            }
            else if (maxSessionNumberReached) { /// max min is reached
                openAttentionWindow();
            }
            else if (beginIsOn) {
                openAttentionWindow();
            }
            else {
                _depth.pushedReturn = false;
                beginIsOn = true;
                mainSystem->doWhatCommandIs(beginCommand);
                _activeSession = std::move(mainSystem->getActiveSession());
            }
        }
        else if (_commandMenu.selected == 1) {
            if (!beginIsOn) {
                openAttentionWindow();
            }
            else {
                _depth.pushedReturn = false;
                beginIsOn = false;
                mainSystem->doWhatCommandIs(endCommand);
            }
        }

        if (_commandMenu.selected == 2) {
            if (maxTotalMinutesReached) {
                openAttentionWindow();
            }
            else if (maxSessionNumberReached) {
                openAttentionWindow();
            }
            else if (beginIsOn) {
                openAttentionWindow();
            }
            else {
                _depth.pushedReturn = false;
                determineAddOrDelete(true); // Add
            }
        }
        else if (deleteSelected) {
            if (maxTotalMinutesReached) {
                openAttentionWindow();
            }
            else if (maxSessionNumberReached) {
                openAttentionWindow();
            }
            else if (beginIsOn) {
                openAttentionWindow();
            }
            else {
                _depth.pushedReturn = false;
                determineAddOrDelete(false); // Delete
            }
        }
        else if (_commandMenu.selected == 4) {
            _depth.pageCode = 0b0001;
            _depth.pushedReturn = true;
            interface->selectedSummary = false;
            openSummaries();
        }
    };
    _appendixMenu.on_enter = [this] {
        if (_appendixMenu.selected == 0) {
            _depth.pushedReturn = false;
            openCustomise();
        }
        else if (_appendixMenu.selected == 1) {
            _depth.pushedReturn = false;
            openLanguage();
        }
        else if (_appendixMenu.selected == 2) {
            _depth.pageCode = 0b0010;
            _depth.pushedReturn = true;
        }
        else if (_appendixMenu.selected == 3) {
            _depth.pageCode = 0b0100;
            _depth.pushedReturn = true;
        }
        else if (_appendixMenu.selected == 4) {
            if (beginIsOn) {
                openAttentionWindow();
            }
            else {
                _depth.pushedReturn = false;
                openReset();
            }
        }
        else if (_appendixMenu.selected == 5) {
            isThreadEnded.notify_all();
            if (beginIsOn) {
                mainSystem->doWhatCommandIs(endCommand);
            }
            on_quit();
        }
    };
}

void liveApplication::MainPage::adjustCommonRenderVariables() noexcept {
    _currentPageSize = Terminal::Size();
    _pageWidth = _currentPageSize.dimx;
    _pageHeight = _currentPageSize.dimy;
    _mainPartWidth = (int) (_pageWidth * 0.20);

    maxTotalMinutesReached = mainSystem->isTotalMinutesAtLimit();
    _applyActiveSessionSettingsIfThereIs();
    _closeActiveSessionIfPossible();
}

/// @details Each mainPage uses the leftPlate.
Element liveApplication::MainPage::generalLeftPlate() noexcept {
    auto leftPlate = vbox({ window(text(_languagePreference->getRequiredWord(85)),
                            _commandMenu.Render() | center) | flex | size(WIDTH, EQUAL, _mainPartWidth),
                            window(text(_languagePreference->getRequiredWord(86)),
                                   _appendixMenu.Render() | center) | flex | size(WIDTH, EQUAL, _mainPartWidth),
                           }) | size(HEIGHT, EQUAL, _pageHeight)
                              | color(commonColor->textColor)
                              | bgcolor(commonColor->backgroundColor);

    return leftPlate;
}

/// provides empty plate to fill with data as summaries.
Element liveApplication::MainPage::renderMainPageWithHugePlate(
        const std::wstring &windowName,
        Element &&document
        ) noexcept {

    adjustCommonRenderVariables();
    _remainingPartWidth1 = (int) (_pageWidth * 0.80);

    auto leftPlate = generalLeftPlate();
    auto hugePlate = window(text(windowName), std::move(document))
                        | size(WIDTH, EQUAL, _remainingPartWidth1)
                        | color(commonColor->textColor)
                        | bgcolor(commonColor->backgroundColor);

    auto cluster = hbox({leftPlate, hugePlate}) | clear_under;

    return cluster;
}

/// provides empty plate for text and paragraph.
Element liveApplication::MainPage::renderMainPageWithHugePlate(
        const std::wstring &windowName,
        Elements clauses
        ) noexcept {

    adjustCommonRenderVariables();
    _remainingPartWidth1 = (int) (_pageWidth * 0.80);

    auto leftPlate = generalLeftPlate();
    auto hugePlate = window(text(windowName), vbox(std::move(clauses)))
                        | size(WIDTH, EQUAL, _remainingPartWidth1)
                        | color(commonColor->textColor)
                        | bgcolor(commonColor->backgroundColor);
    auto cluster = hbox({leftPlate, hugePlate}) | clear_under;

    return cluster;
}

Element liveApplication::MainPage::renderRightPlate(
        const std::wstring& minuteHeader,
        const std::wstring& sessionHeader
        ) noexcept {

    _weekDay = _languagePreference->getWeekDayName(mainSystem->getDayOfProgramStart());
    _totalMinutes = mainSystem->getTotalMinutes();
    _totalSessions = mainSystem->getDailyTotalSessionNumber();
    bool thereIsAnActiveSession = beginIsOn && _activeSession != nullptr;
    _accumulatedMinutes = 0;

    /// Comparing To window adjustments
    auto lastDailySummary = mainSystem->getLatestDailySummary(thereIsADailySummary);
    auto lastWeeklySummary = mainSystem->getLatestWeeklySummary(thereIsAWeeklySummary);

    int dailyDifference = 0;
    int weeklyDifference = 0;

    thereIsAnActiveSession = _closeActiveSessionIfPossible();

    Element previousDay;
    if (thereIsADailySummary) {
        dailyDifference = _accumulatedMinutes + _totalMinutes - lastDailySummary->dailyTotalMinutes;

        previousDay = vbox({
                            text(_languagePreference->getRequiredWord(87)),
                            hbox({ text(std::to_wstring(dailyDifference)) | bold,
                                text(_languagePreference->getRequiredWord(88)) }) | center
                            });
    }

    Element previousWeekAverage;
    if (thereIsAWeeklySummary) {
        auto weeklyAverage = _calculateAverage(lastWeeklySummary->weeklyTotalMinutes, DAY_NUMBER_IN_A_WEEK);

        weeklyDifference = _accumulatedMinutes + _totalMinutes - weeklyAverage;

        previousWeekAverage = vbox({ text(_languagePreference->getRequiredWord(89)),
                                     hbox({ text(std::to_wstring(weeklyDifference)) | bold,
                                            text(_languagePreference->getRequiredWord(88)) })| center
                                    });
    }

    Element comparing;
    if (thereIsADailySummary && thereIsAWeeklySummary) {
        comparing = window(
                            text(_languagePreference->getRequiredWord(90)),
                            vbox({std::move(previousDay),
                            std::move(previousWeekAverage)})
                            );
    }
    else if (thereIsADailySummary) {
        comparing = window(text(_languagePreference->getRequiredWord(90)), std::move(previousDay));
    }
    else if (thereIsAWeeklySummary) {
        comparing = window(
                            text(_languagePreference->getRequiredWord(90)),
                            std::move(previousWeekAverage)
                           );
    }


    /// Program Start window adjustments
    Element programStart = vbox({ hbox( text(_languagePreference->getRequiredWord(91)),
                                  text(_weekDay),
                                  text(L", "),
                                  hbox( text(digitsToWstring(dayAndTime->day)),
                                        text(L"/"),
                                        text(digitsToWstring(dayAndTime->month)),
                                        text(L"/"),
                                        text(std::to_wstring(dayAndTime->year)))),
                                  hbox( text(_languagePreference->getRequiredWord(92)),
                                        text(digitsToWstring(dayAndTime->hour)),
                                        text(L":"),
                                        text(digitsToWstring(dayAndTime->minute))),
                                  hbox( text(_languagePreference->getRequiredWord(93)),
                                        text(std::to_wstring(dayAndTime->abstractYear))),
                                  }) | flex_grow;

    bool thereIsComparison = thereIsADailySummary || thereIsAWeeklySummary;

    /// Right Plane adjustments
    Element informationWithBeginOrNot;
    if (thereIsAnActiveSession) {

        Element subActiveSession = hbox( text(_languagePreference->getRequiredWord(94)),
                                         hbox( text(digitsToWstring(_activeSession->hour)),
                                               text(L":"),
                                               text(digitsToWstring(_activeSession->minute))
                                              ) | bold
                                        ) | center;

        if (thereIsComparison) {
            informationWithBeginOrNot = vbox({ window(text(_languagePreference->getRequiredWord(95)),
                                               vbox({ std::move(subActiveSession),
                                                      hbox( text(std::to_wstring(_accumulatedMinutes)) | bold,
                                                           text(_languagePreference->getRequiredWord(96)))
                                                           | center
                                               })),
                                               comparing,
                                               window(text(_languagePreference->getRequiredWord(97)),
                                                      std::move(programStart))
                                        });
        }
        else {
            informationWithBeginOrNot = vbox({ window(text(_languagePreference->getRequiredWord(95)),
                                               vbox({ std::move(subActiveSession),
                                                      hbox( text(std::to_wstring(_accumulatedMinutes)) | bold,
                                                            text(_languagePreference->getRequiredWord(96)))
                                                            | center
                                               })),
                                               window(text(_languagePreference->getRequiredWord(97)),
                                                      std::move(programStart))
                                        });
        }
    }
    else {
        if (thereIsComparison) {
            informationWithBeginOrNot = vbox({
                comparing,
                window(text(_languagePreference->getRequiredWord(97)), std::move(programStart))
            });
        }
        else {
            informationWithBeginOrNot = vbox({
                window(text(_languagePreference->getRequiredWord(97)), std::move(programStart))
            });
        }
    }

    return vbox({
                hbox(
                     window(text(minuteHeader),text(std::to_wstring(_totalMinutes)) | bold | center)
                         | flex | size(WIDTH, EQUAL, _remainingPartWidth2),
                     window(text(sessionHeader),text(std::to_wstring(_totalSessions)) | bold | center)
                         | flex | size(WIDTH, EQUAL, _remainingPartWidth2)
                ) | size(HEIGHT, EQUAL, _specialPageHeightForMainPage),
                std::move(informationWithBeginOrNot) | size(HEIGHT, EQUAL, _remainingPageHeight)
                | size(WIDTH, EQUAL, _remainingPartWidth3)
            }) | color(commonColor->textColor)
               | bgcolor(commonColor->backgroundColor);
}

/// the last function to illustrate Main page on the screen.
Element liveApplication::MainPage::renderMainPage() noexcept {
    adjustCommonRenderVariables();
    _remainingPartWidth1 = (int) (_pageWidth * 0.4); /// 40%
    _remainingPartWidth2 = (int) (_pageWidth * 0.2); /// 20%
    _remainingPartWidth3 = (int) (_pageWidth * 0.4); /// 40%
    _specialPageHeightForMainPage = (int) (_pageHeight * 0.4); /// 40%
    _remainingPageHeight = _pageHeight - _specialPageHeightForMainPage;

    auto leftPlate = generalLeftPlate();

    Elements dailyReportScreenView;

    auto dailyHistory = std::move(_getDailyHistoryPage());
    _vectorSize = dailyHistory.size();
    _screenCapacity = _pageHeight - TWO_ROWS;

    bool scrollablePage = _vectorSize > _screenCapacity;
    if (scrollablePage) {
        Element threeDotsTheFirst = text(L".   .   .") | center;
        Element threeDotsTheLast = text(L".   .   .") | center;
        bool firstThreeDots = false, lastThreeDots = false;
        uint16_t cursorMaxValue = _vectorSize - _screenCapacity;

        if (_cursor == 0) { lastThreeDots = true; }
        else if (_cursor == cursorMaxValue) { firstThreeDots = true; }
        else { firstThreeDots = true; lastThreeDots = true; }

        if (firstThreeDots) {
            dailyReportScreenView.push_back(std::move(threeDotsTheFirst));
        }

        uint16_t upperLimit = _cursor + _screenCapacity;
        if (upperLimit > _vectorSize) {
            upperLimit = _vectorSize;
        }
        uint16_t indexAtTheEnd = upperLimit - INDEX_OFFSET_ONE;

        for (uint16_t i = _cursor; i < upperLimit; ++i) {
            if (i != 0 && firstThreeDots) {
                firstThreeDots = false;
                continue;
            }
            if (i == indexAtTheEnd && lastThreeDots) { break; }
            dailyReportScreenView.push_back(std::move(dailyHistory.at(i)));
        }

        if (lastThreeDots) {
            dailyReportScreenView.push_back(std::move(threeDotsTheLast));
        }
    }
    else { dailyReportScreenView = std::move(dailyHistory); }

    auto midPlate =
            window(text(_languagePreference->getRequiredWord(98)),
                   vbox(std::move(dailyReportScreenView)))
            | flex | size(WIDTH, EQUAL, _remainingPartWidth1)
            | size(HEIGHT, EQUAL, _pageHeight)
            | color(commonColor->textColor)
            | bgcolor(commonColor->backgroundColor);

    std::wstring minuteHeader;
    std::wstring sessionHeader;
    if (_pageWidth > 90) {
        minuteHeader = _languagePreference->getRequiredWord(99);
        sessionHeader = _languagePreference->getRequiredWord(100);
    } else {
        minuteHeader = _languagePreference->getRequiredWord(101);
        sessionHeader = _languagePreference->getRequiredWord(102);
    }

    return hbox({leftPlate, midPlate, renderRightPlate(minuteHeader, sessionHeader)}) | clear_under;
}

/// renders last daily summary data for now.
Element liveApplication::MainPage::renderDailySummaryPage() noexcept {
    auto lastDailySummary = mainSystem->getLatestDailySummary(thereIsADailySummary);

    Element dailySummary;
    if (thereIsADailySummary) {
        auto day = lastDailySummary->day;
        auto month = lastDailySummary->month;
        uint8_t abstractYear = lastDailySummary->abstractYear;
        uint16_t year = lastDailySummary->year;
        uint16_t totalMinutes = lastDailySummary->dailyTotalMinutes;
        uint16_t totalSessionNumber = lastDailySummary->dailyTotalSessionNumber;
        std::wstring weekDay = _languagePreference->getWeekDayName(lastDailySummary->weekDay);

        std::wstring date =
                weekDay + L", " +
                std::to_wstring(day) + L"/" +
                std::to_wstring(month) + L"/" +
                std::to_wstring(year);

        uint16_t averageMinutePerSession = totalMinutes / totalSessionNumber;

        dailySummary =
                vbox({
                    window(text(date),
                           vbox({
                               hbox({
                                   text(_languagePreference->getRequiredWord(103)),
                                   text(std::to_wstring(totalMinutes))
                               }),
                               hbox({
                                   text(_languagePreference->getRequiredWord(104)),
                                   text(std::to_wstring(totalSessionNumber))
                               }),
                               hbox({
                                   text(_languagePreference->getRequiredWord(105)),
                                   text(std::to_wstring(averageMinutePerSession) +
                                           _languagePreference->getRequiredWord(106))
                               }),
                               hbox({
                                   text(_languagePreference->getRequiredWord(107)),
                                   text(std::to_wstring(abstractYear))
                               })
                           })
                    )
                });

        return renderMainPageWithHugePlate(
                _languagePreference->getRequiredWord(108),
                std::move(dailySummary)
                );
    }

    return std::move(dailySummary);
}

/// renders last weekly summary data for now.
Element liveApplication::MainPage::renderWeeklySummaryPage() noexcept {
    auto lastWeeklySummary = mainSystem->getLatestWeeklySummary(thereIsAWeeklySummary);

    Element weeklySummary;
    if (thereIsAWeeklySummary) {
        uint8_t firstDay = lastWeeklySummary->firstDay;
        uint8_t firstMonth = lastWeeklySummary->firstMonth;
        uint8_t lastDay = lastWeeklySummary->firstDay;
        uint8_t lastMonth = lastWeeklySummary->lastMonth;
        uint8_t abstractYear = lastWeeklySummary->abstractYear;
        uint16_t firstYear = lastWeeklySummary->firstYear;
        uint16_t lastYear = lastWeeklySummary->lastYear;
        uint16_t totalMinutes = lastWeeklySummary->weeklyTotalMinutes;
        uint16_t totalSessionNumber = lastWeeklySummary->weeklyTotalSessionNumber;
        std::wstring weekName = _stows(lastWeeklySummary->weekName);

        _languagePreference->modifyWeekName(weekName);

        std::wstring firstDate = std::to_wstring(firstDay) + L"/" +
                                 std::to_wstring(firstMonth) + L"/" +
                                 std::to_wstring(firstYear);
        std::wstring lastDate = std::to_wstring(lastDay) + L"/" +
                                std::to_wstring(lastMonth) + L"/" +
                                std::to_wstring(lastYear);

        uint16_t averageMinutePerSession = totalMinutes / totalSessionNumber;
        uint16_t averageMinutePerDay = totalMinutes / DAY_NUMBER_IN_A_WEEK;

        weeklySummary =
                vbox({
                      window(
                             text(weekName),
                             vbox({
                                 hbox({
                                     text(_languagePreference->getRequiredWord(109)),
                                     text(firstDate),
                                     text(L" - "),
                                     text(lastDate)
                                 }),
                                 hbox({
                                     text(_languagePreference->getRequiredWord(110)),
                                     text(std::to_wstring(totalMinutes))
                                 }),
                                 hbox({
                                     text(_languagePreference->getRequiredWord(111)),
                                     text(std::to_wstring(totalSessionNumber))
                                 }),
                                 text(_languagePreference->getRequiredWord(112)),
                                 hbox({
                                     text(_languagePreference->getRequiredWord(113)),
                                     text(std::to_wstring(averageMinutePerDay) +
                                          _languagePreference->getRequiredWord(114))
                                 }),
                                 hbox({
                                     text(_languagePreference->getRequiredWord(113)),
                                     text(std::to_wstring(averageMinutePerSession) +
                                             _languagePreference->getRequiredWord(115))
                                 }),
                                 hbox({
                                     text(_languagePreference->getRequiredWord(116)),
                                     text(std::to_wstring(abstractYear))
                                 })
                             })
                      )
                });

        return renderMainPageWithHugePlate(
                _languagePreference->getRequiredWord(117),
                std::move(weeklySummary)
                );
    }

    return std::move(weeklySummary);
}

/// renders last monthly summary data for now.
Element liveApplication::MainPage::renderMonthlySummaryPage() noexcept {
    auto lastMonthlySummary = mainSystem->getLatestMonthlySummary(thereIsAMonthlySummary);

    Element monthlySummary;
    if (thereIsAMonthlySummary) {
        uint8_t monthNumber = lastMonthlySummary->monthNumber;
        uint8_t abstractYear = lastMonthlySummary->abstractYear;
        uint16_t totalMinutes = lastMonthlySummary->monthlyTotalMinutes;
        uint16_t totalSessionNumber = lastMonthlySummary->monthlyTotalSessionNumber;
        std::wstring firstWeekName = _stows(lastMonthlySummary->firstWeek);
        std::wstring lastWeekName = _stows(lastMonthlySummary->lastWeek);

        std::wstring monthName = _languagePreference->getRequiredWord(118) + std::to_wstring(monthNumber);

        uint16_t averageMinutesPerSession = totalMinutes / totalSessionNumber;
        uint16_t averageMinutesPerWeek = totalMinutes / WEEK_NUMBER_IN_A_MONTH;
        uint16_t averageMinutesPerDay = totalMinutes / DAY_NUMBER_IN_A_MONTH;

        monthlySummary =
                vbox({
                    window(
                            text(monthName),
                            vbox({
                                hbox({
                                    text(_languagePreference->getRequiredWord(119)),
                                    text(firstWeekName),
                                    text(L" - "),
                                    text(lastWeekName)
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(120)),
                                    text(std::to_wstring(totalMinutes))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(121)),
                                    text(std::to_wstring(totalSessionNumber))
                                }),
                                text(_languagePreference->getRequiredWord(122)),
                                hbox({
                                    text(_languagePreference->getRequiredWord(123)),
                                    text(std::to_wstring(averageMinutesPerWeek) +
                                         _languagePreference->getRequiredWord(124))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(123)),
                                    text(std::to_wstring(averageMinutesPerDay) +
                                            _languagePreference->getRequiredWord(114))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(123)),
                                    text(std::to_wstring(averageMinutesPerSession) +
                                            _languagePreference->getRequiredWord(115))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(125)),
                                    text(std::to_wstring(abstractYear))
                                })
                            })
                    )
                });

        return renderMainPageWithHugePlate(
                _languagePreference->getRequiredWord(126),
                std::move(monthlySummary)
                );
    }

    return std::move(monthlySummary);
}

/// renders last yearly summary data for now.
Element liveApplication::MainPage::renderYearlySummaryPage() noexcept {
    auto lastYearlySummary = mainSystem->getLatestYearlySummary(thereIsAYearlySummary);

    Element yearlySummary;
    if (thereIsAYearlySummary) {
        uint8_t abstractYear = lastYearlySummary->abstractYear;
        uint16_t totalMinutes = lastYearlySummary->yearlyTotalMinutes;
        uint16_t totalSessionNumber = lastYearlySummary->yearlyTotalSessionNumber;

        std::wstring imaginaryYear = _languagePreference->getRequiredWord(127) + std::to_wstring(abstractYear);

        uint16_t averageMinutesPerSession = totalMinutes / totalSessionNumber;
        uint16_t averageMinutesPerMonth = totalMinutes / MONTH_NUMBER_IN_A_YEAR;
        uint16_t averageMinutesPerWeek = totalMinutes / WEEK_NUMBER_IN_A_YEAR;
        uint16_t averageMinutesPerDay = totalMinutes / DAY_NUMBER_IN_A_YEAR;

        yearlySummary =
                vbox({
                    window(
                            text(imaginaryYear),
                            vbox({
                                hbox({
                                    text(_languagePreference->getRequiredWord(128)),
                                    text(std::to_wstring(totalMinutes))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(129)),
                                    text(std::to_wstring(
                                            totalSessionNumber))
                                }),
                                text(_languagePreference->getRequiredWord(130)),
                                hbox({
                                    text(_languagePreference->getRequiredWord(131)),
                                    text(std::to_wstring(averageMinutesPerMonth) +
                                         _languagePreference->getRequiredWord(132))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(131)),
                                    text(std::to_wstring(averageMinutesPerWeek) +
                                         _languagePreference->getRequiredWord(124))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(131)),
                                    text(std::to_wstring(averageMinutesPerDay) +
                                         _languagePreference->getRequiredWord(114))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(131)),
                                    text(std::to_wstring(averageMinutesPerSession) +
                                         _languagePreference->getRequiredWord(115))
                                }),
                                hbox({
                                    text(_languagePreference->getRequiredWord(133)),
                                    text(std::to_wstring(abstractYear))
                                })
                            })
                    )
                });

        return renderMainPageWithHugePlate(
                _languagePreference->getRequiredWord(134),
                std::move(yearlySummary)
                );
    }

    return std::move(yearlySummary);
}

/// the last function to illustrate Help Page option on the screen.
Element liveApplication::MainPage::renderHelpPage() noexcept {

    Elements helpClauses = {
            vbox( text(_languagePreference->getRequiredWord(135)) | bold | center,
                 hflow( text(_languagePreference->getRequiredWord(136)) | bold,
                        text(_languagePreference->getRequiredWord(137))),
                 hflow( text(_languagePreference->getRequiredWord(138)) | bold,
                        text(_languagePreference->getRequiredWord(139))),
                 hflow( text(_languagePreference->getRequiredWord(140)) | bold,
                        text(_languagePreference->getRequiredWord(141))),
                 hflow( text(_languagePreference->getRequiredWord(142)) | bold,
                        text(_languagePreference->getRequiredWord(143))),
                 hflow( text(_languagePreference->getRequiredWord(144)) | bold,
                        text(_languagePreference->getRequiredWord(145))),
                 hflow( text(_languagePreference->getRequiredWord(146)) | bold,
                        text(_languagePreference->getRequiredWord(147))),
                 hflow( text(_languagePreference->getRequiredWord(148)) | bold,
                        text(_languagePreference->getRequiredWord(149))),
                 hflow( text(_languagePreference->getRequiredWord(150)) | bold,
                        text(_languagePreference->getRequiredWord(151))),
                 hflow( text(_languagePreference->getRequiredWord(152)) | bold,
                        text(_languagePreference->getRequiredWord(153))),
                 hflow( text(_languagePreference->getRequiredWord(154)) | bold,
                        text(_languagePreference->getRequiredWord(155))),
                 hflow( text(_languagePreference->getRequiredWord(156)) | bold,
                        text(_languagePreference->getRequiredWord(157))),
                 text(_languagePreference->getRequiredWord(158)) | bold | center,
                 hflow( text(_languagePreference->getRequiredWord(159)) | bold,
                        text(_languagePreference->getRequiredWord(160))),
                 hflow( text(_languagePreference->getRequiredWord(161)) | bold,
                        text(_languagePreference->getRequiredWord(162))),
                 hflow( text(_languagePreference->getRequiredWord(163)) | bold,
                        text(_languagePreference->getRequiredWord(164))),
                 hflow( text(_languagePreference->getRequiredWord(165)) | bold,
                        text(_languagePreference->getRequiredWord(166))),
                 hflow( text(_languagePreference->getRequiredWord(167)) | bold,
                        text(_languagePreference->getRequiredWord(168))),
                 hflow( text(_languagePreference->getRequiredWord(169)) | bold,
                        text(_languagePreference->getRequiredWord(170))),
                 hflow( text(_languagePreference->getRequiredWord(171)) | bold,
                        text(_languagePreference->getRequiredWord(172))),
                 text(_languagePreference->getRequiredWord(173)) | bold | center,
                 hflow( text(L" > ") | bold,
                        text(_languagePreference->getRequiredWord(174)))
            )
    };

    return renderMainPageWithHugePlate(
            _languagePreference->getRequiredWord(175),
            std::move(helpClauses)
            );
}

/// the last function to illustrate About Page option on the screen.
Element liveApplication::MainPage::renderAboutPage() noexcept {

    Element aboutClauses =
            vbox({
                text(L"Time Accumulator") | bold | center | size(HEIGHT, EQUAL, 2), // 2 rows
                hflow(paragraph(_languagePreference->getRequiredWord(176)))
                        | size(HEIGHT, EQUAL, 4), // 4 rows
                hflow(paragraph(_languagePreference->getRequiredWord(177)))
                        | size(HEIGHT, EQUAL, 7), // 7 rows
                hflow(paragraph(_languagePreference->getRequiredWord(178)))
            });

    return renderMainPageWithHugePlate(
            _languagePreference->getRequiredWord(179),
            std::move(aboutClauses)
            );
}

liveApplication::TimeAccumulator::TimeAccumulator(std::shared_ptr<mount::MainSystem> mainSystem) noexcept {
    _language = std::make_unique<Language>(mainSystem);
    _languagePreference = std::make_shared<LanguagePreference>(commonLanguage);

    _mainPage = std::make_unique<MainPage>(_languagePreference);
    _addAndDelete = std::make_unique<AddAndDelete>(_languagePreference);
    _summaries = std::make_unique<Summaries>(_languagePreference);
    _customise = std::make_unique<Customise>(_languagePreference);
    _reset = std::make_unique<Reset>(_languagePreference);
    _attentionWindow = std::make_unique<AttentionWindow>(_languagePreference);

    _commonColor->mainSystem = mainSystem;
    _mainPage->mainSystem = mainSystem;
    _addAndDelete->mainSystem = mainSystem;
    _reset->mainSystem = mainSystem;
    _mainPage->interface = _interface;
    _summaries->interface = _interface;

    _commonColor->adjustColorPair();
    _mainPage->commonColor = _commonColor;
    _addAndDelete->commonColor = _commonColor;
    _summaries->commonColor = _commonColor;
    _customise->commonColor = _commonColor;
    _language->commonColor = _commonColor;
    _reset->commonColor = _commonColor;
    _attentionWindow->commonColor = _commonColor;

    _mainPage->dayAndTime = std::move(mainSystem->getProgramStartInformation());

    Add(&_container);
    _container.Add(&(*_mainPage));
    _container.Add(&(*_addAndDelete));
    _container.Add(&(*_summaries));
    _container.Add(&(*_customise));
    _container.Add(&(*_language));
    _container.Add(&(*_reset));
    _container.Add(&(*_attentionWindow));

    _mainPage->on_quit = [&] { on_quit(); };
    _mainPage->determineAddOrDelete = [&](bool addOrDelete) {
        _addAndDelete->addOrDelete = addOrDelete;
        _addAndDelete->TakeFocus();
    };
    _mainPage->openCustomise = [&] {
        _customise->TakeFocus();
    };
    _mainPage->openLanguage = [&] {
        _language->TakeFocus();
    };
    _mainPage->openSummaries = [&] {
        _summaries->TakeFocus();
    };
    _mainPage->openReset = [&] {
        _reset->TakeFocus();
    };
    _mainPage->openAttentionWindow = [&] {
        _attentionWindow->totalMinutesAreAtLimit = _mainPage->maxTotalMinutesReached;
        _attentionWindow->sessionNumberIsAtLimit = _mainPage->maxSessionNumberReached;
        _attentionWindow->thereIsActiveSession = _mainPage->beginIsOn;
        _attentionWindow->TakeFocus();
    };

    _addAndDelete->closeWindow = [&] {
        _mainPage->TakeFocus();
    };

    _summaries->closeWindow = [&] {
        _mainPage->TakeFocus();
    };

    _customise->closeWindow = [&] {
        _mainPage->TakeFocus();
    };

    _language->closeWindow = [&] {
        _mainPage->TakeFocus();
    };

    _reset->closeWindow = [&] {
        _mainPage->TakeFocus();
    };

    _attentionWindow->closeWindow = [&] {
        _mainPage->TakeFocus();
    };
}
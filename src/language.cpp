#include "language.h"

LanguagePreference::LanguagePreference(uint8_t commonLanguage) noexcept {
    _commonLanguage = commonLanguage;
}

std::wstring LanguagePreference::getRequiredWord(uint8_t index) noexcept {
    if (_commonLanguage == 0)
        return _Turkish.at(index);
    return _English.at(index);
}

std::wstring LanguagePreference::getWeekDayName(uint8_t weekDay) noexcept {
    if (_commonLanguage == 0)
        return week_days_in_Turkish[weekDay];
    return week_days_in_English[weekDay];
}

void LanguagePreference::modifyWeekName(std::wstring& weekName) const noexcept {
    if (_commonLanguage == 0) {
        std::wstring temp = {};
        bool numberBeginning = false;
        for (const auto& character : weekName) {
            if (character == L'-') {
                numberBeginning = true;
                continue;
            }
            if (numberBeginning) {
                temp.push_back(character);
            }
        }
        weekName = {};
        weekName = L"Hafta-";
        weekName.append(temp);
    }
    else {
        /// Making first letter of week name upper case
        std::reverse(weekName.begin(), weekName.end());
        weekName.pop_back();
        weekName.push_back(L'W');
        std::reverse(weekName.begin(), weekName.end());
    }
}
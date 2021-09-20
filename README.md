# Time Accumulator

![license][badge.license]
![C++][badge.cpp]

[badge.license]: https://img.shields.io/github/license/asari555/TimeAccumulator_windows?color=brightgreen
[badge.cpp]: https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white

A TUI app that accumulates working times, allows users to track work minutes and session in a day, and lets users make aware about their performance summaries of previous day, week, month and year. Additionaly, this app indicates users' increased/decreased performance values compared to the previous day.

### Table of Contents

+ **[Demonstration](#demonstration)**
+ **[About Time Accumulator](#about-time-accumulator)**
+ **[Calendar Abstraction](#calendar-abstraction)**
+ **[Operating Systems](#operating-systems)**
+ **[Tested On](#tested-on)**
+ **[Compiled Via](#compiled-via)**
+ **[Command Keys](#command-keys)**
+ **[Requirements](#requirements)**
+ **[Dependencies](#dependencies)**

## Demonstration 

General use of the app.

![demo_linux](./doc/demo_linux.gif)

## About Time Accumulator

Some critical features of this application that should be known:

+ Accumulating of working time for daily performance is the main purpose of this app. For achieving the purpose, users can start and finish sessions with `Begin` and `End` at multiple times.

+ Users can `Add` additional untracked working sessions and `Delete` sessions forgotten open.

+ For the long term usages, the app illustrates daily, weekly, monthly and yearly total working performance summaries in the summaries stage. These summaries can be reached via `Summary` stage in the commands menu. 

+ This app compares current performance and previous day performance and then opens an indicator on the homepage to attract users' attention.

+ `Customise` (`Özelleştir`) stage is the enjoyable part of the application thanks to [ftxui](https://github.com/ArthurSonzogni/FTXUI) library. Many color combinations can be made with changing the text and background colors. In addition, the changes should be saved to remain the selected combinations for further usages. Also, the default option sets the template to black text and white background and saves this preference.

+ Turkish and English are provided as language preferences. The former is the default language. Users can change the language with following the instruction below. The program should be closed and opened again after changing the `Language` preference.

    > **Dil** > **English**	then	**Çıkış**

+ `About` me and `Help` stages are provided in the homepage to learn more and how to use respectively.

+ Mostly, technological devices has a red button for reasonable excuses that should not be pushed in normal conditions. The red button is `Reset` for Time Accumulator. Users can permanently delete their performance history, language preference and customization settings all together via `Reset`.

+ The app automatically close previous session and open new session for coming day when you are working at the end of the day at 00:00 pm.

+ Data of the app is stored at the folder named as user name via *.time-accumulator* folder.

+ *Daily Report* panel illustrates history of started-ended, added and deleted sessions with `S`, `A` and `D` keys respectively. If the history exceeds the page, users can scroll it up and down. In addition, illustration precision of accumulated time is both minutes second level in this panel.

+ *Total Minutes* panel indicates accumulated daily total minutes. The precision is minutes level. The values of the total minutes in *Daily Report* and *Total Minutes* panel may differ due to the rounding rules. 

+ *Total Sessions* panel indicates the number of started-ended and added sessions. `Delete` command does not effect the session number. 

+ *Program Start* panel gives information about when the current app started and the abstract year, namely year number passed with working. 

## Calendar Abstraction

There is a difference between real life and the app in terms of day number in a month and a year. For making ideal and standard the abstraction of a month, the day number of a month was fixed to 28. It is because of that a month has 4 weeks and each week has 7 days. Therefore, the day number of a year becomes 336 days. The abstract year is not a real year starting from the first working day. It is 336 pure working days called abstract year.

Users can only increase the number of working day by just working. It means passing days between two working days are not accounted in the abstraction. Because of that, each summary becomes available when the required abstract number is reached. For example, daily summary is unveiled after a working day passed. It is similar with the other summaries. At least 7, 28 and 336 days passed in work are required to illustrate weekly, monthly and yearly summaries respectively.

In addition, each summary stage is regenerated after the next required number of working days accumulated. For example, the app regenerates the weekly summary after each week that is another separate package of 7 days. This also applies to monthly and annual summaries with their required number of days.

## Command Keys

+ Up `⯅`: Move on upper raw.

+ Down `⯆`: Move on lower raw.

+ Right `⯈`: Move on next menu.

+ Left `⯇`: Move on previous menu.

+ Go up `U`: Scroll daily report histories page up.

+ Go down `D`: Scroll daily report histories page down.

+ Esc `⎋`: Escape from the current window.

+ Return `⮠ `: Select.

## Operating Systems

+ Current platform

    [![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)](https://github.com/asari555/TimeAccumulator)

+ For another platform, please click
    
	[![Mac OS](https://img.shields.io/badge/mac%20os-gray?style=flat&logo=macos&logoColor=F0F0F0)](https://github.com/asari555/TimeAccumulator_macos)

    [![Windows](https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white)](https://github.com/asari555/TimeAccumulator_windows)

## Tested On

![ubuntu-20.04](https://img.shields.io/badge/ubuntu-20.04-brightgreen.svg)

## Compiled Via

> gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)

## Requirements

[Doxygen](https://www.doxygen.nl/index.html)

## Dependencies

+ [Boost 1.71.0](https://www.boost.org/doc/libs/1_71_0/more/getting_started/windows.html)

+ [ftxui](https://github.com/ArthurSonzogni/FTXUI)

+ [msgpack-c](https://github.com/msgpack/msgpack-c/tree/cpp_master)

+ Dynamic Libs:
	- libpthread.so.0
	- libstdc++.so.6
	- libgcc_s.so.1
	- libc.so.6

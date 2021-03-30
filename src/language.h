#ifndef TIME_ACCUMULATOR_LANGUAGE_H
#define TIME_ACCUMULATOR_LANGUAGE_H

#define DEFAULT_LANGUAGE 0

#include <vector>
#include <string>
#include <memory>
#include <map>

class LanguagePreference {
private:
    uint8_t _commonLanguage = DEFAULT_LANGUAGE;

    const std::vector<std::wstring> _Turkish = { // 0 : Turkish
            L" dakika giriniz ",    // index: 0
            L"Eklemek İçin : ",
            L"Silmek İçin : ",
            L"Günlük",
            L"Haftalık",
            L"Aylık",    // index: 5
            L"Yıllık",
            L"Son Özetler",
            L"Varsayılan",
            L"Kaydet",
            L"Siyah",    // index: 10
            L"Kırmızı",
            L"Yeşil",
            L"Sarı",
            L"Mavi",
            L"Eflatun",    // index: 15
            L"Camgöbeği",
            L"Koyu Gri",
            L"Açık Gri",
            L"Açık Kırmızı",
            L"Açık Yeşil",    // index: 20
            L"Açık Sarı",
            L"Açık Mavi",
            L"Açık Eflatun",
            L"Açık Camgöbeği",
            L"Beyaz",    // index: 25
            L"Akuamarin",
            L"Mavi Menekşe",
            L"Donanma Mavisi",
            L"Bahriyeli Mavisi",
            L"Turkuaz",    // index: 30
            L"Menekşe",
            L"Koyu Eflatun",
            L"Sarı",
            L"Yeşil Sarı",
            L"Deniz Yeşili",    // index: 35
            L"Koyu Yeşil",
            L"Pembe",
            L"Mor",
            L"Açık Altın",
            L"Altın",    // index: 40
            L"Açık Turuncu",
            L"Turuncu",
            L"Turuncu Kırmızı",
            L"Kahverengi",
            L"Mercan",    // index: 45
            L"Başak",
            L"Buğday",
            L"Buğulu Gül",
            L"Gül",
            L"Koyu Kırmızı",    // index: 50
            L"Kırmızı",
            L"Gri",
            L"Barut Grisi",
            L"Siyah",
            L"Beyaz",    // index: 55
            L"Özelleştir",
            L"Yazı Rengi",
            L"Zemin Rengi",
            L"Hayır",
            L"Evet",    // index: 60
            L"Dikkat",
            L"TÜM veriyi silmek mi istiyorsun?",
            L"Tamam",
            L"Faal çalışma bulunmaktadır, sonlandırın lütfen.",
            L"Çalışma sayısı haddine ulaştı.",    // index: 65
            L"Faal çalışma bulunmamaktadır.",
            L" -> Bugüne ait kayıt yok",
            L"Ç ",
            L" dk. ",
            L" s.",    // index: 70
            L"E ",
            L"S ",
            L" dk. ",
            L"Başlat",
            L"Bitir",    // index: 75
            L"Ekle",
            L"Sil",
            L"Özet",
            L"Özelleştir",
            L"Dil",    // index: 80
            L"Yardım",
            L"Hakkında",
            L"Sıfırla",
            L"Çıkış",
            L"Komutlar",    // index: 85
            L"Ek İşlemler",
            L"Önceki Günle               : ",
            L" dk.",
            L"Önceki Hafta Ortalamasıyla : ",
            L"Karşılaştırma",    // index: 90
            L"Gün  : ",
            L"Saat : ",
            L"Farazi Yıl : ",
            L"Başlangıç saati ",
            L"Faal Çalışma",    // index: 95
            L" dk. tutuldu",
            L"Uygulama Başlangıcı",
            L"Günlük Döküm",
            L"Toplam Dakika",
            L"Toplam Çalışma",    // index: 100
            L"T. Dakika",
            L"T. Çalışma",
            L"Toplam Dakika  : ",
            L"Toplam Çalışma : ",
            L"Ortalama       : ",    // index: 105
            L" dk/çalışma",
            L"Farazi Sene    : ",
            L"Son Günlük Özet",
            L"Başlangıç - Bitiş Günleri : ",
            L"Toplam dakika             : ",    // index: 110
            L"Toplam çalışma            : ",
            L"Ortalama                  : ",
            L"                            ",
            L" dk/gün,",
            L" dk/çalışma",    // index: 115
            L"Farazi sene               : ",
            L"Son Haftalık Özet",
            L"Ay-",
            L"İlk - Son haftalar : ",
            L"Toplam dakika      : ",    // index: 120
            L"Toplam çalışma     : ",
            L"Ortalamalar        : ",
            L"                     ",
            L" dk/hafta,",
            L"Farazi sene        : ",    // index: 125
            L"Son Aylık Özet",
            L"Sene-",
            L"Toplam dakika      : ",
            L"Toplam çalışma     : ",
            L"Ortalamalar        : ",    // index: 130
            L"                     ",
            L" dk/ay,",
            L"Farazi sene        : ",
            L"Son Yıllık Özet",
            L"Ana Komutlar:",    // index: 135
            L" > Başla",
            L"         çalışma başlat",
            L" > Bitir",
            L"         çalışmayı sonlandır",
            L" > Ekle",    // index: 140
            L"          el ile dakika/lar ekle",
            L" > Sil",
            L"           fazlalık dakika/ları sil",
            L" > Özet",
            L"          son özetler listesini göster",    // index: 145
            L" > Özelleştir",
            L"    özelleştirme sayfasını aç",
            L" > Yardım",
            L"        yardım bildirgesini göster",
            L" > Hakkında",    // index: 150
            L"      uygulama hakkında bilgiler sun",
            L" > Sıfırla",
            L"       kayıtlı tüm veriyi sil",
            L" > Çıkış",
            L"         uygulamayı kapat",    // index: 155
            L"İpucu: ",
            L"\"T.\"\'lerin açılımını görmek için sayfayı genişetin",
            L"Önerilen Klavye Komutları:",
            L" > Sağ Ok",
            L"             sonraki listeye geç",    // index: 160
            L" > Sol Ok",
            L"             önceki listeye geç",
            L" > Yukarı-Aşağı Ok",
            L"    üst veya alt elemana geç",
            L" > Esc",    // index: 165
            L"               geri dön",
            L" > Enter",
            L"              onayla",
            L" > U Harfi",
            L"            günlük döküm odağını yukarı kaydır",    // index: 170
            L" > D Harfi",
            L"            günlük döküm odağını aşağı kaydır",
            L"Günlük Döküm Hakkında:",
            L"Ç : çalışma yapildi, E : eklendi, D : silindi",
            L"Yardım",    // index: 175
            L"   Time Accumulator uygulamasının asıl gayesi günlük toplam "
            "çalışma zamanını ve günlük, haftalık, aylık ve yıllık çalışma "
            "özetlerini kullanıcıya açık şekilde sunmaktır.",
            L"   Uygulama Başla sonra Bitir komutları kullanıldığında "
            "tamamlanmış bir çalışma algılar. Uygulama haricinde tutulmuş "
            "çalışma süresi varsa bu süre Ekle vasitasiyla el ile girilir. "
            "Toplam çalışma sayısı yalnız Bitir ve Ekle komutlarından sonra "
            "artar. Sil komutuysa gayri ihtiyari toplama dahil olmuş dakikaları"
            " silmek için kullanılır.",
            L"   Ek olarak uygulamada bir takım gizli özellikler bulunmaktadır."
            " Bu noktada, saklı olanı keşfetmek için en az bir çalışma haftası"
            " boyunca sabır ve sebat tavsiye edilir.",
            L"Hakkında",	// index : 179,
            L"Günlük çalışılabilecek daha fazla dakika kalmadı."
    };

    const std::vector<std::wstring> _English = {    // 1 : English
            L" minute as positive integer ",    // index: 0
            L"Enter To Add : ",
            L"Enter To Delete : ",
            L"Daily",
            L"Weekly",
            L"Monthly",    // index: 5
            L"Yearly",
            L"Last Summaries",
            L"Default",
            L"Save",
            L"Black",    // index: 10
            L"Red",
            L"Green",
            L"Yellow",
            L"Blue",
            L"Magenta",    // index: 15
            L"Cyan",
            L"Dark Gray",
            L"Light Gray",
            L"Light Red",
            L"Light Green",    // index: 20
            L"Light Yellow",
            L"Light Blue",
            L"Light Magenta",
            L"Light Cyan",
            L"White",    // index: 25
            L"Aquamarine",
            L"Blue Violet",
            L"Navy Blue",
            L"Cadet Blue",
            L"Dark Turquoise",    // index: 30
            L"Violet",
            L"Dark Magenta",
            L"Yellow",
            L"Green Yellow",
            L"Dark Sea Green",    // index: 35
            L"Dark Green",
            L"Pink",
            L"Purple",
            L"Light Gold",
            L"Gold",    // index: 40
            L"Orangish White",
            L"Orange",
            L"Orange Red",
            L"Sandy Brown",
            L"Light Coral",    // index: 45
            L"Golden Rod",
            L"Wheat",
            L"Misty Rose",
            L"Rose",
            L"Dark Red",    // index: 50
            L"Red",
            L"Grey",
            L"Slate Grey",
            L"Black",
            L"White",    // index: 55
            L"Customise",
            L"Text Color",
            L"Background Color",
            L"No",
            L"Yes",    // index: 60
            L"Attention",
            L"Do you want to delete ALL data?",
            L"Ok",
            L"There is an active session, please End it.",
            L"Number of sessions has reached the limit.",    // index: 65
            L"There is no active session.",
            L" -> History is empty",
            L"S ",
            L" m. ",
            L" s.",    // index: 70
            L"A ",
            L"D ",
            L" m. ",
            L"Begin",
            L"End",    // index: 75
            L"Add",
            L"Delete",
            L"Summary",
            L"Customise",
            L"Language",    // index: 80
            L"Help",
            L"About",
            L"Reset",
            L"Exit",
            L"Commands",    // index: 85
            L"Appendix",
            L"Previous Day          : ",
            L" min.",
            L"Previous Week Average : ",
            L"Comparing To",    // index: 90
            L"Day  : ",
            L"Time : ",
            L"Abstract Year : ",
            L"Began at ",
            L"Active Session",    // index: 95
            L" min. kept",
            L"Program Start",
            L"Daily Report",
            L"Total Minutes",
            L"Total Sessions",    // index: 100
            L"T. Minutes",
            L"T. Sessions",
            L"Total Minutes  : ",
            L"Total Sessions : ",
            L"Average        : ",    // index: 105
            L" min/session",
            L"Abstract year  : ",
            L"Last Daily Summary",
            L"Start - End dates : ",
            L"Total minutes     : ",    // index: 110
            L"Total sessions    : ",
            L"Averages          : ",
            L"                    ",
            L" min/day,",
            L" min/session",    // index: 115
            L"Abstract year     : ",
            L"Last Weekly Summary",
            L"Month-",
            L"Start - End weeks : ",
            L"Total minutes     : ",    // index: 120
            L"Total sessions    : ",
            L"Averages          :",
            L"                    ",
            L" min/week,",
            L"Abstract year     : ",    // index: 125
            L"Last Monthly Summary",
            L"Year-",
            L"Total minutes  : ",
            L"Total sessions : ",
            L"Averages       : ",    // index: 130
            L"                 ",
            L" min/month,",
            L"Abstract year  : ",
            L"Last Yearly Summary",
            L"Main Options:",    // index: 135
            L" > Begin",
            L"           start a session",
            L" > End",
            L"             finish a started session",
            L" > Add",    // index: 140
            L"             add minute/s as a manually created session",
            L" > Delete",
            L"          delete extra minute/s",
            L" > Summary",
            L"         open a menu for last summaries",    // index: 145
            L" > Customise",
            L"       open page of customising color",
            L" > Help",
            L"            display help message",
            L" > About",    // index: 150
            L"           display information about the application",
            L" > Reset",
            L"           delete all recorded data",
            L" > Exit",
            L"            close the application",    // index: 155
            L"Hint:",
            L" Expand the window to see \"T.\"s on the main page",
            L"Recommended Keyboard Commands:",
            L" > Right Arrow",
            L"     move to following menu",    // index: 160
            L" > Left Arrow",
            L"      move to previous menu",
            L" > Up-Down Arrow",
            L"   move among entities of any menu",
            L" > Escape",    // index: 165
            L"          go back",
            L" > Return",
            L"          select",
            L" > U Letter",
            L"        scroll up daily report page",    // index: 170
            L" > D Letter",
            L"        scroll down daily report page",
            L"About Daily Report:",
            L"S : session done, A : added, D : deleted",
            L"Help",    // index: 175
            L"   Lucidly presenting accumulated time of daily work and daily, "
            "weekly, monthly, and yearly summaries to users is the main purpose"
            " of Time Accumulator.",
            L"   The program creates a session with using Begin then End "
            "command. If a session was done without utilizing the program, "
            "minutes of the session can be added via Add command by hand. "
            "Total session number is increased only after End and Add "
            "commands. There is also delete command to set a session or total "
            "minutes in case of error.",
            L"   In addition, there are some hidden properties of the program. "
            "A little patient is recommended here. Please wait at least a week "
            "of working and discover what is hidden.",
            L"About",   // index: 179
            L"There are no more time left to work in a day."
    };

    const std::vector<std::wstring> week_days_in_English = {
            L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
    const std::vector<std::wstring> week_days_in_Turkish = {
            L"Paz", L"Pzt", L"Sal", L"Çar", L"Per", L"Cum", L"Cmt"};

public:
    explicit LanguagePreference(uint8_t commonLanguage) noexcept;

    std::wstring getRequiredWord(uint8_t index) noexcept;

    std::wstring getWeekDayName(uint8_t weekDay) noexcept;

    void modifyWeekName(std::wstring& weekName) const noexcept;
};

#endif //TIME_ACCUMULATOR_LANGUAGE_H

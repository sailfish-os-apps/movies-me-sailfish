#ifndef MOVIESWORKER_H
#define MOVIESWORKER_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#define tmdbImgUrl         QStringLiteral ("http://image.tmdb.org/t/p/w342")
#define tmdbApiUrl         QStringLiteral ("http://api.themoviedb.org/3")
#define tmdbApiKey         QStringLiteral ("6afcfd358f445edf5deed91b672792be")

#define imgNoPoster        QStringLiteral ("qrc:/qml/images/no-poster-w342.jpg")

#define movieInfoRequest   QStringLiteral ("%1/movie/%2?api_key=%3&append_to_response=releases,alternative_titles,credits")
#define movieSearchRequest QStringLiteral ("%1/search/movie?api_key=%2&query=%3")

static QString languageCodes [] = {
    "",  // AnyLanguage
    "",  // C
    "ab",  // Abkhazian
    "om",  // Oromo
    "aa",  // Afar
    "af",  // Afrikaans
    "sq",  // Albanian
    "am",  // Amharic
    "ar",  // Arabic
    "hy",  // Armenian
    "as",  // Assamese
    "ay",  // Aymara
    "az",  // Azerbaijani
    "ba",  // Bashkir
    "eu",  // Basque
    "bn",  // Bengali
    "dz",  // Dzongkha
    "bh",  // Bihari
    "bi",  // Bislama
    "br",  // Breton
    "bg",  // Bulgarian
    "my",  // Burmese
    "be",  // Belarusian
    "km",  // Khmer
    "ca",  // Catalan
    "zh",  // Chinese
    "co",  // Corsican
    "hr",  // Croatian
    "cs",  // Czech
    "da",  // Danish
    "nl",  // Dutch
    "en",  // English
    "eo",  // Esperanto
    "et",  // Estonian
    "fo",  // Faroese
    "fj",  // Fijian
    "fi",  // Finnish
    "fr",  // French
    "fy",  // Western Frisian
    "gd",  // Gaelic
    "gl",  // Galician
    "ka",  // Georgian
    "de",  // German
    "el",  // Greek
    "kl",  // Greenlandic
    "gn",  // Guarani
    "gu",  // Gujarati
    "ha",  // Hausa
    "he",  // Hebrew
    "hi",  // Hindi
    "hu",  // Hungarian
    "is",  // Icelandic
    "id",  // Indonesian
    "ia",  // Interlingua
    "ie",  // Interlingue
    "iu",  // Inuktitut
    "ik",  // Inupiak
    "ga",  // Irish
    "it",  // Italian
    "ja",  // Japanese
    "jv",  // Javanese
    "kn",  // Kannada
    "ks",  // Kashmiri
    "kk",  // Kazakh
    "rw",  // Kinyarwanda
    "ky",  // Kirghiz
    "ko",  // Korean
    "ku",  // Kurdish
    "rn",  // Rundi
    "lo",  // Lao
    "la",  // Latin
    "lv",  // Latvian
    "ln",  // Lingala
    "lt",  // Lithuanian
    "mk",  // Macedonian
    "mg",  // Malagasy
    "ms",  // Malay
    "ml",  // Malayalam
    "mt",  // Maltese
    "mi",  // Maori
    "mr",  // Marathi
    "mh",  // Marshallese
    "mn",  // Mongolian
    "na",  // Nauru
    "ne",  // Nepali
    "nb",  // NorwegianBokmal
    "oc",  // Occitan
    "or",  // Oriya
    "ps",  // Pashto
    "fa",  // Persian
    "pl",  // Polish
    "pt",  // Portuguese
    "pa",  // Punjabi
    "qu",  // Quechua
    "rm",  // Romansh
    "ro",  // Romanian
    "ru",  // Russian
    "sm",  // Samoan
    "sg",  // Sango
    "sa",  // Sanskrit
    "sr",  // Serbian
    "os",  // Ossetic
    "st",  // Southern Sotho
    "tn",  // Tswana
    "sn",  // Shona
    "sd",  // Sindhi
    "si",  // Sinhala
    "ss",  // Swati
    "sk",  // Slovak
    "sl",  // Slovenian
    "so",  // Somali
    "es",  // Spanish
    "su",  // Sundanese
    "sw",  // Swahili
    "sv",  // Swedish
    "sc",  // Sardinian
    "tg",  // Tajik
    "ta",  // Tamil
    "tt",  // Tatar
    "te",  // Telugu
    "th",  // Thai
    "bo",  // Tibetan
    "ti",  // Tigrinya
    "to",  // Tongan
    "ts",  // Tsonga
    "tr",  // Turkish
    "tk",  // Turkmen
    "ty",  // Tahitian
    "ug",  // Uighur
    "uk",  // Ukrainian
    "ur",  // Urdu
    "uz",  // Uzbek
    "vi",  // Vietnamese
    "vo",  // Volapuk
    "cy",  // Welsh
    "wo",  // Wolof
    "xh",  // Xhosa
    "yi",  // Yiddish
    "yo",  // Yoruba
    "za",  // Zhuang
    "zu",  // Zulu
    "nn",  // NorwegianNynorsk
    "bs",  // Bosnian
    "dv",  // Divehi
    "gv",  // Manx
    "kw",  // Cornish
    "ak",  // Akan
    "kok", // Konkani
    "gaa", // Ga
    "ig",  // Igbo
    "kam", // Kamba
    "syr", // Syriac
    "byn", // Blin
    "gez", // Geez
    "kfo", // Koro
    "sid", // Sidamo
    "cch", // Atsam
    "tig", // Tigre
    "kaj", // Jju
    "fur", // Friulian
    "ve",  // Venda
    "ee",  // Ewe
    "wal", // Walamo
    "haw", // Hawaiian
    "kcg", // Tyap
    "ny",  // Nyanja
    "fil", // Filipino
    "gsw", // Swiss German
    "ii",  // Sichuan Yi
    "kpe", // Kpelle
    "nds", // Low German
    "nr",  // South Ndebele
    "nso", // Northern Sotho
    "se",  // Northern Sami
    "trv", // Taroko
    "guz", // Gusii
    "dav", // Taita
    "ff",  // Fulah
    "ki",  // Kikuyu
    "saq", // Samburu
    "seh", // Sena
    "nd",  // North Ndebele
    "rof", // Rombo
    "shi", // Tachelhit
    "kab", // Kabyle
    "nyn", // Nyankole
    "bez", // Bena
    "vun", // Vunjo
    "bm",  // Bambara
    "ebu", // Embu
    "chr", // Cherokee
    "mfe", // Morisyen
    "kde", // Makonde
    "lag", // Langi
    "lg",  // Ganda
    "bem", // Bemba
    "kea", // Kabuverdianu
    "mer", // Meru
    "kln", // Kalenjin
    "naq", // Nama
    "jmc", // Machame
    "ksh", // Colognian
    "mas", // Masai
    "xog", // Soga
    "luy", // Luyia
    "asa", // Asu
    "teo", // Teso
    "ssy", // Saho
    "khq", // Koyra Chiini
    "rwk", // Rwa
    "luo", // Luo
    "cgg", // Chiga
    "tzm", // Central Morocco Tamazight
    "ses", // Koyraboro Senni
    "ksb", // Shambala
    "brx", // Bodo
    "av",  // Avaric
    "ch",  // Chamorro
    "ce",  // Chechen
    "cu",  // Church
    "cv",  // Chuvash
    "cr",  // Cree
    "ht",  // Haitian
    "hz",  // Herero
    "ho",  // Hiri Motu
    "kr",  // Kanuri
    "kv",  // Komi
    "kg",  // Kongo
    "kj",  // Kwanyama
    "li",  // Limburgish
    "lu",  // LubaKatanga
    "lb",  // Luxembourgish
    "nv",  // Navaho
    "ng",  // Ndonga
    "oj",  // Ojibwa
    "pi",  // Pali
    "wa",  // Walloon
    "agq", // Aghem
    "bas", // Basaa
    "dje", // Zarma
    "dua", // Duala
    "dyo", // JolaFonyi
    "ewo", // Ewondo
    "ksf", // Bafia
    "mgh", // MakhuwaMeetto
    "mua", // Mundang
    "nmg", // Kwasio
    "nus", // Nuer
    "sah", // Sakha
    "sbp", // Sangu
    "swc", // Congo Swahili
    "twq", // Tasawaq
    "vai", // Vai
    "wae", // Walser
    "yav", // Yangben
    "ae",  // Avestan
    "ast", // Asturian
    "jgo", // Ngomba
    "kkj", // Kako
    "mgo", // Meta
    "nnh", // Ngiemboon
    "an",  // Aragonese
    "akk", // Akkadian
    "egy", // AncientEgyptian
    "grc", // AncientGreek
    "arc", // Aramaic
    "ban", // Balinese
    "bax", // Bamun
    "bbc", // BatakToba
    "bug", // Buginese
    "bku", // Buhid
    "xcr", // Carian
    "ccp", // Chakma
    "myz", // ClassicalMandaic
    "cop", // Coptic
    "doi", // Dogri
    "cjm", // EasternCham
    "eky", // EasternKayah
    "ett", // Etruscan
    "got", // Gothic
    "hnn", // Hanunoo
    "inh", // Ingush
    "hmd", // LargeFloweryMiao
    "lep", // Lepcha
    "lif", // Limbu
    "lis", // Lisu
    "khb", // Lu
    "xlc", // Lycian
    "xld", // Lydian
    "man", // Mandingo
    "mni", // Manipuri
    "xmr", // Meroitic
    "nod", // NorthernThai
    "sga", // OldIrish
    "non", // OldNorse
    "peo", // OldPersian
    "otk", // OldTurkish
    "pal", // Pahlavi
    "xpr", // Parthian
    "phn", // Phoenician
    "pra", // PrakritLanguage
    "rej", // Rejang
    "xsa", // Sabaean
    "smp", // Samaritan
    "sat", // Santali
    "saz", // Saurashtra
    "srb", // Sora
    "syl", // Sylheti
    "tbw", // Tagbanwa
    "blt", // TaiDam
    "tdd", // TaiNua
    "uga", // Ugaritic
    "bss", // Akoose
    "lkt", // Lakota
    "zgh", // Standard Moroccan Tamazight
};

static QString countryCodes [] = {
    "ZZ",  // AnyCountry
    "AF",  // Afghanistan
    "AL",  // Albania
    "DZ",  // Algeria
    "AS",  // AmericanSamoa
    "AD",  // Andorra
    "AO",  // Angola
    "AI",  // Anguilla
    "AQ",  // Antarctica
    "AG",  // AntiguaAndBarbuda
    "AR",  // Argentina
    "AM",  // Armenia
    "AW",  // Aruba
    "AU",  // Australia
    "AT",  // Austria
    "AZ",  // Azerbaijan
    "BS",  // Bahamas
    "BH",  // Bahrain
    "BD",  // Bangladesh
    "BB",  // Barbados
    "BY",  // Belarus
    "BE",  // Belgium
    "BZ",  // Belize
    "BJ",  // Benin
    "BM",  // Bermuda
    "BT",  // Bhutan
    "BO",  // Bolivia
    "BA",  // BosniaAndHerzegowina
    "BW",  // Botswana
    "BV",  // BouvetIsland
    "BR",  // Brazil
    "IO",  // BritishIndianOceanTerritory
    "BN",  // Brunei
    "BG",  // Bulgaria
    "BF",  // BurkinaFaso
    "BI",  // Burundi
    "KH",  // Cambodia
    "CM",  // Cameroon
    "CA",  // Canada
    "CV",  // CapeVerde
    "KY",  // CaymanIslands
    "CF",  // CentralAfricanRepublic
    "TD",  // Chad
    "CL",  // Chile
    "CN",  // China
    "CX",  // ChristmasIsland
    "CC",  // CocosIslands
    "CO",  // Colombia
    "KM",  // Comoros
    "CD",  // CongoKinshasa
    "CG",  // CongoBrazzaville
    "CK",  // CookIslands
    "CR",  // CostaRica
    "CI",  // IvoryCoast
    "HR",  // Croatia
    "CU",  // Cuba
    "CY",  // Cyprus
    "CZ",  // CzechRepublic
    "DK",  // Denmark
    "DJ",  // Djibouti
    "DM",  // Dominica
    "DO",  // DominicanRepublic
    "TL",  // EastTimor
    "EC",  // Ecuador
    "EG",  // Egypt
    "SV",  // ElSalvador
    "GQ",  // EquatorialGuinea
    "ER",  // Eritrea
    "EE",  // Estonia
    "ET",  // Ethiopia
    "FK",  // FalklandIslands
    "FO",  // FaroeIslands
    "FJ",  // Fiji
    "FI",  // Finland
    "FR",  // France
    "GG",  // Guernsey
    "GF",  // FrenchGuiana
    "PF",  // FrenchPolynesia
    "TF",  // FrenchSouthernTerritories
    "GA",  // Gabon
    "GM",  // Gambia
    "GE",  // Georgia
    "DE",  // Germany
    "GH",  // Ghana
    "GI",  // Gibraltar
    "GR",  // Greece
    "GL",  // Greenland
    "GD",  // Grenada
    "GP",  // Guadeloupe
    "GU",  // Guam
    "GT",  // Guatemala
    "GN",  // Guinea
    "GW",  // GuineaBissau
    "GY",  // Guyana
    "HT",  // Haiti
    "HM",  // HeardAndMcDonaldIslands
    "HN",  // Honduras
    "HK",  // HongKong
    "HU",  // Hungary
    "IS",  // Iceland
    "IN",  // India
    "ID",  // Indonesia
    "IR",  // Iran
    "IQ",  // Iraq
    "IE",  // Ireland
    "IL",  // Israel
    "IT",  // Italy
    "JM",  // Jamaica
    "JP",  // Japan
    "JO",  // Jordan
    "KZ",  // Kazakhstan
    "KE",  // Kenya
    "KI",  // Kiribati
    "KP",  // NorthKorea
    "KR",  // SouthKorea
    "KW",  // Kuwait
    "KG",  // Kyrgyzstan
    "LA",  // Laos
    "LV",  // Latvia
    "LB",  // Lebanon
    "LS",  // Lesotho
    "LR",  // Liberia
    "LY",  // Libya
    "LI",  // Liechtenstein
    "LT",  // Lithuania
    "LU",  // Luxembourg
    "MO",  // Macau
    "MK",  // Macedonia
    "MG",  // Madagascar
    "MW",  // Malawi
    "MY",  // Malaysia
    "MV",  // Maldives
    "ML",  // Mali
    "MT",  // Malta
    "MH",  // MarshallIslands
    "MQ",  // Martinique
    "MR",  // Mauritania
    "MU",  // Mauritius
    "YT",  // Mayotte
    "MX",  // Mexico
    "FM",  // Micronesia
    "MD",  // Moldova
    "MC",  // Monaco
    "MN",  // Mongolia
    "MS",  // Montserrat
    "MA",  // Morocco
    "MZ",  // Mozambique
    "MM",  // Myanmar
    "NA",  // Namibia
    "NR",  // Nauru
    "NP",  // Nepal
    "NL",  // Netherlands
    "CW",  // CuraSao
    "NC",  // NewCaledonia
    "NZ",  // NewZealand
    "NI",  // Nicaragua
    "NE",  // Niger
    "NG",  // Nigeria
    "NU",  // Niue
    "NF",  // NorfolkIsland
    "MP",  // NorthernMarianaIslands
    "NO",  // Norway
    "OM",  // Oman
    "PK",  // Pakistan
    "PW",  // Palau
    "PS",  // PalestinianTerritories
    "PA",  // Panama
    "PG",  // PapuaNewGuinea
    "PY",  // Paraguay
    "PE",  // Peru
    "PH",  // Philippines
    "PN",  // Pitcairn
    "PL",  // Poland
    "PT",  // Portugal
    "PR",  // PuertoRico
    "QA",  // Qatar
    "RE",  // Reunion
    "RO",  // Romania
    "RU",  // Russia
    "RW",  // Rwanda
    "KN",  // SaintKittsAndNevis
    "LC",  // SaintLucia
    "VC",  // SaintVincentAndTheGrenadines
    "WS",  // Samoa
    "SM",  // SanMarino
    "ST",  // SaoTomeAndPrincipe
    "SA",  // SaudiArabia
    "SN",  // Senegal
    "SC",  // Seychelles
    "SL",  // SierraLeone
    "SG",  // Singapore
    "SK",  // Slovakia
    "SI",  // Slovenia
    "SB",  // SolomonIslands
    "SO",  // Somalia
    "ZA",  // SouthAfrica
    "GS",  // SouthGeorgiaAndTheSouthSandwichIslands
    "ES",  // Spain
    "LK",  // SriLanka
    "SH",  // SaintHelena
    "PM",  // SaintPierreAndMiquelon
    "SD",  // Sudan
    "SR",  // Suriname
    "SJ",  // SvalbardAndJanMayenIslands
    "SZ",  // Swaziland
    "SE",  // Sweden
    "CH",  // Switzerland
    "SY",  // Syria
    "TW",  // Taiwan
    "TJ",  // Tajikistan
    "TZ",  // Tanzania
    "TH",  // Thailand
    "TG",  // Togo
    "TK",  // Tokelau
    "TO",  // Tonga
    "TT",  // TrinidadAndTobago
    "TN",  // Tunisia
    "TR",  // Turkey
    "TM",  // Turkmenistan
    "TC",  // TurksAndCaicosIslands
    "TV",  // Tuvalu
    "UG",  // Uganda
    "UA",  // Ukraine
    "AE",  // UnitedArabEmirates
    "GB",  // UnitedKingdom
    "US",  // UnitedStates
    "UM",  // UnitedStatesMinorOutlyingIslands
    "UY",  // Uruguay
    "UZ",  // Uzbekistan
    "VU",  // Vanuatu
    "VA",  // VaticanCityState
    "VE",  // Venezuela
    "VN",  // Vietnam
    "VG",  // BritishVirginIslands
    "VI",  // UnitedStatesVirginIslands
    "WF",  // WallisAndFutunaIslands
    "EH",  // WesternSahara
    "YE",  // Yemen
    "IC",  // CanaryIslands
    "ZM",  // Zambia
    "ZW",  // Zimbabwe
    "CP",  // ClippertonIsland
    "ME",  // Montenegro
    "RS",  // Serbia
    "BL",  // Saint Barthelemy
    "MF",  // Saint Martin
    "419", // LatinAmericaAndTheCaribbean
    "AC",  // AscensionIsland
    "AX",  // AlandIslands
    "DG",  // DiegoGarcia
    "EA",  // CeutaAndMelilla
    "IM",  // IsleOfMan
    "JE",  // Jersey
    "TA",  // TristanDaCunha
    "SS",  // SouthSudan
    "BQ",  // Bonaire
    "SX",  // SintMaarten
    "XK",  // Kosovo
};

class MoviesWorker : public QObject {
    Q_OBJECT

public:
    explicit MoviesWorker    (QObject * parent = NULL);

public slots:
    void initialize          ();
    void loadMoviesFromDb    ();
    void searchForMovie      (QString name);
    void removeMovieInfo     (int     tmdbId);
    void getFullMovieInfo    (int     tmdbId);
    void updateMovieVote     (int     tmdbId, int vote);

signals:
    void movieItemAdded      (int tmdbId);
    void movieItemRemoved    (int tmdbId);
    void movieItemUpdated    (int tmdbId, QVariantMap values);
    void movieSearchResults  (QVariantList list);
    void releaseDatesUpdated (QVariantList list);
    void castRolesUpdated    (QVariantList list);
    void crewTeamUpdated     (QVariantList list);
    void altTitlesUpdated    (QVariantList list);

private slots:
    void onSearchReply       ();
    void onMovieReply        ();
    //void onReplyFinished     (QNetworkReply * reply);
    void onReplyError        (QNetworkReply::NetworkError code);

private:
    QSqlDatabase            m_db;
    QNetworkAccessManager * m_nam;
    QHash<QString, QString> m_hashCountries;
    QHash<QString, QString> m_hashLanguages;
};

#endif // MOVIESWORKER_H

#pragma once

#include <cstdint>
#include <utility>

#include "utils/range_set.h"

namespace Unicode
{
    using CharSet   = RangeSet<std::uint32_t>;
    using CharRange = CharSet::Range;

    namespace Ranges
    {
        // Last version of the range list can be found at: https://www.unicode.org/Public/UCD/latest/ucd/Blocks.txt
        inline constexpr CharRange
            Basic_Latin                                    = CharRange::Inclusive(0x0000, 0x007F),
            Latin_1_Supplement                             = CharRange::Inclusive(0x0080, 0x00FF),
            Latin_Extended_A                               = CharRange::Inclusive(0x0100, 0x017F),
            Latin_Extended_B                               = CharRange::Inclusive(0x0180, 0x024F),
            IPA_Extensions                                 = CharRange::Inclusive(0x0250, 0x02AF),
            Spacing_Modifier_Letters                       = CharRange::Inclusive(0x02B0, 0x02FF),
            Combining_Diacritical_Marks                    = CharRange::Inclusive(0x0300, 0x036F),
            Greek_and_Coptic                               = CharRange::Inclusive(0x0370, 0x03FF),
            Cyrillic                                       = CharRange::Inclusive(0x0400, 0x04FF),
            Cyrillic_Supplement                            = CharRange::Inclusive(0x0500, 0x052F),
            Armenian                                       = CharRange::Inclusive(0x0530, 0x058F),
            Hebrew                                         = CharRange::Inclusive(0x0590, 0x05FF),
            Arabic                                         = CharRange::Inclusive(0x0600, 0x06FF),
            Syriac                                         = CharRange::Inclusive(0x0700, 0x074F),
            Arabic_Supplement                              = CharRange::Inclusive(0x0750, 0x077F),
            Thaana                                         = CharRange::Inclusive(0x0780, 0x07BF),
            NKo                                            = CharRange::Inclusive(0x07C0, 0x07FF),
            Samaritan                                      = CharRange::Inclusive(0x0800, 0x083F),
            Mandaic                                        = CharRange::Inclusive(0x0840, 0x085F),
            Syriac_Supplement                              = CharRange::Inclusive(0x0860, 0x086F),
            Arabic_Extended_A                              = CharRange::Inclusive(0x08A0, 0x08FF),
            Devanagari                                     = CharRange::Inclusive(0x0900, 0x097F),
            Bengali                                        = CharRange::Inclusive(0x0980, 0x09FF),
            Gurmukhi                                       = CharRange::Inclusive(0x0A00, 0x0A7F),
            Gujarati                                       = CharRange::Inclusive(0x0A80, 0x0AFF),
            Oriya                                          = CharRange::Inclusive(0x0B00, 0x0B7F),
            Tamil                                          = CharRange::Inclusive(0x0B80, 0x0BFF),
            Telugu                                         = CharRange::Inclusive(0x0C00, 0x0C7F),
            Kannada                                        = CharRange::Inclusive(0x0C80, 0x0CFF),
            Malayalam                                      = CharRange::Inclusive(0x0D00, 0x0D7F),
            Sinhala                                        = CharRange::Inclusive(0x0D80, 0x0DFF),
            Thai                                           = CharRange::Inclusive(0x0E00, 0x0E7F),
            Lao                                            = CharRange::Inclusive(0x0E80, 0x0EFF),
            Tibetan                                        = CharRange::Inclusive(0x0F00, 0x0FFF),
            Myanmar                                        = CharRange::Inclusive(0x1000, 0x109F),
            Georgian                                       = CharRange::Inclusive(0x10A0, 0x10FF),
            Hangul_Jamo                                    = CharRange::Inclusive(0x1100, 0x11FF),
            Ethiopic                                       = CharRange::Inclusive(0x1200, 0x137F),
            Ethiopic_Supplement                            = CharRange::Inclusive(0x1380, 0x139F),
            Cherokee                                       = CharRange::Inclusive(0x13A0, 0x13FF),
            Unified_Canadian_Aboriginal_Syllabics          = CharRange::Inclusive(0x1400, 0x167F),
            Ogham                                          = CharRange::Inclusive(0x1680, 0x169F),
            Runic                                          = CharRange::Inclusive(0x16A0, 0x16FF),
            Tagalog                                        = CharRange::Inclusive(0x1700, 0x171F),
            Hanunoo                                        = CharRange::Inclusive(0x1720, 0x173F),
            Buhid                                          = CharRange::Inclusive(0x1740, 0x175F),
            Tagbanwa                                       = CharRange::Inclusive(0x1760, 0x177F),
            Khmer                                          = CharRange::Inclusive(0x1780, 0x17FF),
            Mongolian                                      = CharRange::Inclusive(0x1800, 0x18AF),
            Unified_Canadian_Aboriginal_Syllabics_Extended = CharRange::Inclusive(0x18B0, 0x18FF),
            Limbu                                          = CharRange::Inclusive(0x1900, 0x194F),
            Tai_Le                                         = CharRange::Inclusive(0x1950, 0x197F),
            New_Tai_Lue                                    = CharRange::Inclusive(0x1980, 0x19DF),
            Khmer_Symbols                                  = CharRange::Inclusive(0x19E0, 0x19FF),
            Buginese                                       = CharRange::Inclusive(0x1A00, 0x1A1F),
            Tai_Tham                                       = CharRange::Inclusive(0x1A20, 0x1AAF),
            Combining_Diacritical_Marks_Extended           = CharRange::Inclusive(0x1AB0, 0x1AFF),
            Balinese                                       = CharRange::Inclusive(0x1B00, 0x1B7F),
            Sundanese                                      = CharRange::Inclusive(0x1B80, 0x1BBF),
            Batak                                          = CharRange::Inclusive(0x1BC0, 0x1BFF),
            Lepcha                                         = CharRange::Inclusive(0x1C00, 0x1C4F),
            Ol_Chiki                                       = CharRange::Inclusive(0x1C50, 0x1C7F),
            Cyrillic_Extended_C                            = CharRange::Inclusive(0x1C80, 0x1C8F),
            Georgian_Extended                              = CharRange::Inclusive(0x1C90, 0x1CBF),
            Sundanese_Supplement                           = CharRange::Inclusive(0x1CC0, 0x1CCF),
            Vedic_Extensions                               = CharRange::Inclusive(0x1CD0, 0x1CFF),
            Phonetic_Extensions                            = CharRange::Inclusive(0x1D00, 0x1D7F),
            Phonetic_Extensions_Supplement                 = CharRange::Inclusive(0x1D80, 0x1DBF),
            Combining_Diacritical_Marks_Supplement         = CharRange::Inclusive(0x1DC0, 0x1DFF),
            Latin_Extended_Additional                      = CharRange::Inclusive(0x1E00, 0x1EFF),
            Greek_Extended                                 = CharRange::Inclusive(0x1F00, 0x1FFF),
            General_Punctuation                            = CharRange::Inclusive(0x2000, 0x206F),
            Superscripts_and_Subscripts                    = CharRange::Inclusive(0x2070, 0x209F),
            Currency_Symbols                               = CharRange::Inclusive(0x20A0, 0x20CF),
            Combining_Diacritical_Marks_for_Symbols        = CharRange::Inclusive(0x20D0, 0x20FF),
            Letterlike_Symbols                             = CharRange::Inclusive(0x2100, 0x214F),
            Number_Forms                                   = CharRange::Inclusive(0x2150, 0x218F),
            Arrows                                         = CharRange::Inclusive(0x2190, 0x21FF),
            Mathematical_Operators                         = CharRange::Inclusive(0x2200, 0x22FF),
            Miscellaneous_Technical                        = CharRange::Inclusive(0x2300, 0x23FF),
            Control_Pictures                               = CharRange::Inclusive(0x2400, 0x243F),
            Optical_Character_Recognition                  = CharRange::Inclusive(0x2440, 0x245F),
            Enclosed_Alphanumerics                         = CharRange::Inclusive(0x2460, 0x24FF),
            Box_Drawing                                    = CharRange::Inclusive(0x2500, 0x257F),
            Block_Elements                                 = CharRange::Inclusive(0x2580, 0x259F),
            Geometric_Shapes                               = CharRange::Inclusive(0x25A0, 0x25FF),
            Miscellaneous_Symbols                          = CharRange::Inclusive(0x2600, 0x26FF),
            Dingbats                                       = CharRange::Inclusive(0x2700, 0x27BF),
            Miscellaneous_Mathematical_Symbols_A           = CharRange::Inclusive(0x27C0, 0x27EF),
            Supplemental_Arrows_A                          = CharRange::Inclusive(0x27F0, 0x27FF),
            Braille_Patterns                               = CharRange::Inclusive(0x2800, 0x28FF),
            Supplemental_Arrows_B                          = CharRange::Inclusive(0x2900, 0x297F),
            Miscellaneous_Mathematical_Symbols_B           = CharRange::Inclusive(0x2980, 0x29FF),
            Supplemental_Mathematical_Operators            = CharRange::Inclusive(0x2A00, 0x2AFF),
            Miscellaneous_Symbols_and_Arrows               = CharRange::Inclusive(0x2B00, 0x2BFF),
            Glagolitic                                     = CharRange::Inclusive(0x2C00, 0x2C5F),
            Latin_Extended_C                               = CharRange::Inclusive(0x2C60, 0x2C7F),
            Coptic                                         = CharRange::Inclusive(0x2C80, 0x2CFF),
            Georgian_Supplement                            = CharRange::Inclusive(0x2D00, 0x2D2F),
            Tifinagh                                       = CharRange::Inclusive(0x2D30, 0x2D7F),
            Ethiopic_Extended                              = CharRange::Inclusive(0x2D80, 0x2DDF),
            Cyrillic_Extended_A                            = CharRange::Inclusive(0x2DE0, 0x2DFF),
            Supplemental_Punctuation                       = CharRange::Inclusive(0x2E00, 0x2E7F),
            CJK_Radicals_Supplement                        = CharRange::Inclusive(0x2E80, 0x2EFF),
            Kangxi_Radicals                                = CharRange::Inclusive(0x2F00, 0x2FDF),
            Ideographic_Description_Characters             = CharRange::Inclusive(0x2FF0, 0x2FFF),
            CJK_Symbols_and_Punctuation                    = CharRange::Inclusive(0x3000, 0x303F),
            Hiragana                                       = CharRange::Inclusive(0x3040, 0x309F),
            Katakana                                       = CharRange::Inclusive(0x30A0, 0x30FF),
            Bopomofo                                       = CharRange::Inclusive(0x3100, 0x312F),
            Hangul_Compatibility_Jamo                      = CharRange::Inclusive(0x3130, 0x318F),
            Kanbun                                         = CharRange::Inclusive(0x3190, 0x319F),
            Bopomofo_Extended                              = CharRange::Inclusive(0x31A0, 0x31BF),
            CJK_Strokes                                    = CharRange::Inclusive(0x31C0, 0x31EF),
            Katakana_Phonetic_Extensions                   = CharRange::Inclusive(0x31F0, 0x31FF),
            Enclosed_CJK_Letters_and_Months                = CharRange::Inclusive(0x3200, 0x32FF),
            CJK_Compatibility                              = CharRange::Inclusive(0x3300, 0x33FF),
            CJK_Unified_Ideographs_Extension_A             = CharRange::Inclusive(0x3400, 0x4DBF),
            Yijing_Hexagram_Symbols                        = CharRange::Inclusive(0x4DC0, 0x4DFF),
            CJK_Unified_Ideographs                         = CharRange::Inclusive(0x4E00, 0x9FFF),
            Yi_Syllables                                   = CharRange::Inclusive(0xA000, 0xA48F),
            Yi_Radicals                                    = CharRange::Inclusive(0xA490, 0xA4CF),
            Lisu                                           = CharRange::Inclusive(0xA4D0, 0xA4FF),
            Vai                                            = CharRange::Inclusive(0xA500, 0xA63F),
            Cyrillic_Extended_B                            = CharRange::Inclusive(0xA640, 0xA69F),
            Bamum                                          = CharRange::Inclusive(0xA6A0, 0xA6FF),
            Modifier_Tone_Letters                          = CharRange::Inclusive(0xA700, 0xA71F),
            Latin_Extended_D                               = CharRange::Inclusive(0xA720, 0xA7FF),
            Syloti_Nagri                                   = CharRange::Inclusive(0xA800, 0xA82F),
            Common_Indic_Number_Forms                      = CharRange::Inclusive(0xA830, 0xA83F),
            Phags_pa                                       = CharRange::Inclusive(0xA840, 0xA87F),
            Saurashtra                                     = CharRange::Inclusive(0xA880, 0xA8DF),
            Devanagari_Extended                            = CharRange::Inclusive(0xA8E0, 0xA8FF),
            Kayah_Li                                       = CharRange::Inclusive(0xA900, 0xA92F),
            Rejang                                         = CharRange::Inclusive(0xA930, 0xA95F),
            Hangul_Jamo_Extended_A                         = CharRange::Inclusive(0xA960, 0xA97F),
            Javanese                                       = CharRange::Inclusive(0xA980, 0xA9DF),
            Myanmar_Extended_B                             = CharRange::Inclusive(0xA9E0, 0xA9FF),
            Cham                                           = CharRange::Inclusive(0xAA00, 0xAA5F),
            Myanmar_Extended_A                             = CharRange::Inclusive(0xAA60, 0xAA7F),
            Tai_Viet                                       = CharRange::Inclusive(0xAA80, 0xAADF),
            Meetei_Mayek_Extensions                        = CharRange::Inclusive(0xAAE0, 0xAAFF),
            Ethiopic_Extended_A                            = CharRange::Inclusive(0xAB00, 0xAB2F),
            Latin_Extended_E                               = CharRange::Inclusive(0xAB30, 0xAB6F),
            Cherokee_Supplement                            = CharRange::Inclusive(0xAB70, 0xABBF),
            Meetei_Mayek                                   = CharRange::Inclusive(0xABC0, 0xABFF),
            Hangul_Syllables                               = CharRange::Inclusive(0xAC00, 0xD7AF),
            Hangul_Jamo_Extended_B                         = CharRange::Inclusive(0xD7B0, 0xD7FF),
            High_Surrogates                                = CharRange::Inclusive(0xD800, 0xDB7F),
            High_Private_Use_Surrogates                    = CharRange::Inclusive(0xDB80, 0xDBFF),
            Low_Surrogates                                 = CharRange::Inclusive(0xDC00, 0xDFFF),
            Private_Use_Area                               = CharRange::Inclusive(0xE000, 0xF8FF),
            CJK_Compatibility_Ideographs                   = CharRange::Inclusive(0xF900, 0xFAFF),
            Alphabetic_Presentation_Forms                  = CharRange::Inclusive(0xFB00, 0xFB4F),
            Arabic_Presentation_Forms_A                    = CharRange::Inclusive(0xFB50, 0xFDFF),
            Variation_Selectors                            = CharRange::Inclusive(0xFE00, 0xFE0F),
            Vertical_Forms                                 = CharRange::Inclusive(0xFE10, 0xFE1F),
            Combining_Half_Marks                           = CharRange::Inclusive(0xFE20, 0xFE2F),
            CJK_Compatibility_Forms                        = CharRange::Inclusive(0xFE30, 0xFE4F),
            Small_Form_Variants                            = CharRange::Inclusive(0xFE50, 0xFE6F),
            Arabic_Presentation_Forms_B                    = CharRange::Inclusive(0xFE70, 0xFEFF),
            Halfwidth_and_Fullwidth_Forms                  = CharRange::Inclusive(0xFF00, 0xFFEF),
            Specials                                       = CharRange::Inclusive(0xFFF0, 0xFFFF),
            Linear_B_Syllabary                             = CharRange::Inclusive(0x10000, 0x1007F),
            Linear_B_Ideograms                             = CharRange::Inclusive(0x10080, 0x100FF),
            Aegean_Numbers                                 = CharRange::Inclusive(0x10100, 0x1013F),
            Ancient_Greek_Numbers                          = CharRange::Inclusive(0x10140, 0x1018F),
            Ancient_Symbols                                = CharRange::Inclusive(0x10190, 0x101CF),
            Phaistos_Disc                                  = CharRange::Inclusive(0x101D0, 0x101FF),
            Lycian                                         = CharRange::Inclusive(0x10280, 0x1029F),
            Carian                                         = CharRange::Inclusive(0x102A0, 0x102DF),
            Coptic_Epact_Numbers                           = CharRange::Inclusive(0x102E0, 0x102FF),
            Old_Italic                                     = CharRange::Inclusive(0x10300, 0x1032F),
            Gothic                                         = CharRange::Inclusive(0x10330, 0x1034F),
            Old_Permic                                     = CharRange::Inclusive(0x10350, 0x1037F),
            Ugaritic                                       = CharRange::Inclusive(0x10380, 0x1039F),
            Old_Persian                                    = CharRange::Inclusive(0x103A0, 0x103DF),
            Deseret                                        = CharRange::Inclusive(0x10400, 0x1044F),
            Shavian                                        = CharRange::Inclusive(0x10450, 0x1047F),
            Osmanya                                        = CharRange::Inclusive(0x10480, 0x104AF),
            Osage                                          = CharRange::Inclusive(0x104B0, 0x104FF),
            Elbasan                                        = CharRange::Inclusive(0x10500, 0x1052F),
            Caucasian_Albanian                             = CharRange::Inclusive(0x10530, 0x1056F),
            Linear_A                                       = CharRange::Inclusive(0x10600, 0x1077F),
            Cypriot_Syllabary                              = CharRange::Inclusive(0x10800, 0x1083F),
            Imperial_Aramaic                               = CharRange::Inclusive(0x10840, 0x1085F),
            Palmyrene                                      = CharRange::Inclusive(0x10860, 0x1087F),
            Nabataean                                      = CharRange::Inclusive(0x10880, 0x108AF),
            Hatran                                         = CharRange::Inclusive(0x108E0, 0x108FF),
            Phoenician                                     = CharRange::Inclusive(0x10900, 0x1091F),
            Lydian                                         = CharRange::Inclusive(0x10920, 0x1093F),
            Meroitic_Hieroglyphs                           = CharRange::Inclusive(0x10980, 0x1099F),
            Meroitic_Cursive                               = CharRange::Inclusive(0x109A0, 0x109FF),
            Kharoshthi                                     = CharRange::Inclusive(0x10A00, 0x10A5F),
            Old_South_Arabian                              = CharRange::Inclusive(0x10A60, 0x10A7F),
            Old_North_Arabian                              = CharRange::Inclusive(0x10A80, 0x10A9F),
            Manichaean                                     = CharRange::Inclusive(0x10AC0, 0x10AFF),
            Avestan                                        = CharRange::Inclusive(0x10B00, 0x10B3F),
            Inscriptional_Parthian                         = CharRange::Inclusive(0x10B40, 0x10B5F),
            Inscriptional_Pahlavi                          = CharRange::Inclusive(0x10B60, 0x10B7F),
            Psalter_Pahlavi                                = CharRange::Inclusive(0x10B80, 0x10BAF),
            Old_Turkic                                     = CharRange::Inclusive(0x10C00, 0x10C4F),
            Old_Hungarian                                  = CharRange::Inclusive(0x10C80, 0x10CFF),
            Hanifi_Rohingya                                = CharRange::Inclusive(0x10D00, 0x10D3F),
            Rumi_Numeral_Symbols                           = CharRange::Inclusive(0x10E60, 0x10E7F),
            Old_Sogdian                                    = CharRange::Inclusive(0x10F00, 0x10F2F),
            Sogdian                                        = CharRange::Inclusive(0x10F30, 0x10F6F),
            Brahmi                                         = CharRange::Inclusive(0x11000, 0x1107F),
            Kaithi                                         = CharRange::Inclusive(0x11080, 0x110CF),
            Sora_Sompeng                                   = CharRange::Inclusive(0x110D0, 0x110FF),
            Chakma                                         = CharRange::Inclusive(0x11100, 0x1114F),
            Mahajani                                       = CharRange::Inclusive(0x11150, 0x1117F),
            Sharada                                        = CharRange::Inclusive(0x11180, 0x111DF),
            Sinhala_Archaic_Numbers                        = CharRange::Inclusive(0x111E0, 0x111FF),
            Khojki                                         = CharRange::Inclusive(0x11200, 0x1124F),
            Multani                                        = CharRange::Inclusive(0x11280, 0x112AF),
            Khudawadi                                      = CharRange::Inclusive(0x112B0, 0x112FF),
            Grantha                                        = CharRange::Inclusive(0x11300, 0x1137F),
            Newa                                           = CharRange::Inclusive(0x11400, 0x1147F),
            Tirhuta                                        = CharRange::Inclusive(0x11480, 0x114DF),
            Siddham                                        = CharRange::Inclusive(0x11580, 0x115FF),
            Modi                                           = CharRange::Inclusive(0x11600, 0x1165F),
            Mongolian_Supplement                           = CharRange::Inclusive(0x11660, 0x1167F),
            Takri                                          = CharRange::Inclusive(0x11680, 0x116CF),
            Ahom                                           = CharRange::Inclusive(0x11700, 0x1173F),
            Dogra                                          = CharRange::Inclusive(0x11800, 0x1184F),
            Warang_Citi                                    = CharRange::Inclusive(0x118A0, 0x118FF),
            Zanabazar_Square                               = CharRange::Inclusive(0x11A00, 0x11A4F),
            Soyombo                                        = CharRange::Inclusive(0x11A50, 0x11AAF),
            Pau_Cin_Hau                                    = CharRange::Inclusive(0x11AC0, 0x11AFF),
            Bhaiksuki                                      = CharRange::Inclusive(0x11C00, 0x11C6F),
            Marchen                                        = CharRange::Inclusive(0x11C70, 0x11CBF),
            Masaram_Gondi                                  = CharRange::Inclusive(0x11D00, 0x11D5F),
            Gunjala_Gondi                                  = CharRange::Inclusive(0x11D60, 0x11DAF),
            Makasar                                        = CharRange::Inclusive(0x11EE0, 0x11EFF),
            Cuneiform                                      = CharRange::Inclusive(0x12000, 0x123FF),
            Cuneiform_Numbers_and_Punctuation              = CharRange::Inclusive(0x12400, 0x1247F),
            Early_Dynastic_Cuneiform                       = CharRange::Inclusive(0x12480, 0x1254F),
            Egyptian_Hieroglyphs                           = CharRange::Inclusive(0x13000, 0x1342F),
            Anatolian_Hieroglyphs                          = CharRange::Inclusive(0x14400, 0x1467F),
            Bamum_Supplement                               = CharRange::Inclusive(0x16800, 0x16A3F),
            Mro                                            = CharRange::Inclusive(0x16A40, 0x16A6F),
            Bassa_Vah                                      = CharRange::Inclusive(0x16AD0, 0x16AFF),
            Pahawh_Hmong                                   = CharRange::Inclusive(0x16B00, 0x16B8F),
            Medefaidrin                                    = CharRange::Inclusive(0x16E40, 0x16E9F),
            Miao                                           = CharRange::Inclusive(0x16F00, 0x16F9F),
            Ideographic_Symbols_and_Punctuation            = CharRange::Inclusive(0x16FE0, 0x16FFF),
            Tangut                                         = CharRange::Inclusive(0x17000, 0x187FF),
            Tangut_Components                              = CharRange::Inclusive(0x18800, 0x18AFF),
            Kana_Supplement                                = CharRange::Inclusive(0x1B000, 0x1B0FF),
            Kana_Extended_A                                = CharRange::Inclusive(0x1B100, 0x1B12F),
            Nushu                                          = CharRange::Inclusive(0x1B170, 0x1B2FF),
            Duployan                                       = CharRange::Inclusive(0x1BC00, 0x1BC9F),
            Shorthand_Format_Controls                      = CharRange::Inclusive(0x1BCA0, 0x1BCAF),
            Byzantine_Musical_Symbols                      = CharRange::Inclusive(0x1D000, 0x1D0FF),
            Musical_Symbols                                = CharRange::Inclusive(0x1D100, 0x1D1FF),
            Ancient_Greek_Musical_Notation                 = CharRange::Inclusive(0x1D200, 0x1D24F),
            Mayan_Numerals                                 = CharRange::Inclusive(0x1D2E0, 0x1D2FF),
            Tai_Xuan_Jing_Symbols                          = CharRange::Inclusive(0x1D300, 0x1D35F),
            Counting_Rod_Numerals                          = CharRange::Inclusive(0x1D360, 0x1D37F),
            Mathematical_Alphanumeric_Symbols              = CharRange::Inclusive(0x1D400, 0x1D7FF),
            Sutton_SignWriting                             = CharRange::Inclusive(0x1D800, 0x1DAAF),
            Glagolitic_Supplement                          = CharRange::Inclusive(0x1E000, 0x1E02F),
            Mende_Kikakui                                  = CharRange::Inclusive(0x1E800, 0x1E8DF),
            Adlam                                          = CharRange::Inclusive(0x1E900, 0x1E95F),
            Indic_Siyaq_Numbers                            = CharRange::Inclusive(0x1EC70, 0x1ECBF),
            Arabic_Mathematical_Alphabetic_Symbols         = CharRange::Inclusive(0x1EE00, 0x1EEFF),
            Mahjong_Tiles                                  = CharRange::Inclusive(0x1F000, 0x1F02F),
            Domino_Tiles                                   = CharRange::Inclusive(0x1F030, 0x1F09F),
            Playing_Cards                                  = CharRange::Inclusive(0x1F0A0, 0x1F0FF),
            Enclosed_Alphanumeric_Supplement               = CharRange::Inclusive(0x1F100, 0x1F1FF),
            Enclosed_Ideographic_Supplement                = CharRange::Inclusive(0x1F200, 0x1F2FF),
            Miscellaneous_Symbols_and_Pictographs          = CharRange::Inclusive(0x1F300, 0x1F5FF),
            Emoticons                                      = CharRange::Inclusive(0x1F600, 0x1F64F),
            Ornamental_Dingbats                            = CharRange::Inclusive(0x1F650, 0x1F67F),
            Transport_and_Map_Symbols                      = CharRange::Inclusive(0x1F680, 0x1F6FF),
            Alchemical_Symbols                             = CharRange::Inclusive(0x1F700, 0x1F77F),
            Geometric_Shapes_Extended                      = CharRange::Inclusive(0x1F780, 0x1F7FF),
            Supplemental_Arrows_C                          = CharRange::Inclusive(0x1F800, 0x1F8FF),
            Supplemental_Symbols_and_Pictographs           = CharRange::Inclusive(0x1F900, 0x1F9FF),
            Chess_Symbols                                  = CharRange::Inclusive(0x1FA00, 0x1FA6F),
            CJK_Unified_Ideographs_Extension_B             = CharRange::Inclusive(0x20000, 0x2A6DF),
            CJK_Unified_Ideographs_Extension_C             = CharRange::Inclusive(0x2A700, 0x2B73F),
            CJK_Unified_Ideographs_Extension_D             = CharRange::Inclusive(0x2B740, 0x2B81F),
            CJK_Unified_Ideographs_Extension_E             = CharRange::Inclusive(0x2B820, 0x2CEAF),
            CJK_Unified_Ideographs_Extension_F             = CharRange::Inclusive(0x2CEB0, 0x2EBEF),
            CJK_Compatibility_Ideographs_Supplement        = CharRange::Inclusive(0x2F800, 0x2FA1F),
            Tags                                           = CharRange::Inclusive(0xE0000, 0xE007F),
            Variation_Selectors_Supplement                 = CharRange::Inclusive(0xE0100, 0xE01EF),
            Supplementary_Private_Use_Area_A               = CharRange::Inclusive(0xF0000, 0xFFFFF),
            Supplementary_Private_Use_Area_B               = CharRange::Inclusive(0x100000, 0x10FFFF);
    }
}

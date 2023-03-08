/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_TYPE_UNICODE_BLOCK_HPP
#define ANTKEEPER_TYPE_UNICODE_BLOCK_HPP

namespace type {
namespace unicode {

/**
 * Contiguous range of Unicode characters.
 *
 * @see https://en.wikipedia.org/wiki/Unicode_block
 * @see http://jrgraphix.net/research/unicode.php
 */
struct block
{
	/// First character in the block.
	char32_t first;
	
	/// Last character in the block.
	char32_t last;
};

/// Basic Latin[g]
inline constexpr block basic_latin{0x0, 0x7F};

/// Latin-1 Supplement[h]
inline constexpr block latin_1_supplement{0x80, 0xFF};

/// Latin Extended-A
inline constexpr block latin_extended_a{0x0100, 0x017F};

/// Latin Extended-B
inline constexpr block latin_extended_b{0x0180, 0x024F};

/// IPA Extensions
inline constexpr block ipa_extensions{0x0250, 0x02AF};

/// Spacing Modifier Letters
inline constexpr block spacing_modifier_letters{0x02B0, 0x02FF};

/// Combining Diacritical Marks
inline constexpr block combining_diacritical_marks{0x0300, 0x036F};

/// Greek and Coptic
inline constexpr block greek_and_coptic{0x0370, 0x03FF};

/// Cyrillic
inline constexpr block cyrillic{0x0400, 0x04FF};

/// Cyrillic Supplement
inline constexpr block cyrillic_supplement{0x0500, 0x052F};

/// Armenian
inline constexpr block armenian{0x0530, 0x058F};

/// Hebrew
inline constexpr block hebrew{0x0590, 0x05FF};

/// Arabic
inline constexpr block arabic{0x0600, 0x06FF};

/// Syriac
inline constexpr block syriac{0x0700, 0x074F};

/// Arabic Supplement
inline constexpr block arabic_supplement{0x0750, 0x077F};

/// Thaana
inline constexpr block thaana{0x0780, 0x07BF};

/// NKo
inline constexpr block nko{0x07C0, 0x07FF};

/// Samaritan
inline constexpr block samaritan{0x0800, 0x083F};

/// Mandaic
inline constexpr block mandaic{0x0840, 0x085F};

/// Syriac Supplement
inline constexpr block syriac_supplement{0x0860, 0x086F};

/// Arabic Extended-B
inline constexpr block arabic_extended_b{0x0870, 0x089F};

/// Arabic Extended-A
inline constexpr block arabic_extended_a{0x08A0, 0x08FF};

/// Devanagari
inline constexpr block devanagari{0x0900, 0x097F};

/// Bengali
inline constexpr block bengali{0x0980, 0x09FF};

/// Gurmukhi
inline constexpr block gurmukhi{0x0A00, 0x0A7F};

/// Gujarati
inline constexpr block gujarati{0x0A80, 0x0AFF};

/// Oriya
inline constexpr block oriya{0x0B00, 0x0B7F};

/// Tamil
inline constexpr block tamil{0x0B80, 0x0BFF};

/// Telugu
inline constexpr block telugu{0x0C00, 0x0C7F};

/// Kannada
inline constexpr block kannada{0x0C80, 0x0CFF};

/// Malayalam
inline constexpr block malayalam{0x0D00, 0x0D7F};

/// Sinhala
inline constexpr block sinhala{0x0D80, 0x0DFF};

/// Thai
inline constexpr block thai{0x0E00, 0x0E7F};

/// Lao
inline constexpr block lao{0x0E80, 0x0EFF};

/// Tibetan
inline constexpr block tibetan{0x0F00, 0x0FFF};

/// Myanmar
inline constexpr block myanmar{0x1000, 0x109F};

/// Georgian
inline constexpr block georgian{0x10A0, 0x10FF};

/// Hangul Jamo
inline constexpr block hangul_jamo{0x1100, 0x11FF};

/// Ethiopic
inline constexpr block ethiopic{0x1200, 0x137F};

/// Ethiopic Supplement
inline constexpr block ethiopic_supplement{0x1380, 0x139F};

/// Cherokee
inline constexpr block cherokee{0x13A0, 0x13FF};

/// Unified Canadian Aboriginal Syllabics
inline constexpr block unified_canadian_aboriginal_syllabics{0x1400, 0x167F};

/// Ogham
inline constexpr block ogham{0x1680, 0x169F};

/// Runic
inline constexpr block runic{0x16A0, 0x16FF};

/// Tagalog
inline constexpr block tagalog{0x1700, 0x171F};

/// Hanunoo
inline constexpr block hanunoo{0x1720, 0x173F};

/// Buhid
inline constexpr block buhid{0x1740, 0x175F};

/// Tagbanwa
inline constexpr block tagbanwa{0x1760, 0x177F};

/// Khmer
inline constexpr block khmer{0x1780, 0x17FF};

/// Mongolian
inline constexpr block mongolian{0x1800, 0x18AF};

/// Unified Canadian Aboriginal Syllabics Extended
inline constexpr block unified_canadian_aboriginal_syllabics_extended{0x18B0, 0x18FF};

/// Limbu
inline constexpr block limbu{0x1900, 0x194F};

/// Tai Le
inline constexpr block tai_le{0x1950, 0x197F};

/// New Tai Lue
inline constexpr block new_tai_lue{0x1980, 0x19DF};

/// Khmer Symbols
inline constexpr block khmer_symbols{0x19E0, 0x19FF};

/// Buginese
inline constexpr block buginese{0x1A00, 0x1A1F};

/// Tai Tham
inline constexpr block tai_tham{0x1A20, 0x1AAF};

/// Combining Diacritical Marks Extended
inline constexpr block combining_diacritical_marks_extended{0x1AB0, 0x1AFF};

/// Balinese
inline constexpr block balinese{0x1B00, 0x1B7F};

/// Sundanese
inline constexpr block sundanese{0x1B80, 0x1BBF};

/// Batak
inline constexpr block batak{0x1BC0, 0x1BFF};

/// Lepcha
inline constexpr block lepcha{0x1C00, 0x1C4F};

/// Ol Chiki
inline constexpr block ol_chiki{0x1C50, 0x1C7F};

/// Cyrillic Extended-C
inline constexpr block cyrillic_extended_c{0x1C80, 0x1C8F};

/// Georgian Extended
inline constexpr block georgian_extended{0x1C90, 0x1CBF};

/// Sundanese Supplement
inline constexpr block sundanese_supplement{0x1CC0, 0x1CCF};

/// Vedic Extensions
inline constexpr block vedic_extensions{0x1CD0, 0x1CFF};

/// Phonetic Extensions
inline constexpr block phonetic_extensions{0x1D00, 0x1D7F};

/// Phonetic Extensions Supplement
inline constexpr block phonetic_extensions_supplement{0x1D80, 0x1DBF};

/// Combining Diacritical Marks Supplement
inline constexpr block combining_diacritical_marks_supplement{0x1DC0, 0x1DFF};

/// Latin Extended Additional
inline constexpr block latin_extended_additional{0x1E00, 0x1EFF};

/// Greek Extended
inline constexpr block greek_extended{0x1F00, 0x1FFF};

/// General Punctuation
inline constexpr block general_punctuation{0x2000, 0x206F};

/// Superscripts and Subscripts
inline constexpr block superscripts_and_subscripts{0x2070, 0x209F};

/// Currency Symbols
inline constexpr block currency_symbols{0x20A0, 0x20CF};

/// Combining Diacritical Marks for Symbols
inline constexpr block combining_diacritical_marks_for_symbols{0x20D0, 0x20FF};

/// Letterlike Symbols
inline constexpr block letterlike_symbols{0x2100, 0x214F};

/// Number Forms
inline constexpr block number_forms{0x2150, 0x218F};

/// Arrows
inline constexpr block arrows{0x2190, 0x21FF};

/// Mathematical Operators
inline constexpr block mathematical_operators{0x2200, 0x22FF};

/// Miscellaneous Technical
inline constexpr block miscellaneous_technical{0x2300, 0x23FF};

/// Control Pictures
inline constexpr block control_pictures{0x2400, 0x243F};

/// Optical Character Recognition
inline constexpr block optical_character_recognition{0x2440, 0x245F};

/// Enclosed Alphanumerics
inline constexpr block enclosed_alphanumerics{0x2460, 0x24FF};

/// Box Drawing
inline constexpr block box_drawing{0x2500, 0x257F};

/// Block Elements
inline constexpr block block_elements{0x2580, 0x259F};

/// Geometric Shapes
inline constexpr block geometric_shapes{0x25A0, 0x25FF};

/// Miscellaneous Symbols
inline constexpr block miscellaneous_symbols{0x2600, 0x26FF};

/// Dingbats
inline constexpr block dingbats{0x2700, 0x27BF};

/// Miscellaneous Mathematical Symbols-A
inline constexpr block miscellaneous_mathematical_symbols_a{0x27C0, 0x27EF};

/// Supplemental Arrows-A
inline constexpr block supplemental_arrows_a{0x27F0, 0x27FF};

/// Braille Patterns
inline constexpr block braille_patterns{0x2800, 0x28FF};

/// Supplemental Arrows-B
inline constexpr block supplemental_arrows_b{0x2900, 0x297F};

/// Miscellaneous Mathematical Symbols-B
inline constexpr block miscellaneous_mathematical_symbols_b{0x2980, 0x29FF};

/// Supplemental Mathematical Operators
inline constexpr block supplemental_mathematical_operators{0x2A00, 0x2AFF};

/// Miscellaneous Symbols and Arrows
inline constexpr block miscellaneous_symbols_and_arrows{0x2B00, 0x2BFF};

/// Glagolitic
inline constexpr block glagolitic{0x2C00, 0x2C5F};

/// Latin Extended-C
inline constexpr block latin_extended_c{0x2C60, 0x2C7F};

/// Coptic
inline constexpr block coptic{0x2C80, 0x2CFF};

/// Georgian Supplement
inline constexpr block georgian_supplement{0x2D00, 0x2D2F};

/// Tifinagh
inline constexpr block tifinagh{0x2D30, 0x2D7F};

/// Ethiopic Extended
inline constexpr block ethiopic_extended{0x2D80, 0x2DDF};

/// Cyrillic Extended-A
inline constexpr block cyrillic_extended_a{0x2DE0, 0x2DFF};

/// Supplemental Punctuation
inline constexpr block supplemental_punctuation{0x2E00, 0x2E7F};

/// CJK Radicals Supplement
inline constexpr block cjk_radicals_supplement{0x2E80, 0x2EFF};

/// Kangxi Radicals
inline constexpr block kangxi_radicals{0x2F00, 0x2FDF};

/// Ideographic Description Characters
inline constexpr block ideographic_description_characters{0x2FF0, 0x2FFF};

/// CJK Symbols and Punctuation
inline constexpr block cjk_symbols_and_punctuation{0x3000, 0x303F};

/// Hiragana
inline constexpr block hiragana{0x3040, 0x309F};

/// Katakana
inline constexpr block katakana{0x30A0, 0x30FF};

/// Bopomofo
inline constexpr block bopomofo{0x3100, 0x312F};

/// Hangul Compatibility Jamo
inline constexpr block hangul_compatibility_jamo{0x3130, 0x318F};

/// Kanbun
inline constexpr block kanbun{0x3190, 0x319F};

/// Bopomofo Extended
inline constexpr block bopomofo_extended{0x31A0, 0x31BF};

/// CJK Strokes
inline constexpr block cjk_strokes{0x31C0, 0x31EF};

/// Katakana Phonetic Extensions
inline constexpr block katakana_phonetic_extensions{0x31F0, 0x31FF};

/// Enclosed CJK Letters and Months
inline constexpr block enclosed_cjk_letters_and_months{0x3200, 0x32FF};

/// CJK Compatibility
inline constexpr block cjk_compatibility{0x3300, 0x33FF};

/// CJK Unified Ideographs Extension A
inline constexpr block cjk_unified_ideographs_extension_a{0x3400, 0x4DBF};

/// Yijing Hexagram Symbols
inline constexpr block yijing_hexagram_symbols{0x4DC0, 0x4DFF};

/// CJK Unified Ideographs
inline constexpr block cjk_unified_ideographs{0x4E00, 0x9FFF};

/// Yi Syllables
inline constexpr block yi_syllables{0xA000, 0xA48F};

/// Yi Radicals
inline constexpr block yi_radicals{0xA490, 0xA4CF};

/// Lisu
inline constexpr block lisu{0xA4D0, 0xA4FF};

/// Vai
inline constexpr block vai{0xA500, 0xA63F};

/// Cyrillic Extended-B
inline constexpr block cyrillic_extended_b{0xA640, 0xA69F};

/// Bamum
inline constexpr block bamum{0xA6A0, 0xA6FF};

/// Modifier Tone Letters
inline constexpr block modifier_tone_letters{0xA700, 0xA71F};

/// Latin Extended-D
inline constexpr block latin_extended_d{0xA720, 0xA7FF};

/// Syloti Nagri
inline constexpr block syloti_nagri{0xA800, 0xA82F};

/// Common Indic Number Forms
inline constexpr block common_indic_number_forms{0xA830, 0xA83F};

/// Phags-pa
inline constexpr block phags_pa{0xA840, 0xA87F};

/// Saurashtra
inline constexpr block saurashtra{0xA880, 0xA8DF};

/// Devanagari Extended
inline constexpr block devanagari_extended{0xA8E0, 0xA8FF};

/// Kayah Li
inline constexpr block kayah_li{0xA900, 0xA92F};

/// Rejang
inline constexpr block rejang{0xA930, 0xA95F};

/// Hangul Jamo Extended-A
inline constexpr block hangul_jamo_extended_a{0xA960, 0xA97F};

/// Javanese
inline constexpr block javanese{0xA980, 0xA9DF};

/// Myanmar Extended-B
inline constexpr block myanmar_extended_b{0xA9E0, 0xA9FF};

/// Cham
inline constexpr block cham{0xAA00, 0xAA5F};

/// Myanmar Extended-A
inline constexpr block myanmar_extended_a{0xAA60, 0xAA7F};

/// Tai Viet
inline constexpr block tai_viet{0xAA80, 0xAADF};

/// Meetei Mayek Extensions
inline constexpr block meetei_mayek_extensions{0xAAE0, 0xAAFF};

/// Ethiopic Extended-A
inline constexpr block ethiopic_extended_a{0xAB00, 0xAB2F};

/// Latin Extended-E
inline constexpr block latin_extended_e{0xAB30, 0xAB6F};

/// Cherokee Supplement
inline constexpr block cherokee_supplement{0xAB70, 0xABBF};

/// Meetei Mayek
inline constexpr block meetei_mayek{0xABC0, 0xABFF};

/// Hangul Syllables
inline constexpr block hangul_syllables{0xAC00, 0xD7AF};

/// Hangul Jamo Extended-B
inline constexpr block hangul_jamo_extended_b{0xD7B0, 0xD7FF};

/// High Surrogates
inline constexpr block high_surrogates{0xD800, 0xDB7F};

/// High Private Use Surrogates
inline constexpr block high_private_use_surrogates{0xDB80, 0xDBFF};

/// Low Surrogates
inline constexpr block low_surrogates{0xDC00, 0xDFFF};

/// Private Use Area
inline constexpr block private_use_area{0xE000, 0xF8FF};

/// CJK Compatibility Ideographs
inline constexpr block cjk_compatibility_ideographs{0xF900, 0xFAFF};

/// Alphabetic Presentation Forms
inline constexpr block alphabetic_presentation_forms{0xFB00, 0xFB4F};

/// Arabic Presentation Forms-A
inline constexpr block arabic_presentation_forms_a{0xFB50, 0xFDFF};

/// Variation Selectors
inline constexpr block variation_selectors{0xFE00, 0xFE0F};

/// Vertical Forms
inline constexpr block vertical_forms{0xFE10, 0xFE1F};

/// Combining Half Marks
inline constexpr block combining_half_marks{0xFE20, 0xFE2F};

/// CJK Compatibility Forms
inline constexpr block cjk_compatibility_forms{0xFE30, 0xFE4F};

/// Small Form Variants
inline constexpr block small_form_variants{0xFE50, 0xFE6F};

/// Arabic Presentation Forms-B
inline constexpr block arabic_presentation_forms_b{0xFE70, 0xFEFF};

/// Halfwidth and Fullwidth Forms
inline constexpr block halfwidth_and_fullwidth_forms{0xFF00, 0xFFEF};

/// Specials
inline constexpr block specials{0xFFF0, 0xFFFF};

/// Linear B Syllabary
inline constexpr block linear_b_syllabary{0x10000, 0x1007F};

/// Linear B Ideograms
inline constexpr block linear_b_ideograms{0x10080, 0x100FF};

/// Aegean Numbers
inline constexpr block aegean_numbers{0x10100, 0x1013F};

/// Ancient Greek Numbers
inline constexpr block ancient_greek_numbers{0x10140, 0x1018F};

/// Ancient Symbols
inline constexpr block ancient_symbols{0x10190, 0x101CF};

/// Phaistos Disc
inline constexpr block phaistos_disc{0x101D0, 0x101FF};

/// Lycian
inline constexpr block lycian{0x10280, 0x1029F};

/// Carian
inline constexpr block carian{0x102A0, 0x102DF};

/// Coptic Epact Numbers
inline constexpr block coptic_epact_numbers{0x102E0, 0x102FF};

/// Old Italic
inline constexpr block old_italic{0x10300, 0x1032F};

/// Gothic
inline constexpr block gothic{0x10330, 0x1034F};

/// Old Permic
inline constexpr block old_permic{0x10350, 0x1037F};

/// Ugaritic
inline constexpr block ugaritic{0x10380, 0x1039F};

/// Old Persian
inline constexpr block old_persian{0x103A0, 0x103DF};

/// Deseret
inline constexpr block deseret{0x10400, 0x1044F};

/// Shavian
inline constexpr block shavian{0x10450, 0x1047F};

/// Osmanya
inline constexpr block osmanya{0x10480, 0x104AF};

/// Osage
inline constexpr block osage{0x104B0, 0x104FF};

/// Elbasan
inline constexpr block elbasan{0x10500, 0x1052F};

/// Caucasian Albanian
inline constexpr block caucasian_albanian{0x10530, 0x1056F};

/// Vithkuqi
inline constexpr block vithkuqi{0x10570, 0x105BF};

/// Linear A
inline constexpr block linear_a{0x10600, 0x1077F};

/// Latin Extended-F
inline constexpr block latin_extended_f{0x10780, 0x107BF};

/// Cypriot Syllabary
inline constexpr block cypriot_syllabary{0x10800, 0x1083F};

/// Imperial Aramaic
inline constexpr block imperial_aramaic{0x10840, 0x1085F};

/// Palmyrene
inline constexpr block palmyrene{0x10860, 0x1087F};

/// Nabataean
inline constexpr block nabataean{0x10880, 0x108AF};

/// Hatran
inline constexpr block hatran{0x108E0, 0x108FF};

/// Phoenician
inline constexpr block phoenician{0x10900, 0x1091F};

/// Lydian
inline constexpr block lydian{0x10920, 0x1093F};

/// Meroitic Hieroglyphs
inline constexpr block meroitic_hieroglyphs{0x10980, 0x1099F};

/// Meroitic Cursive
inline constexpr block meroitic_cursive{0x109A0, 0x109FF};

/// Kharoshthi
inline constexpr block kharoshthi{0x10A00, 0x10A5F};

/// Old South Arabian
inline constexpr block old_south_arabian{0x10A60, 0x10A7F};

/// Old North Arabian
inline constexpr block old_north_arabian{0x10A80, 0x10A9F};

/// Manichaean
inline constexpr block manichaean{0x10AC0, 0x10AFF};

/// Avestan
inline constexpr block avestan{0x10B00, 0x10B3F};

/// Inscriptional Parthian
inline constexpr block inscriptional_parthian{0x10B40, 0x10B5F};

/// Inscriptional Pahlavi
inline constexpr block inscriptional_pahlavi{0x10B60, 0x10B7F};

/// Psalter Pahlavi
inline constexpr block psalter_pahlavi{0x10B80, 0x10BAF};

/// Old Turkic
inline constexpr block old_turkic{0x10C00, 0x10C4F};

/// Old Hungarian
inline constexpr block old_hungarian{0x10C80, 0x10CFF};

/// Hanifi Rohingya
inline constexpr block hanifi_rohingya{0x10D00, 0x10D3F};

/// Rumi Numeral Symbols
inline constexpr block rumi_numeral_symbols{0x10E60, 0x10E7F};

/// Yezidi
inline constexpr block yezidi{0x10E80, 0x10EBF};

/// Old Sogdian
inline constexpr block old_sogdian{0x10F00, 0x10F2F};

/// Sogdian
inline constexpr block sogdian{0x10F30, 0x10F6F};

/// Old Uyghur
inline constexpr block old_uyghur{0x10F70, 0x10FAF};

/// Chorasmian
inline constexpr block chorasmian{0x10FB0, 0x10FDF};

/// Elymaic
inline constexpr block elymaic{0x10FE0, 0x10FFF};

/// Brahmi
inline constexpr block brahmi{0x11000, 0x1107F};

/// Kaithi
inline constexpr block kaithi{0x11080, 0x110CF};

/// Sora Sompeng
inline constexpr block sora_sompeng{0x110D0, 0x110FF};

/// Chakma
inline constexpr block chakma{0x11100, 0x1114F};

/// Mahajani
inline constexpr block mahajani{0x11150, 0x1117F};

/// Sharada
inline constexpr block sharada{0x11180, 0x111DF};

/// Sinhala Archaic Numbers
inline constexpr block sinhala_archaic_numbers{0x111E0, 0x111FF};

/// Khojki
inline constexpr block khojki{0x11200, 0x1124F};

/// Multani
inline constexpr block multani{0x11280, 0x112AF};

/// Khudawadi
inline constexpr block khudawadi{0x112B0, 0x112FF};

/// Grantha
inline constexpr block grantha{0x11300, 0x1137F};

/// Newa
inline constexpr block newa{0x11400, 0x1147F};

/// Tirhuta
inline constexpr block tirhuta{0x11480, 0x114DF};

/// Siddham
inline constexpr block siddham{0x11580, 0x115FF};

/// Modi
inline constexpr block modi{0x11600, 0x1165F};

/// Mongolian Supplement
inline constexpr block mongolian_supplement{0x11660, 0x1167F};

/// Takri
inline constexpr block takri{0x11680, 0x116CF};

/// Ahom
inline constexpr block ahom{0x11700, 0x1174F};

/// Dogra
inline constexpr block dogra{0x11800, 0x1184F};

/// Warang Citi
inline constexpr block warang_citi{0x118A0, 0x118FF};

/// Dives Akuru
inline constexpr block dives_akuru{0x11900, 0x1195F};

/// Nandinagari
inline constexpr block nandinagari{0x119A0, 0x119FF};

/// Zanabazar Square
inline constexpr block zanabazar_square{0x11A00, 0x11A4F};

/// Soyombo
inline constexpr block soyombo{0x11A50, 0x11AAF};

/// Unified Canadian Aboriginal Syllabics Extended-A
inline constexpr block unified_canadian_aboriginal_syllabics_extended_a{0x11AB0, 0x11ABF};

/// Pau Cin Hau
inline constexpr block pau_cin_hau{0x11AC0, 0x11AFF};

/// Bhaiksuki
inline constexpr block bhaiksuki{0x11C00, 0x11C6F};

/// Marchen
inline constexpr block marchen{0x11C70, 0x11CBF};

/// Masaram Gondi
inline constexpr block masaram_gondi{0x11D00, 0x11D5F};

/// Gunjala Gondi
inline constexpr block gunjala_gondi{0x11D60, 0x11DAF};

/// Makasar
inline constexpr block makasar{0x11EE0, 0x11EFF};

/// Lisu Supplement
inline constexpr block lisu_supplement{0x11FB0, 0x11FBF};

/// Tamil Supplement
inline constexpr block tamil_supplement{0x11FC0, 0x11FFF};

/// Cuneiform
inline constexpr block cuneiform{0x12000, 0x123FF};

/// Cuneiform Numbers and Punctuation
inline constexpr block cuneiform_numbers_and_punctuation{0x12400, 0x1247F};

/// Early Dynastic Cuneiform
inline constexpr block early_dynastic_cuneiform{0x12480, 0x1254F};

/// Cypro-Minoan
inline constexpr block cypro_minoan{0x12F90, 0x12FFF};

/// Egyptian Hieroglyphs
inline constexpr block egyptian_hieroglyphs{0x13000, 0x1342F};

/// Egyptian Hieroglyph Format Controls
inline constexpr block egyptian_hieroglyph_format_controls{0x13430, 0x1343F};

/// Anatolian Hieroglyphs
inline constexpr block anatolian_hieroglyphs{0x14400, 0x1467F};

/// Bamum Supplement
inline constexpr block bamum_supplement{0x16800, 0x16A3F};

/// Mro
inline constexpr block mro{0x16A40, 0x16A6F};

/// Tangsa
inline constexpr block tangsa{0x16A70, 0x16ACF};

/// Bassa Vah
inline constexpr block bassa_vah{0x16AD0, 0x16AFF};

/// Pahawh Hmong
inline constexpr block pahawh_hmong{0x16B00, 0x16B8F};

/// Medefaidrin
inline constexpr block medefaidrin{0x16E40, 0x16E9F};

/// Miao
inline constexpr block miao{0x16F00, 0x16F9F};

/// Ideographic Symbols and Punctuation
inline constexpr block ideographic_symbols_and_punctuation{0x16FE0, 0x16FFF};

/// Tangut
inline constexpr block tangut{0x17000, 0x187FF};

/// Tangut Components
inline constexpr block tangut_components{0x18800, 0x18AFF};

/// Khitan Small Script
inline constexpr block khitan_small_script{0x18B00, 0x18CFF};

/// Tangut Supplement
inline constexpr block tangut_supplement{0x18D00, 0x18D7F};

/// Kana Extended-B
inline constexpr block kana_extended_b{0x1AFF0, 0x1AFFF};

/// Kana Supplement
inline constexpr block kana_supplement{0x1B000, 0x1B0FF};

/// Kana Extended-A
inline constexpr block kana_extended_a{0x1B100, 0x1B12F};

/// Small Kana Extension
inline constexpr block small_kana_extension{0x1B130, 0x1B16F};

/// Nushu
inline constexpr block nushu{0x1B170, 0x1B2FF};

/// Duployan
inline constexpr block duployan{0x1BC00, 0x1BC9F};

/// Shorthand Format Controls
inline constexpr block shorthand_format_controls{0x1BCA0, 0x1BCAF};

/// Znamenny Musical Notation
inline constexpr block znamenny_musical_notation{0x1CF00, 0x1CFCF};

/// Byzantine Musical Symbols
inline constexpr block byzantine_musical_symbols{0x1D000, 0x1D0FF};

/// Musical Symbols
inline constexpr block musical_symbols{0x1D100, 0x1D1FF};

/// Ancient Greek Musical Notation
inline constexpr block ancient_greek_musical_notation{0x1D200, 0x1D24F};

/// Mayan Numerals
inline constexpr block mayan_numerals{0x1D2E0, 0x1D2FF};

/// Tai Xuan Jing Symbols
inline constexpr block tai_xuan_jing_symbols{0x1D300, 0x1D35F};

/// Counting Rod Numerals
inline constexpr block counting_rod_numerals{0x1D360, 0x1D37F};

/// Mathematical Alphanumeric Symbols
inline constexpr block mathematical_alphanumeric_symbols{0x1D400, 0x1D7FF};

/// Sutton SignWriting
inline constexpr block sutton_signwriting{0x1D800, 0x1DAAF};

/// Latin Extended-G
inline constexpr block latin_extended_g{0x1DF00, 0x1DFFF};

/// Glagolitic Supplement
inline constexpr block glagolitic_supplement{0x1E000, 0x1E02F};

/// Nyiakeng Puachue Hmong
inline constexpr block nyiakeng_puachue_hmong{0x1E100, 0x1E14F};

/// Toto
inline constexpr block toto{0x1E290, 0x1E2BF};

/// Wancho
inline constexpr block wancho{0x1E2C0, 0x1E2FF};

/// Ethiopic Extended-B
inline constexpr block ethiopic_extended_b{0x1E7E0, 0x1E7FF};

/// Mende Kikakui
inline constexpr block mende_kikakui{0x1E800, 0x1E8DF};

/// Adlam
inline constexpr block adlam{0x1E900, 0x1E95F};

/// Indic Siyaq Numbers
inline constexpr block indic_siyaq_numbers{0x1EC70, 0x1ECBF};

/// Ottoman Siyaq Numbers
inline constexpr block ottoman_siyaq_numbers{0x1ED00, 0x1ED4F};

/// Arabic Mathematical Alphabetic Symbols
inline constexpr block arabic_mathematical_alphabetic_symbols{0x1EE00, 0x1EEFF};

/// Mahjong Tiles
inline constexpr block mahjong_tiles{0x1F000, 0x1F02F};

/// Domino Tiles
inline constexpr block domino_tiles{0x1F030, 0x1F09F};

/// Playing Cards
inline constexpr block playing_cards{0x1F0A0, 0x1F0FF};

/// Enclosed Alphanumeric Supplement
inline constexpr block enclosed_alphanumeric_supplement{0x1F100, 0x1F1FF};

/// Enclosed Ideographic Supplement
inline constexpr block enclosed_ideographic_supplement{0x1F200, 0x1F2FF};

/// Miscellaneous Symbols and Pictographs
inline constexpr block miscellaneous_symbols_and_pictographs{0x1F300, 0x1F5FF};

/// Emoticons
inline constexpr block emoticons{0x1F600, 0x1F64F};

/// Ornamental Dingbats
inline constexpr block ornamental_dingbats{0x1F650, 0x1F67F};

/// Transport and Map Symbols
inline constexpr block transport_and_map_symbols{0x1F680, 0x1F6FF};

/// Alchemical Symbols
inline constexpr block alchemical_symbols{0x1F700, 0x1F77F};

/// Geometric Shapes Extended
inline constexpr block geometric_shapes_extended{0x1F780, 0x1F7FF};

/// Supplemental Arrows-C
inline constexpr block supplemental_arrows_c{0x1F800, 0x1F8FF};

/// Supplemental Symbols and Pictographs
inline constexpr block supplemental_symbols_and_pictographs{0x1F900, 0x1F9FF};

/// Chess Symbols
inline constexpr block chess_symbols{0x1FA00, 0x1FA6F};

/// Symbols and Pictographs Extended-A
inline constexpr block symbols_and_pictographs_extended_a{0x1FA70, 0x1FAFF};

/// Symbols for Legacy Computing
inline constexpr block symbols_for_legacy_computing{0x1FB00, 0x1FBFF};

/// CJK Unified Ideographs Extension B
inline constexpr block cjk_unified_ideographs_extension_b{0x20000, 0x2A6DF};

/// CJK Unified Ideographs Extension C
inline constexpr block cjk_unified_ideographs_extension_c{0x2A700, 0x2B73F};

/// CJK Unified Ideographs Extension D
inline constexpr block cjk_unified_ideographs_extension_d{0x2B740, 0x2B81F};

/// CJK Unified Ideographs Extension E
inline constexpr block cjk_unified_ideographs_extension_e{0x2B820, 0x2CEAF};

/// CJK Unified Ideographs Extension F
inline constexpr block cjk_unified_ideographs_extension_f{0x2CEB0, 0x2EBEF};

/// CJK Compatibility Ideographs Supplement
inline constexpr block cjk_compatibility_ideographs_supplement{0x2F800, 0x2FA1F};

/// CJK Unified Ideographs Extension G
inline constexpr block cjk_unified_ideographs_extension_g{0x30000, 0x3134F};

/// Tags
inline constexpr block tags{0xE0000, 0xE007F};

/// Variation Selectors Supplement
inline constexpr block variation_selectors_supplement{0xE0100, 0xE01EF};

/// Supplementary Private Use Area-A
inline constexpr block supplementary_private_use_area_a{0xF0000, 0xFFFFF};

/// Supplementary Private Use Area-B
inline constexpr block supplementary_private_use_area_b{0x100000, 0x10FFFF};

} // namespace unicode
} // namespace type

#endif // ANTKEEPER_TYPE_UNICODE_BLOCK_HPP

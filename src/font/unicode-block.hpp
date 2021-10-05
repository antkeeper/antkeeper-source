/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_FONT_UNICODE_BLOCK_HPP
#define ANTKEEPER_FONT_UNICODE_BLOCK_HPP

namespace font {

/// Unicode-related functions and data
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
	
	/// Basic Latin[g]
	static const block basic_latin;

	/// Latin-1 Supplement[h]
	static const block latin_1_supplement;

	/// Latin Extended-A
	static const block latin_extended_a;

	/// Latin Extended-B
	static const block latin_extended_b;

	/// IPA Extensions
	static const block ipa_extensions;

	/// Spacing Modifier Letters
	static const block spacing_modifier_letters;

	/// Combining Diacritical Marks
	static const block combining_diacritical_marks;

	/// Greek and Coptic
	static const block greek_and_coptic;

	/// Cyrillic
	static const block cyrillic;

	/// Cyrillic Supplement
	static const block cyrillic_supplement;

	/// Armenian
	static const block armenian;

	/// Hebrew
	static const block hebrew;

	/// Arabic
	static const block arabic;

	/// Syriac
	static const block syriac;

	/// Arabic Supplement
	static const block arabic_supplement;

	/// Thaana
	static const block thaana;

	/// NKo
	static const block nko;

	/// Samaritan
	static const block samaritan;

	/// Mandaic
	static const block mandaic;

	/// Syriac Supplement
	static const block syriac_supplement;

	/// Arabic Extended-B
	static const block arabic_extended_b;

	/// Arabic Extended-A
	static const block arabic_extended_a;

	/// Devanagari
	static const block devanagari;

	/// Bengali
	static const block bengali;

	/// Gurmukhi
	static const block gurmukhi;

	/// Gujarati
	static const block gujarati;

	/// Oriya
	static const block oriya;

	/// Tamil
	static const block tamil;

	/// Telugu
	static const block telugu;

	/// Kannada
	static const block kannada;

	/// Malayalam
	static const block malayalam;

	/// Sinhala
	static const block sinhala;

	/// Thai
	static const block thai;

	/// Lao
	static const block lao;

	/// Tibetan
	static const block tibetan;

	/// Myanmar
	static const block myanmar;

	/// Georgian
	static const block georgian;

	/// Hangul Jamo
	static const block hangul_jamo;

	/// Ethiopic
	static const block ethiopic;

	/// Ethiopic Supplement
	static const block ethiopic_supplement;

	/// Cherokee
	static const block cherokee;

	/// Unified Canadian Aboriginal Syllabics
	static const block unified_canadian_aboriginal_syllabics;

	/// Ogham
	static const block ogham;

	/// Runic
	static const block runic;

	/// Tagalog
	static const block tagalog;

	/// Hanunoo
	static const block hanunoo;

	/// Buhid
	static const block buhid;

	/// Tagbanwa
	static const block tagbanwa;

	/// Khmer
	static const block khmer;

	/// Mongolian
	static const block mongolian;

	/// Unified Canadian Aboriginal Syllabics Extended
	static const block unified_canadian_aboriginal_syllabics_extended;

	/// Limbu
	static const block limbu;

	/// Tai Le
	static const block tai_le;

	/// New Tai Lue
	static const block new_tai_lue;

	/// Khmer Symbols
	static const block khmer_symbols;

	/// Buginese
	static const block buginese;

	/// Tai Tham
	static const block tai_tham;

	/// Combining Diacritical Marks Extended
	static const block combining_diacritical_marks_extended;

	/// Balinese
	static const block balinese;

	/// Sundanese
	static const block sundanese;

	/// Batak
	static const block batak;

	/// Lepcha
	static const block lepcha;

	/// Ol Chiki
	static const block ol_chiki;

	/// Cyrillic Extended-C
	static const block cyrillic_extended_c;

	/// Georgian Extended
	static const block georgian_extended;

	/// Sundanese Supplement
	static const block sundanese_supplement;

	/// Vedic Extensions
	static const block vedic_extensions;

	/// Phonetic Extensions
	static const block phonetic_extensions;

	/// Phonetic Extensions Supplement
	static const block phonetic_extensions_supplement;

	/// Combining Diacritical Marks Supplement
	static const block combining_diacritical_marks_supplement;

	/// Latin Extended Additional
	static const block latin_extended_additional;

	/// Greek Extended
	static const block greek_extended;

	/// General Punctuation
	static const block general_punctuation;

	/// Superscripts and Subscripts
	static const block superscripts_and_subscripts;

	/// Currency Symbols
	static const block currency_symbols;

	/// Combining Diacritical Marks for Symbols
	static const block combining_diacritical_marks_for_symbols;

	/// Letterlike Symbols
	static const block letterlike_symbols;

	/// Number Forms
	static const block number_forms;

	/// Arrows
	static const block arrows;

	/// Mathematical Operators
	static const block mathematical_operators;

	/// Miscellaneous Technical
	static const block miscellaneous_technical;

	/// Control Pictures
	static const block control_pictures;

	/// Optical Character Recognition
	static const block optical_character_recognition;

	/// Enclosed Alphanumerics
	static const block enclosed_alphanumerics;

	/// Box Drawing
	static const block box_drawing;

	/// Block Elements
	static const block block_elements;

	/// Geometric Shapes
	static const block geometric_shapes;

	/// Miscellaneous Symbols
	static const block miscellaneous_symbols;

	/// Dingbats
	static const block dingbats;

	/// Miscellaneous Mathematical Symbols-A
	static const block miscellaneous_mathematical_symbols_a;

	/// Supplemental Arrows-A
	static const block supplemental_arrows_a;

	/// Braille Patterns
	static const block braille_patterns;

	/// Supplemental Arrows-B
	static const block supplemental_arrows_b;

	/// Miscellaneous Mathematical Symbols-B
	static const block miscellaneous_mathematical_symbols_b;

	/// Supplemental Mathematical Operators
	static const block supplemental_mathematical_operators;

	/// Miscellaneous Symbols and Arrows
	static const block miscellaneous_symbols_and_arrows;

	/// Glagolitic
	static const block glagolitic;

	/// Latin Extended-C
	static const block latin_extended_c;

	/// Coptic
	static const block coptic;

	/// Georgian Supplement
	static const block georgian_supplement;

	/// Tifinagh
	static const block tifinagh;

	/// Ethiopic Extended
	static const block ethiopic_extended;

	/// Cyrillic Extended-A
	static const block cyrillic_extended_a;

	/// Supplemental Punctuation
	static const block supplemental_punctuation;

	/// CJK Radicals Supplement
	static const block cjk_radicals_supplement;

	/// Kangxi Radicals
	static const block kangxi_radicals;

	/// Ideographic Description Characters
	static const block ideographic_description_characters;

	/// CJK Symbols and Punctuation
	static const block cjk_symbols_and_punctuation;

	/// Hiragana
	static const block hiragana;

	/// Katakana
	static const block katakana;

	/// Bopomofo
	static const block bopomofo;

	/// Hangul Compatibility Jamo
	static const block hangul_compatibility_jamo;

	/// Kanbun
	static const block kanbun;

	/// Bopomofo Extended
	static const block bopomofo_extended;

	/// CJK Strokes
	static const block cjk_strokes;

	/// Katakana Phonetic Extensions
	static const block katakana_phonetic_extensions;

	/// Enclosed CJK Letters and Months
	static const block enclosed_cjk_letters_and_months;

	/// CJK Compatibility
	static const block cjk_compatibility;

	/// CJK Unified Ideographs Extension A
	static const block cjk_unified_ideographs_extension_a;

	/// Yijing Hexagram Symbols
	static const block yijing_hexagram_symbols;

	/// CJK Unified Ideographs
	static const block cjk_unified_ideographs;

	/// Yi Syllables
	static const block yi_syllables;

	/// Yi Radicals
	static const block yi_radicals;

	/// Lisu
	static const block lisu;

	/// Vai
	static const block vai;

	/// Cyrillic Extended-B
	static const block cyrillic_extended_b;

	/// Bamum
	static const block bamum;

	/// Modifier Tone Letters
	static const block modifier_tone_letters;

	/// Latin Extended-D
	static const block latin_extended_d;

	/// Syloti Nagri
	static const block syloti_nagri;

	/// Common Indic Number Forms
	static const block common_indic_number_forms;

	/// Phags-pa
	static const block phags_pa;

	/// Saurashtra
	static const block saurashtra;

	/// Devanagari Extended
	static const block devanagari_extended;

	/// Kayah Li
	static const block kayah_li;

	/// Rejang
	static const block rejang;

	/// Hangul Jamo Extended-A
	static const block hangul_jamo_extended_a;

	/// Javanese
	static const block javanese;

	/// Myanmar Extended-B
	static const block myanmar_extended_b;

	/// Cham
	static const block cham;

	/// Myanmar Extended-A
	static const block myanmar_extended_a;

	/// Tai Viet
	static const block tai_viet;

	/// Meetei Mayek Extensions
	static const block meetei_mayek_extensions;

	/// Ethiopic Extended-A
	static const block ethiopic_extended_a;

	/// Latin Extended-E
	static const block latin_extended_e;

	/// Cherokee Supplement
	static const block cherokee_supplement;

	/// Meetei Mayek
	static const block meetei_mayek;

	/// Hangul Syllables
	static const block hangul_syllables;

	/// Hangul Jamo Extended-B
	static const block hangul_jamo_extended_b;

	/// High Surrogates
	static const block high_surrogates;

	/// High Private Use Surrogates
	static const block high_private_use_surrogates;

	/// Low Surrogates
	static const block low_surrogates;

	/// Private Use Area
	static const block private_use_area;

	/// CJK Compatibility Ideographs
	static const block cjk_compatibility_ideographs;

	/// Alphabetic Presentation Forms
	static const block alphabetic_presentation_forms;

	/// Arabic Presentation Forms-A
	static const block arabic_presentation_forms_a;

	/// Variation Selectors
	static const block variation_selectors;

	/// Vertical Forms
	static const block vertical_forms;

	/// Combining Half Marks
	static const block combining_half_marks;

	/// CJK Compatibility Forms
	static const block cjk_compatibility_forms;

	/// Small Form Variants
	static const block small_form_variants;

	/// Arabic Presentation Forms-B
	static const block arabic_presentation_forms_b;

	/// Halfwidth and Fullwidth Forms
	static const block halfwidth_and_fullwidth_forms;

	/// Specials
	static const block specials;

	/// Linear B Syllabary
	static const block linear_b_syllabary;

	/// Linear B Ideograms
	static const block linear_b_ideograms;

	/// Aegean Numbers
	static const block aegean_numbers;

	/// Ancient Greek Numbers
	static const block ancient_greek_numbers;

	/// Ancient Symbols
	static const block ancient_symbols;

	/// Phaistos Disc
	static const block phaistos_disc;

	/// Lycian
	static const block lycian;

	/// Carian
	static const block carian;

	/// Coptic Epact Numbers
	static const block coptic_epact_numbers;

	/// Old Italic
	static const block old_italic;

	/// Gothic
	static const block gothic;

	/// Old Permic
	static const block old_permic;

	/// Ugaritic
	static const block ugaritic;

	/// Old Persian
	static const block old_persian;

	/// Deseret
	static const block deseret;

	/// Shavian
	static const block shavian;

	/// Osmanya
	static const block osmanya;

	/// Osage
	static const block osage;

	/// Elbasan
	static const block elbasan;

	/// Caucasian Albanian
	static const block caucasian_albanian;

	/// Vithkuqi
	static const block vithkuqi;

	/// Linear A
	static const block linear_a;

	/// Latin Extended-F
	static const block latin_extended_f;

	/// Cypriot Syllabary
	static const block cypriot_syllabary;

	/// Imperial Aramaic
	static const block imperial_aramaic;

	/// Palmyrene
	static const block palmyrene;

	/// Nabataean
	static const block nabataean;

	/// Hatran
	static const block hatran;

	/// Phoenician
	static const block phoenician;

	/// Lydian
	static const block lydian;

	/// Meroitic Hieroglyphs
	static const block meroitic_hieroglyphs;

	/// Meroitic Cursive
	static const block meroitic_cursive;

	/// Kharoshthi
	static const block kharoshthi;

	/// Old South Arabian
	static const block old_south_arabian;

	/// Old North Arabian
	static const block old_north_arabian;

	/// Manichaean
	static const block manichaean;

	/// Avestan
	static const block avestan;

	/// Inscriptional Parthian
	static const block inscriptional_parthian;

	/// Inscriptional Pahlavi
	static const block inscriptional_pahlavi;

	/// Psalter Pahlavi
	static const block psalter_pahlavi;

	/// Old Turkic
	static const block old_turkic;

	/// Old Hungarian
	static const block old_hungarian;

	/// Hanifi Rohingya
	static const block hanifi_rohingya;

	/// Rumi Numeral Symbols
	static const block rumi_numeral_symbols;

	/// Yezidi
	static const block yezidi;

	/// Old Sogdian
	static const block old_sogdian;

	/// Sogdian
	static const block sogdian;

	/// Old Uyghur
	static const block old_uyghur;

	/// Chorasmian
	static const block chorasmian;

	/// Elymaic
	static const block elymaic;

	/// Brahmi
	static const block brahmi;

	/// Kaithi
	static const block kaithi;

	/// Sora Sompeng
	static const block sora_sompeng;

	/// Chakma
	static const block chakma;

	/// Mahajani
	static const block mahajani;

	/// Sharada
	static const block sharada;

	/// Sinhala Archaic Numbers
	static const block sinhala_archaic_numbers;

	/// Khojki
	static const block khojki;

	/// Multani
	static const block multani;

	/// Khudawadi
	static const block khudawadi;

	/// Grantha
	static const block grantha;

	/// Newa
	static const block newa;

	/// Tirhuta
	static const block tirhuta;

	/// Siddham
	static const block siddham;

	/// Modi
	static const block modi;

	/// Mongolian Supplement
	static const block mongolian_supplement;

	/// Takri
	static const block takri;

	/// Ahom
	static const block ahom;

	/// Dogra
	static const block dogra;

	/// Warang Citi
	static const block warang_citi;

	/// Dives Akuru
	static const block dives_akuru;

	/// Nandinagari
	static const block nandinagari;

	/// Zanabazar Square
	static const block zanabazar_square;

	/// Soyombo
	static const block soyombo;

	/// Unified Canadian Aboriginal Syllabics Extended-A
	static const block unified_canadian_aboriginal_syllabics_extended_a;

	/// Pau Cin Hau
	static const block pau_cin_hau;

	/// Bhaiksuki
	static const block bhaiksuki;

	/// Marchen
	static const block marchen;

	/// Masaram Gondi
	static const block masaram_gondi;

	/// Gunjala Gondi
	static const block gunjala_gondi;

	/// Makasar
	static const block makasar;

	/// Lisu Supplement
	static const block lisu_supplement;

	/// Tamil Supplement
	static const block tamil_supplement;

	/// Cuneiform
	static const block cuneiform;

	/// Cuneiform Numbers and Punctuation
	static const block cuneiform_numbers_and_punctuation;

	/// Early Dynastic Cuneiform
	static const block early_dynastic_cuneiform;

	/// Cypro-Minoan
	static const block cypro_minoan;

	/// Egyptian Hieroglyphs
	static const block egyptian_hieroglyphs;

	/// Egyptian Hieroglyph Format Controls
	static const block egyptian_hieroglyph_format_controls;

	/// Anatolian Hieroglyphs
	static const block anatolian_hieroglyphs;

	/// Bamum Supplement
	static const block bamum_supplement;

	/// Mro
	static const block mro;

	/// Tangsa
	static const block tangsa;

	/// Bassa Vah
	static const block bassa_vah;

	/// Pahawh Hmong
	static const block pahawh_hmong;

	/// Medefaidrin
	static const block medefaidrin;

	/// Miao
	static const block miao;

	/// Ideographic Symbols and Punctuation
	static const block ideographic_symbols_and_punctuation;

	/// Tangut
	static const block tangut;

	/// Tangut Components
	static const block tangut_components;

	/// Khitan Small Script
	static const block khitan_small_script;

	/// Tangut Supplement
	static const block tangut_supplement;

	/// Kana Extended-B
	static const block kana_extended_b;

	/// Kana Supplement
	static const block kana_supplement;

	/// Kana Extended-A
	static const block kana_extended_a;

	/// Small Kana Extension
	static const block small_kana_extension;

	/// Nushu
	static const block nushu;

	/// Duployan
	static const block duployan;

	/// Shorthand Format Controls
	static const block shorthand_format_controls;

	/// Znamenny Musical Notation
	static const block znamenny_musical_notation;

	/// Byzantine Musical Symbols
	static const block byzantine_musical_symbols;

	/// Musical Symbols
	static const block musical_symbols;

	/// Ancient Greek Musical Notation
	static const block ancient_greek_musical_notation;

	/// Mayan Numerals
	static const block mayan_numerals;

	/// Tai Xuan Jing Symbols
	static const block tai_xuan_jing_symbols;

	/// Counting Rod Numerals
	static const block counting_rod_numerals;

	/// Mathematical Alphanumeric Symbols
	static const block mathematical_alphanumeric_symbols;

	/// Sutton SignWriting
	static const block sutton_signwriting;

	/// Latin Extended-G
	static const block latin_extended_g;

	/// Glagolitic Supplement
	static const block glagolitic_supplement;

	/// Nyiakeng Puachue Hmong
	static const block nyiakeng_puachue_hmong;

	/// Toto
	static const block toto;

	/// Wancho
	static const block wancho;

	/// Ethiopic Extended-B
	static const block ethiopic_extended_b;

	/// Mende Kikakui
	static const block mende_kikakui;

	/// Adlam
	static const block adlam;

	/// Indic Siyaq Numbers
	static const block indic_siyaq_numbers;

	/// Ottoman Siyaq Numbers
	static const block ottoman_siyaq_numbers;

	/// Arabic Mathematical Alphabetic Symbols
	static const block arabic_mathematical_alphabetic_symbols;

	/// Mahjong Tiles
	static const block mahjong_tiles;

	/// Domino Tiles
	static const block domino_tiles;

	/// Playing Cards
	static const block playing_cards;

	/// Enclosed Alphanumeric Supplement
	static const block enclosed_alphanumeric_supplement;

	/// Enclosed Ideographic Supplement
	static const block enclosed_ideographic_supplement;

	/// Miscellaneous Symbols and Pictographs
	static const block miscellaneous_symbols_and_pictographs;

	/// Emoticons
	static const block emoticons;

	/// Ornamental Dingbats
	static const block ornamental_dingbats;

	/// Transport and Map Symbols
	static const block transport_and_map_symbols;

	/// Alchemical Symbols
	static const block alchemical_symbols;

	/// Geometric Shapes Extended
	static const block geometric_shapes_extended;

	/// Supplemental Arrows-C
	static const block supplemental_arrows_c;

	/// Supplemental Symbols and Pictographs
	static const block supplemental_symbols_and_pictographs;

	/// Chess Symbols
	static const block chess_symbols;

	/// Symbols and Pictographs Extended-A
	static const block symbols_and_pictographs_extended_a;

	/// Symbols for Legacy Computing
	static const block symbols_for_legacy_computing;

	/// CJK Unified Ideographs Extension B
	static const block cjk_unified_ideographs_extension_b;

	/// CJK Unified Ideographs Extension C
	static const block cjk_unified_ideographs_extension_c;

	/// CJK Unified Ideographs Extension D
	static const block cjk_unified_ideographs_extension_d;

	/// CJK Unified Ideographs Extension E
	static const block cjk_unified_ideographs_extension_e;

	/// CJK Unified Ideographs Extension F
	static const block cjk_unified_ideographs_extension_f;

	/// CJK Compatibility Ideographs Supplement
	static const block cjk_compatibility_ideographs_supplement;

	/// CJK Unified Ideographs Extension G
	static const block cjk_unified_ideographs_extension_g;

	/// Tags
	static const block tags;

	/// Variation Selectors Supplement
	static const block variation_selectors_supplement;

	/// Supplementary Private Use Area-A
	static const block supplementary_private_use_area_a;

	/// Supplementary Private Use Area-B
	static const block supplementary_private_use_area_b;
};

} // namespace unicode
} // namespace font

#endif // ANTKEEPER_FONT_UNICODE_BLOCK_HPP

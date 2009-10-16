
#include "sinfont.h"



void languages::enableAll() { basic_latin = true;
                              latin_supplement = true;
                              latin_extended_a = true;
                              latin_extended_b = true;
                              ipa_extensions = true;
                              spacing_modifier_letters = true;
                              combining_diacritical_marks = true;
                              greek_coptic = true;
                              cyrillic = true;
                              cyrillic_supplement = true; }

void languages::enableLatin() { latin_supplement = true;
                                latin_extended_a = true;
                                latin_extended_b = true;
                                ipa_extensions = true;
                                spacing_modifier_letters = true;
                                combining_diacritical_marks = true; }

void languages::enableGreek() { greek_coptic = true; }

void languages::enableCyrillic() { cyrillic = true;
                                   cyrillic_supplement = true; }

void languages::enableExtendedAscii() { latin_supplement = true; }


languages::languagess() :  basic_latin(true),
                           latin_supplement(false),
                           latin_extended_a(false),
                           latin_extended_b(false),
                           ipa_extensions(false),
                           spacing_modifier_letters(false),
                           combining_diacritical_marks(false),
                           greek_coptic(false),
                           cyrillic(false),
                           cyrillic_supplement(false) {}

void languages::populate_list() {
    int i;

    valid_chars.clear();

    if ( basic_latin )
        for ( i=0x0000; i <= 0x007E; i++ ) valid_chars.push_back(i);
    if ( latin_supplement )
        for ( i=0x00A0; i <= 0x00FF; i++ ) valid_chars.push_back(i);
    if ( latin_extended_a )
        for ( i=0x0100; i <= 0x017F; i++ ) valid_chars.push_back(i);
    if ( latin_extended_b )
        for ( i=0x0180; i <= 0x024F; i++ ) valid_chars.push_back(i);
    if ( ipa_extensions )
        for ( i=0x0250; i <= 0x02AF; i++ ) valid_chars.push_back(i);
    if ( spacing_modifier_letters )
        for ( i=0x02B0; i <= 0x02FF; i++ ) valid_chars.push_back(i);
    if ( combining_diacritical_marks )
        for ( i=0x0300; i <= 0x036F; i++ ) valid_chars.push_back(i);
    if ( greek_coptic )
        for ( i=0x0370; i <= 0x03FF; i++ ) valid_chars.push_back(i);
    if ( cyrillic )
        for ( i=0x0400; i <= 0x04FF; i++ ) valid_chars.push_back(i);
    if ( cyrillic_supplement )
        for ( i=0x0500; i <= 0x052F; i++ ) valid_chars.push_back(i);
}

/*
 * Copyright (c) 2020-2023 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef COMMON_FIXED_8x16_SPRITE_FONT_ACCENT_H
#define COMMON_FIXED_8x16_SPRITE_FONT_ACCENT_H

#include "bn_sprite_font.h"
#include "bn_sprite_items_fixed_8x16_font_accent.h"
#include "bn_utf8_characters_map.h"

constexpr bn::utf8_character fixed_8x16_sprite_font_accent_utf8_characters[] = {
    "Á", "É", "Í", "Ó", "Ú", "Ü", "Ñ", "á",
    "é", "í", "ó", "ú", "ü", "ñ", "¡", "¿"};

constexpr bn::span<const bn::utf8_character>
    fixed_8x16_sprite_font_accent_utf8_characters_span(
        fixed_8x16_sprite_font_accent_utf8_characters);

constexpr auto fixed_8x16_sprite_font_accent_utf8_characters_map =
    bn::utf8_characters_map<
        fixed_8x16_sprite_font_accent_utf8_characters_span>();

constexpr bn::sprite_font fixed_8x16_sprite_font_accent(
    bn::sprite_items::fixed_8x16_font_accent,
    fixed_8x16_sprite_font_accent_utf8_characters_map.reference());

#endif
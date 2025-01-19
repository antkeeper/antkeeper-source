// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_GL_FORMAT_LUT_HPP
#define ANTKEEPER_GL_GL_FORMAT_LUT_HPP

#include <glad/gl.h>

namespace gl {

/// Maps gl::format to OpenGL internal format, base format, and pixel type.
constexpr GLenum gl_format_lut[][3] =
{
	{0                                           , 0                 , 0                                }, // undefined
	{0                                           , GL_RG             , 0                                }, // r4g4_unorm_pack8
	{GL_RGBA4                                    , GL_RGBA           , GL_UNSIGNED_SHORT_4_4_4_4        }, // r4g4b4a4_unorm_pack16
	{GL_RGBA4                                    , GL_BGRA           , GL_UNSIGNED_SHORT_4_4_4_4        }, // b4g4r4a4_unorm_pack16
	{GL_RGB565                                   , GL_RGB            , GL_UNSIGNED_SHORT_5_6_5          }, // r5g6b5_unorm_pack16
	{GL_RGB565                                   , GL_BGR            , GL_UNSIGNED_SHORT_5_6_5          }, // b5g6r5_unorm_pack16
	{GL_RGB5_A1                                  , GL_RGBA           , GL_UNSIGNED_SHORT_5_5_5_1        }, // r5g5b5a1_unorm_pack16
	{GL_RGB5_A1                                  , GL_BGRA           , GL_UNSIGNED_SHORT_5_5_5_1        }, // b5g5r5a1_unorm_pack16
	{GL_RGB5_A1                                  , GL_BGRA           , GL_UNSIGNED_SHORT_1_5_5_5_REV    }, // a1r5g5b5_unorm_pack16
	{GL_R8                                       , GL_RED            , GL_UNSIGNED_BYTE                 }, // r8_unorm
	{GL_R8_SNORM                                 , GL_RED            , GL_BYTE                          }, // r8_snorm
	{0                                           , GL_RED            , GL_UNSIGNED_BYTE                 }, // r8_uscaled
	{0                                           , GL_RED            , GL_BYTE                          }, // r8_sscaled
	{GL_R8UI                                     , GL_RED_INTEGER    , GL_UNSIGNED_BYTE                 }, // r8_uint
	{GL_R8I                                      , GL_RED_INTEGER    , GL_BYTE                          }, // r8_sint
	{0                                           , GL_RED            , GL_UNSIGNED_BYTE                 }, // r8_srgb
	{GL_RG8                                      , GL_RG             , GL_UNSIGNED_BYTE                 }, // r8g8_unorm
	{GL_RG8_SNORM                                , GL_RG             , GL_BYTE                          }, // r8g8_snorm
	{0                                           , GL_RED            , GL_UNSIGNED_BYTE                 }, // r8g8_uscaled
	{0                                           , GL_RED            , GL_BYTE                          }, // r8g8_sscaled
	{GL_RG8UI                                    , GL_RG_INTEGER     , GL_UNSIGNED_BYTE                 }, // r8g8_uint
	{GL_RG8I                                     , GL_RG_INTEGER     , GL_BYTE                          }, // r8g8_sint
	{0                                           , GL_RG             , GL_UNSIGNED_BYTE                 }, // r8g8_srgb
	{GL_RGB8                                     , GL_RGB            , GL_UNSIGNED_BYTE                 }, // r8g8b8_unorm
	{GL_RGB8_SNORM                               , GL_RGB            , GL_BYTE                          }, // r8g8b8_snorm
	{0                                           , GL_RED            , GL_UNSIGNED_BYTE                 }, // r8g8b8_uscaled
	{0                                           , GL_RED            , GL_BYTE                          }, // r8g8b8_sscaled
	{GL_RGB8UI                                   , GL_RGB_INTEGER    , GL_UNSIGNED_BYTE                 }, // r8g8b8_uint
	{GL_RGB8I                                    , GL_RGB_INTEGER    , GL_BYTE                          }, // r8g8b8_sint
	{GL_SRGB8                                    , GL_RGB            , GL_UNSIGNED_BYTE                 }, // r8g8b8_srgb
	{GL_RGB8                                     , GL_BGR            , GL_UNSIGNED_BYTE                 }, // b8g8r8_unorm
	{GL_RGB8_SNORM                               , GL_BGR            , GL_BYTE                          }, // b8g8r8_snorm
	{0                                           , GL_BGR            , GL_UNSIGNED_BYTE                 }, // b8g8r8_uscaled
	{0                                           , GL_BGR            , GL_BYTE                          }, // b8g8r8_sscaled
	{GL_RGB8UI                                   , GL_BGR_INTEGER    , GL_UNSIGNED_BYTE                 }, // b8g8r8_uint
	{GL_RGB8I                                    , GL_BGR_INTEGER    , GL_BYTE                          }, // b8g8r8_sint
	{GL_SRGB8                                    , GL_BGR            , GL_UNSIGNED_BYTE                 }, // b8g8r8_srgb
	{GL_RGBA8                                    , GL_RGBA           , GL_UNSIGNED_BYTE                 }, // r8g8b8a8_unorm
	{GL_RGBA8_SNORM                              , GL_RGBA           , GL_BYTE                          }, // r8g8b8a8_snorm
	{0                                           , GL_RGBA           , GL_UNSIGNED_BYTE                 }, // r8g8b8a8_uscaled
	{0                                           , GL_RGBA           , GL_BYTE                          }, // r8g8b8a8_sscaled
	{GL_RGBA8UI                                  , GL_RGBA_INTEGER   , GL_UNSIGNED_BYTE                 }, // r8g8b8a8_uint
	{GL_RGBA8I                                   , GL_RGBA_INTEGER   , GL_BYTE                          }, // r8g8b8a8_sint
	{GL_SRGB8_ALPHA8                             , GL_RGBA           , GL_UNSIGNED_BYTE                 }, // r8g8b8a8_srgb
	{GL_RGBA8                                    , GL_BGRA           , GL_UNSIGNED_BYTE                 }, // b8g8r8a8_unorm
	{GL_RGBA8_SNORM                              , GL_BGRA           , GL_BYTE                          }, // b8g8r8a8_snorm
	{0                                           , GL_BGRA           , GL_UNSIGNED_BYTE                 }, // b8g8r8a8_uscaled
	{0                                           , GL_BGRA           , GL_BYTE                          }, // b8g8r8a8_sscaled
	{GL_RGBA8UI                                  , GL_BGRA_INTEGER   , GL_UNSIGNED_BYTE                 }, // b8g8r8a8_uint
	{GL_RGBA8I                                   , GL_BGRA_INTEGER   , GL_BYTE                          }, // b8g8r8a8_sint
	{GL_SRGB8_ALPHA8                             , GL_BGRA           , GL_UNSIGNED_BYTE                 }, // b8g8r8a8_srgb
	{GL_RGBA8                                    , GL_RGBA           , GL_UNSIGNED_INT_8_8_8_8_REV      }, // a8b8g8r8_unorm_pack32
	{GL_RGBA8_SNORM                              , GL_RGBA           , 0                                }, // a8b8g8r8_snorm_pack32
	{0                                           , GL_RGBA           , GL_UNSIGNED_INT_8_8_8_8_REV      }, // a8b8g8r8_uscaled_pack32
	{0                                           , GL_RGBA           , 0                                }, // a8b8g8r8_sscaled_pack32
	{GL_RGBA8UI                                  , GL_RGBA_INTEGER   , GL_UNSIGNED_INT_8_8_8_8_REV      }, // a8b8g8r8_uint_pack32
	{GL_RGBA8I                                   , GL_RGBA_INTEGER   , 0                                }, // a8b8g8r8_sint_pack32
	{GL_SRGB8_ALPHA8                             , GL_RGBA           , GL_UNSIGNED_INT_8_8_8_8_REV      }, // a8b8g8r8_srgb_pack32
	{GL_RGB10_A2                                 , GL_BGRA           , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2r10g10b10_unorm_pack32
	{0                                           , GL_BGRA           , 0                                }, // a2r10g10b10_snorm_pack32
	{0                                           , GL_BGRA           , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2r10g10b10_uscaled_pack32
	{0                                           , GL_BGRA           , 0                                }, // a2r10g10b10_sscaled_pack32
	{GL_RGB10_A2UI                               , GL_BGRA_INTEGER   , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2r10g10b10_uint_pack32
	{0                                           , GL_BGRA_INTEGER   , 0                                }, // a2r10g10b10_sint_pack32
	{GL_RGB10_A2                                 , GL_RGBA           , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2b10g10r10_unorm_pack32
	{0                                           , GL_RGBA           , 0                                }, // a2b10g10r10_snorm_pack32
	{0                                           , GL_RGBA           , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2b10g10r10_uscaled_pack32
	{0                                           , GL_RGBA           , 0                                }, // a2b10g10r10_sscaled_pack32
	{GL_RGB10_A2UI                               , GL_RGBA_INTEGER   , GL_UNSIGNED_INT_2_10_10_10_REV   }, // a2b10g10r10_uint_pack32
	{0                                           , GL_RGBA_INTEGER   , 0                                }, // a2b10g10r10_sint_pack32
	{GL_R16                                      , GL_RED            , GL_UNSIGNED_SHORT                }, // r16_unorm
	{GL_R16_SNORM                                , GL_RED            , GL_SHORT                         }, // r16_snorm
	{0                                           , GL_RED            , GL_UNSIGNED_SHORT                }, // r16_uscaled
	{0                                           , GL_RED            , GL_SHORT                         }, // r16_sscaled
	{GL_R16UI                                    , GL_RED_INTEGER    , GL_UNSIGNED_SHORT                }, // r16_uint
	{GL_R16I                                     , GL_RED_INTEGER    , GL_SHORT                         }, // r16_sint
	{GL_R16F                                     , GL_RED            , GL_HALF_FLOAT                    }, // r16_sfloat
	{GL_RG16                                     , GL_RG             , GL_UNSIGNED_SHORT                }, // r16g16_unorm
	{GL_RG16_SNORM                               , GL_RG             , GL_SHORT                         }, // r16g16_snorm
	{0                                           , GL_RG             , GL_UNSIGNED_SHORT                }, // r16g16_uscaled
	{0                                           , GL_RG             , GL_SHORT                         }, // r16g16_sscaled
	{GL_RG16UI                                   , GL_RG_INTEGER     , GL_UNSIGNED_SHORT                }, // r16g16_uint
	{GL_RG16I                                    , GL_RG_INTEGER     , GL_SHORT                         }, // r16g16_sint
	{GL_RG16F                                    , GL_RG             , GL_HALF_FLOAT                    }, // r16g16_sfloat
	{GL_RGB16                                    , GL_RGB            , GL_UNSIGNED_SHORT                }, // r16g16b16_unorm
	{GL_RGB16_SNORM                              , GL_RGB            , GL_SHORT                         }, // r16g16b16_snorm
	{0                                           , GL_RGB            , GL_UNSIGNED_SHORT                }, // r16g16b16_uscaled
	{0                                           , GL_RGB            , GL_SHORT                         }, // r16g16b16_sscaled
	{GL_RGB16UI                                  , GL_RGB_INTEGER    , GL_UNSIGNED_SHORT                }, // r16g16b16_uint
	{GL_RGB16I                                   , GL_RGB_INTEGER    , GL_SHORT                         }, // r16g16b16_sint
	{GL_RGB16F                                   , GL_RGB            , GL_HALF_FLOAT                    }, // r16g16b16_sfloat
	{GL_RGBA16                                   , GL_RGBA           , GL_UNSIGNED_SHORT                }, // r16g16b16a16_unorm
	{GL_RGBA16_SNORM                             , GL_RGBA           , GL_SHORT                         }, // r16g16b16a16_snorm
	{0                                           , GL_RGBA           , GL_UNSIGNED_SHORT                }, // r16g16b16a16_uscaled
	{0                                           , GL_RGBA           , GL_SHORT                         }, // r16g16b16a16_sscaled
	{GL_RGBA16UI                                 , GL_RGBA_INTEGER   , GL_UNSIGNED_SHORT                }, // r16g16b16a16_uint
	{GL_RGBA16I                                  , GL_RGBA_INTEGER   , GL_SHORT                         }, // r16g16b16a16_sint
	{GL_RGBA16F                                  , GL_RGBA           , GL_HALF_FLOAT                    }, // r16g16b16a16_sfloat
	{GL_R32UI                                    , GL_RED_INTEGER    , GL_UNSIGNED_INT                  }, // r32_uint
	{GL_R32I                                     , GL_RED_INTEGER    , GL_INT                           }, // r32_sint
	{GL_R32F                                     , GL_RED            , GL_FLOAT                         }, // r32_sfloat
	{GL_RG32UI                                   , GL_RG_INTEGER     , GL_UNSIGNED_INT                  }, // r32g32_uint
	{GL_RG32I                                    , GL_RG_INTEGER     , GL_INT                           }, // r32g32_sint
	{GL_RG32F                                    , GL_RG             , GL_FLOAT                         }, // r32g32_sfloat
	{GL_RGB32UI                                  , GL_RGB_INTEGER    , GL_UNSIGNED_INT                  }, // r32g32b32_uint
	{GL_RGB32I                                   , GL_RGB_INTEGER    , GL_INT                           }, // r32g32b32_sint
	{GL_RGB32F                                   , GL_RGB            , GL_FLOAT                         }, // r32g32b32_sfloat
	{GL_RGBA32UI                                 , GL_RGBA_INTEGER   , GL_UNSIGNED_INT                  }, // r32g32b32a32_uint
	{GL_RGBA32I                                  , GL_RGBA_INTEGER   , GL_INT                           }, // r32g32b32a32_sint
	{GL_RGBA32F                                  , GL_RGBA           , GL_FLOAT                         }, // r32g32b32a32_sfloat
	{0                                           , GL_RED_INTEGER    , 0                                }, // r64_uint
	{0                                           , GL_RED_INTEGER    , 0                                }, // r64_sint
	{0                                           , GL_RED            , GL_DOUBLE                        }, // r64_sfloat
	{0                                           , GL_RG_INTEGER     , 0                                }, // r64g64_uint
	{0                                           , GL_RG_INTEGER     , 0                                }, // r64g64_sint
	{0                                           , GL_RG             , GL_DOUBLE                        }, // r64g64_sfloat
	{0                                           , GL_RGB_INTEGER    , 0                                }, // r64g64b64_uint
	{0                                           , GL_RGB_INTEGER    , 0                                }, // r64g64b64_sint
	{0                                           , GL_RGB            , GL_DOUBLE                        }, // r64g64b64_sfloat
	{0                                           , GL_RGBA_INTEGER   , 0                                }, // r64g64b64a64_uint
	{0                                           , GL_RGBA_INTEGER   , 0                                }, // r64g64b64a64_sint
	{0                                           , GL_RGBA           , GL_DOUBLE                        }, // r64g64b64a64_sfloat
	{GL_R11F_G11F_B10F                           , GL_BGR            , GL_UNSIGNED_INT_10F_11F_11F_REV  }, // b10g11r11_ufloat_pack32
	{GL_RGB9_E5                                  , GL_BGR            , GL_UNSIGNED_INT_5_9_9_9_REV      }, // e5b9g9r9_ufloat_pack32
	{GL_DEPTH_COMPONENT16                        , GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT                }, // d16_unorm
	{GL_DEPTH_COMPONENT24                        , GL_DEPTH_COMPONENT, GL_UNSIGNED_INT                  }, // x8_d24_unorm_pack32
	{GL_DEPTH_COMPONENT32F                       , GL_DEPTH_COMPONENT, GL_FLOAT                         }, // d32_sfloat
	{GL_STENCIL_INDEX8                           , GL_STENCIL_INDEX  , GL_UNSIGNED_BYTE                 }, // s8_uint
	{0                                           , GL_DEPTH_STENCIL  , 0                                }, // d16_unorm_s8_uint
	{GL_DEPTH24_STENCIL8                         , GL_DEPTH_STENCIL  , GL_UNSIGNED_INT_24_8             }, // d24_unorm_s8_uint
	{GL_DEPTH32F_STENCIL8                        , GL_DEPTH_STENCIL  , GL_FLOAT_32_UNSIGNED_INT_24_8_REV}, // d32_sfloat_s8_uint
	{0                                           , GL_RGB            , 0                                }, // bc1_rgb_unorm_block,
	{0                                           , GL_RGB            , 0                                }, // bc1_rgb_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // bc1_rgba_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // bc1_rgba_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // bc2_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // bc2_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // bc3_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // bc3_srgb_block,
	{GL_COMPRESSED_RED_RGTC1                     , GL_RED            , 0                                }, // bc4_unorm_block,
	{GL_COMPRESSED_SIGNED_RED_RGTC1              , GL_RED            , 0                                }, // bc4_snorm_block,
	{GL_COMPRESSED_RG_RGTC2                      , GL_RG             , 0                                }, // bc5_unorm_block,
	{GL_COMPRESSED_SIGNED_RG_RGTC2               , GL_RG             , 0                                }, // bc5_snorm_block,
	{GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT       , GL_RGB            , 0                                }, // bc6h_ufloat_block,
	{GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT         , GL_RGB            , 0                                }, // bc6h_sfloat_block,
	{GL_COMPRESSED_RGBA_BPTC_UNORM               , GL_RGBA           , 0                                }, // bc7_unorm_block,
	{GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM         , GL_RGBA           , 0                                }, // bc7_srgb_block,
	{GL_COMPRESSED_RGB8_ETC2                     , GL_RGB            , 0                                }, // etc2_r8g8b8_unorm_block,
	{GL_COMPRESSED_SRGB8_ETC2                    , GL_RGB            , 0                                }, // etc2_r8g8b8_srgb_block,
	{GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 , GL_RGBA           , 0                                }, // etc2_r8g8b8a1_unorm_block,
	{GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA           , 0                                }, // etc2_r8g8b8a1_srgb_block,
	{GL_COMPRESSED_RGBA8_ETC2_EAC                , GL_RGBA           , 0                                }, // etc2_r8g8b8a8_unorm_block,
	{GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC         , GL_RGBA           , 0                                }, // etc2_r8g8b8a8_srgb_block,
	{GL_COMPRESSED_R11_EAC                       , GL_RED            , 0                                }, // eac_r11_unorm_block,
	{GL_COMPRESSED_SIGNED_R11_EAC                , GL_RED            , 0                                }, // eac_r11_snorm_block,
	{GL_COMPRESSED_RG11_EAC                      , GL_RG             , 0                                }, // eac_r11g11_unorm_block,
	{GL_COMPRESSED_SIGNED_RG11_EAC               , GL_RG             , 0                                }, // eac_r11g11_snorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_4x4_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_4x4_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_5x4_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_5x4_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_5x5_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_5x5_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_6x5_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_6x5_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_6x6_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_6x6_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x5_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x5_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x6_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x6_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x8_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_8x8_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x5_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x5_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x6_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x6_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x8_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x8_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x10_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_10x10_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_12x10_unorm_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_12x10_srgb_block,
	{0                                           , GL_RGBA           , 0                                }, // astc_12x12_unorm_block,
	{0                                           , GL_RGBA           , 0                                }  // astc_12x12_srgb_block
};

} // namespace gl

#endif // ANTKEEPER_GL_GL_FORMAT_LUT_HPP

// Santroller GPL-3.0; unmodified macro subset. See NOTICE.md.
#pragma once
#define TUD_HID_REPORT_DESC_PS3()                           \
    HID_REPORT_SIZE(8),                                     \
        TUD_HID_REPORT_DESC_PS3_OUTPUT(),                   \
        HID_USAGE_N(0x2621, 2),                             \
        HID_REPORT_COUNT(32),                               \
        HID_FEATURE(HID_DATA | HID_VARIABLE | HID_ABSOLUTE)

#define TUD_HID_REPORT_DESC_PS3_COMPAT_AXES()                  \
    HID_LOGICAL_MAX_N(255, 2),                                 \
        HID_PHYSICAL_MAX_N(255, 2),                            \
        HID_USAGE(HID_USAGE_DESKTOP_X),                        \
        HID_USAGE(HID_USAGE_DESKTOP_Y),                         \
        HID_USAGE(HID_USAGE_DESKTOP_Z),                         \
        HID_USAGE(HID_USAGE_DESKTOP_RZ)

#define TUD_HID_REPORT_DESC_PS3_COMPAT_BUTTON_BOUNDS()        \
    HID_LOGICAL_MIN(0),                                        \
        HID_LOGICAL_MAX(1),                                    \
        HID_PHYSICAL_MIN(0),                                   \
        HID_PHYSICAL_MAX(1)

#define TUD_HID_REPORT_DESC_PS3_COMPAT_BUTTON_USAGES(count)    \
        HID_USAGE_MIN(0x01),                                   \
        HID_USAGE_MAX(count),                                  \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE)

#define TUD_HID_REPORT_DESC_PS3_OUTPUT()                    \
    HID_USAGE_N(0x2621, 2),                                 \
        HID_REPORT_COUNT(8),                                \
        HID_OUTPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE)

#define TUD_HID_REPORT_DESC_PS3_THIRDPARTY_GAMEPAD()           \
    HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                    \
        HID_USAGE(HID_USAGE_DESKTOP_GAMEPAD),                  \
        HID_COLLECTION(HID_COLLECTION_APPLICATION),            \
        HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),                 \
        TUD_HID_REPORT_DESC_PS3_COMPAT_BUTTON_BOUNDS(),        \
        HID_REPORT_COUNT(13),                                  \
        HID_REPORT_SIZE(1),                                    \
        TUD_HID_REPORT_DESC_PS3_COMPAT_BUTTON_USAGES(13),      \
        HID_REPORT_COUNT(3),                                   \
        HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE), \
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                \
        HID_USAGE(HID_USAGE_DESKTOP_HAT_SWITCH),               \
        HID_LOGICAL_MAX(7),                                    \
        HID_PHYSICAL_MAX_N(315, 2),                            \
        HID_REPORT_COUNT(1),                                   \
        HID_REPORT_SIZE(4),                                    \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),     \
        HID_REPORT_COUNT(1),                                   \
        HID_INPUT(HID_CONSTANT | HID_VARIABLE | HID_ABSOLUTE), \
        HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),                \
        TUD_HID_REPORT_DESC_PS3_COMPAT_AXES(),                 \
        HID_REPORT_COUNT(4),                                   \
        HID_REPORT_SIZE(8),                                    \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),     \
        TUD_HID_REPORT_DESC_PS3_VENDOR(),                      \
        TUD_HID_REPORT_DESC_PS3(),                             \
        HID_USAGE(0x2C),                                       \
        HID_USAGE(0x2D),                                       \
        HID_USAGE(0x2E),                                       \
        HID_USAGE(0x2F),                                       \
        HID_REPORT_COUNT(4),                                   \
        HID_LOGICAL_MAX_N(0x3ff, 2),                           \
        HID_PHYSICAL_MAX_N(0x3ff, 2),                          \
        HID_REPORT_SIZE(16),                                   \
        HID_REPORT_COUNT(4),                                   \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE),     \
        HID_COLLECTION_END

#define TUD_HID_REPORT_DESC_PS3_VENDOR()        \
    HID_USAGE_PAGE_N(HID_USAGE_PAGE_VENDOR, 2), \
        HID_USAGE(0x20),                        \
        HID_USAGE(0x21),                        \
        HID_USAGE(0x22),                        \
        HID_USAGE(0x23),                        \
        HID_USAGE(0x24),                        \
        HID_USAGE(0x25),                        \
        HID_USAGE(0x26),                        \
        HID_USAGE(0x27),                        \
        HID_USAGE(0x28),                        \
        HID_USAGE(0x29),                        \
        HID_USAGE(0x2A),                        \
        HID_USAGE(0x2B),                        \
        HID_REPORT_COUNT(0x0C),                 \
        HID_LOGICAL_MIN(0x00),                  \
        HID_LOGICAL_MAX_N(0xff, 2),             \
        HID_REPORT_SIZE(8),                     \
        HID_INPUT(HID_DATA | HID_VARIABLE | HID_ABSOLUTE)

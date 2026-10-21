#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>   // ATtiny814 has 128 bytes of EEPROM, 8 KB of Flash

// ===========================================================================
//  Cthulhu badge LED controller 
//
//  LED blocks - each has its own color palette and its own special effects:
//    EYES             - the 2 eye LEDs        (EYE_LED_ARRAY)
//    TENTACLES 1-9    - first 9 tentacle LEDs (TENTACLE_LED_ARRAY)  (these are the LEDs in the sigil)
//    TENTACLES 10-17  - last 8 tentacle LEDs  (TENTACLE_LED_ARRAY)  (these are the LEDs in the tentacle curls around the edges)
//
//  SCENES:
//     1 Cthulhu Rises        2 Sleeping in R'lyeh    3 The Deep Ones
//     4 Abyssal Tide         5 Shadow over Innsmouth 6 Call of Cthulhu
//     7 Great Old One        8 Elder Gods            9 When the Stars Are Right
//    10 Eldritch Spectrum   11 Dagon                12 Nyarlathotep
//    13 Yog-Sothoth         14 Necronomicon         15 Starry Wisdom
//   SLOW and DREAMY Scenes (smooth / slow color fades and effects):
//    16 Dreaming in R'lyeh  17 The Drowned City     18 Sunken Fire
//    19 Deep Ones' Lullaby  20 Cosmic Drift         21 Madness Creeping
//    22 The Watcher in the Deep (bright blinking eyes, dim glowing tentacles)
//    23 Elder Sign         24 Crimson Stare (bright red eyes, dim red tentacles)
//    25 The Slow Awakening  (slow, in the Cthulhu Awakens colors)
//   SPECIAL-EFFECT scenes (ripples, surf, glitches, twin comets):
//    26 Ripples of R'lyeh   27 Crashing Waves       28 Whisperer in Darkness
//    29 The Twin Cults      30 Pulse of Elder Things 31 Cthulhu Stirs
//    32 Cthulhu Awakens (twinkles, fades; no lightning)
//                                        <- a new or reset badge starts here
//    33 The Yellow Sign (purple / yellow wave with a strobe every few minutes)
//    34 MY CUSTOM           <- your own look from the menu (never overwritten
//                              by the other scenes)
//    The scene number shows as LEDs. Above 17, ALL 17 LEDs light up: the lit
//    (white / yellow) ones count how far past 17 you are, the rest are teal.
//    The same display is used for palette / animation numbers in the menu.
//
//  EXTRA EFFECTS - every scene also gets some of these on top of its animations:
//    TWINKLE = random LEDs flare up like stars
//    FADE    = a slow wave of dimming and brightening rolls through the LEDs
//    SURGE   = the animation speeds up and slows down every 10 s or so
//    SLOWER  = scenes 1-15 and 26-31 run slower than the base speed (the slow &
//              dreamy scenes 16-25 keep their speed). Movement is smoothed (gliding fades).
//    LIGHTNING (scenes 4 and 28, and the 5-tap egg): the lighting strikes once every few
//              minutes (the first one as soon as you pick the scene); the egg strikes often. The extra effects of each scene are
//              fixed (table SCENE_LAYER); MY CUSTOM copies those of the scene you edit.
//
//  BUTTON:
//    Run mode : short press = next scene (a row of LEDs shows the number: white,
//                 or YELLOW if the scene is a favourite)
//               while AUTO-CYCLING: ONE short press STAYS on the scene you are
//                 looking at and stops the cycling (3 taps + hold also steps it to off)
//               hold (0.8 s) = open the THEME MENU
//             SHORTCUTS - tap quickly, then hold on the last tap:
//               SCENE LIST
//                 2 taps + hold = mark / unmark this scene as a FAVORITE
//                                 (green blink = added, red blink = removed)
//                 3 taps + hold = AUTO-CYCLING through your favourites, one step each time:
//                                 off -> 4 s -> 8 s -> 20 s -> 60 s -> off
//                                 (1-4 green blinks = the step, 2 red blinks = off;
//                                 with no favourites marked it cycles every scene;
//                                 to take a scene out of the cycle, unmark it: 2 taps + hold)
//               HIDDEN EXTRAS
//                 4 taps + hold = MORSE message on the eyes
//                                 ("IA IA CTHULHU FHTAGN"); any press stops it
//                 5 taps        = "Cthulhu Awakens" show with lightning (12 s)
//               MENUS
//                 hold          = THEME MENU (palettes and animations of MY CUSTOM)
//                 1 tap + hold  = SETTINGS MENU (brightness, eye brightness, speed)
//    In a menu: short press = change the value on the current page
//               hold        = go to the next page (after the last page the
//                             menu closes and your choices are SAVED)
//               no press for 15 s = menu closes and saves
//    POWER-UP : hold the button while plugging the badge in:
//                 eyes turn RED after 8 s  -> let go now = RESET everything to default
//                 Let go before 8 s = normal start.
//    RESET    : the power-up hold above is the only way (it clears favourites too).
//
//  THEME MENU (hold): a row of BLUE tentacle LEDs shows the page number on entry, every 5th
//  LED magenta to make it easy to count. The block you are editing stays bright and the
//  others dim: PALETTE pages show the palette itself as a steady swatch (one colour per
//  LED), ANIMATION pages play the animation live.
//     1  Eyes palette              2  Eyes animation
//     3  Tentacles 1-9 palette     4  Tentacles 1-9 animation
//     5  Tentacles 10-17 palette   6  Tentacles 10-17 animation
//  Changing anything here copies the scene you are looking at into MY CUSTOM (the last
//  scene) and edits it there. After page 6 the menu closes and SAVES.
//
//  COLO\R OPTIONS (palette pages 1, 3, 5 - each press moves to the next, in this order):
//     1  Great Old One            reds, olive-greens, yellow
//     2  Call of Cthulhu          reds, bright to deep, yellow
//     3  Shadow over Innsmouth    sky blue to deep blue, red
//     4  The Deep Ones            greens, light to dark, yellow
//     5  Abyssal Tide             pale aqua, blues, white
//     6  Elder Gods               reds, bright to deep, dark red-brown
//     7  When the Stars Are Right oranges, white
//     8  Eldritch Spectrum        red, yellow, orange, teal
//     9  Cthulhu Rises            red, green, blue, orange, teal
//    10  Sleeping in R'lyeh       blues, light to deep, white
//    11  Cthulhu Awakened         teal, Cthulhu green, sea green, deep teal, pale aqua
//
//  ANIMATION OPTIONS (animation pages 2, 4, 6 - same order, then back to 1):
//     1  Drift          the original scrolling palette
//     2  Slumber        slow breathing
//     3  Rising tide    a wave of brightness rolls down the tentacles
//     4  Storm          dim glow with lightning (about once every few minutes)
//     5  Stars          random twinkling stars
//     6  Watchful blink normal glow that blinks off now and then
//     7  Heartbeat     slow double "lub-dub" pulse
//     8  Comet          a bright comet with a fading tail chases along the LEDs
//     9  Ember          smooth random flicker, like glowing embers
//    10  Aurora         two slow waves of light drifting through each other
//    11  Scanner        a bright dot sweeps back and forth
//    12  Fairy          every other LED swaps on and off
//    13  Swell          one huge slow ocean swell
//    14  Crossfade      the block slowly melts from one palette colour into the next
//    15  Currents       a smooth gradient of palette colours slowly flows along
//    16  Fog            dim, slow, overlapping waves of colour, like mist
//    17  Gaze           slowly opens, holds, slowly closes (eyes: one after the other)
//    18  Ripple         soft rings spread out from the middle of the tentacles
//    19  Surf           waves crash in with a bright edge and slowly drain away
//    20  Twin comets    two comets run in opposite directions and pass through
//    21  Glitch         dim static with random bright flashes
//    22  Dim            a dim, slowly breathing glow (keeps the eyes standing out)
//    23  Bloom          soft rings spread out from the middle of the sigil (tentacles 1-9)
//    24  Spin           a bright segment circles round the 9 sigil LEDs
//  Animations 14-19 also blend smoothly between the palette colors.
//  The extra effects (twinkle, fade wave, speed surge) are not menu choices: MY CUSTOM
//  keeps those of the scene you started from.
//
//  SETTINGS MENU (1 tap + hold): these apply to ALL scenes, not just MY CUSTOM. A row of
//  ORANGE LEDs shows the page number; the value shows as a row of WHITE LEDs (1 = first):
//     1  Brightness, all LEDs (1 = brightest ... 4 = dimmest)
//     2  Eye brightness, relative to the rest (1 = same ... 4 = dimmest)
//     3  Speed      (1 = slow, 2 = normal, 3 = fast)
//  (Favourites and auto-cycling are set with the tap shortcuts, not in a menu.)
// ===========================================================================

#define EYE_LED_ARRAY 5
#define NUM_EYE_LEDS 2
#define TENTACLE_LED_ARRAY 4
#define NUM_TENTACLE_LEDS 17
#define NUM_TENTACLE_LEDS_ONE_TO_NINE 9                                         // tentacle LEDs 1-9
#define NUM_TENTACLE_LEDS_TEN_TO_SEVENTEEN (NUM_TENTACLE_LEDS - NUM_TENTACLE_LEDS_ONE_TO_NINE) // LEDs 10-17 (8)
#define NUM_SLOTS (NUM_EYE_LEDS + NUM_TENTACLE_LEDS)
#define BUTTON_PIN 1
#define MINIBADGE_CLK_CTRL_H_PIN 3
#define MINIBADGE_CLK_CTRL_L_PIN 2

#define FRAME_MS 40                // animation frame time (25 frames per second)
#define DEBOUNCE_MS 30
#define LONG_PRESS_MS 800
#define MENU_TIMEOUT_MS 15000UL
#define MENU_DIM_SHIFT 3           // non-selected blocks are dimmed to 1/8 in the menu
#define NUM_CYCLE_LEVELS 4         // auto-cycle time steps (see AUTO_CYCLE_TABLE)
#define MORSE_UNIT_MS 160          // length of one Morse "dot"
#define TAP_GAP_MS 800UL           // tap-tap-hold shortcuts: taps (and the final hold) must follow each other this fast
// LED current limit: every frame is scaled down if the LEDs would draw more than
// this. A WS2812 color channel at full draws about 20 mA (all 19 LEDs white = 1.1 A!).
#define MAX_LED_CURRENT_MA 350
#define PAGE_OVERLAY_MS 1000UL     // how long the page-number LEDs stay lit
#define VALUE_OVERLAY_MS 800UL     // how long the value LEDs stay lit
#define EGG_PRESSES 5              // quick presses needed for the easter egg...
#define EGG_WINDOW_MS 2500UL       // ...within this time
#define EGG_DURATION_MS 12000UL    // how long the hidden show runs
#define SCENE_OVERLAY_MS 500UL     // how long the scene-number LEDs stay lit
#define RESET_HOLD_MS 8000         // hold the button this long at power-up to reset

// Scenes: 0-30 use the palette/animation table below, 31 is the special
// "Cthulhu Awakens" look, 32 is your own custom look.
// (NUM_SCENES must stay at 34 or less: the scene number is shown on the 17 tentacle LEDs.)
#define NUM_PRESETS 33
#define SCENE_AWAKENS 31
#define SCENE_YELLOW 32      // the Yellow Sign show
#define SCENE_CUSTOM 33
#define NUM_SCENES 34
#define FAV_BYTES 5                 // one bit per scene
#if NUM_SCENES > FAV_BYTES * 8
#error "FAV_BYTES too small for NUM_SCENES"
#endif
#if NUM_SCENES > 34
#error "Too many scenes to show the scene number on 17 LEDs"
#endif

// --- saved settings (EEPROM) -----------------------------------------------
#define EEPROM_ADDR 0
#define EEPROM_MAGIC 0xD0         

enum {
    GREAT_OLD_ONE,             
    CALL_OF_CTHULHU,           
    SHADOW_OVER_INNSMOUTH,    
    THE_DEEP_ONES,             
    ABYSSAL_TIDE,              
    ELDER_GODS,                
    WHEN_THE_STARS_ARE_RIGHT,  
    ELDRITCH_SPECTRUM,         
    CTHULHU_RISES,             
    SLEEPING_IN_RLYEH,         
    CTHULHU_AWAKENED,          
    NUM_LED_MODES
};
typedef uint8_t LedMode;

// Animations - how a block moves. Any block can use any animation.
enum {
    FX_DRIFT,           // 1: the original scrolling palette
    FX_SLUMBER,         // 2: slow breathing (great for sleeping Cthulhu eyes)
    FX_RISING_TIDE,     // 3: a wave of brightness rolls down the tentacles
    FX_STORM,           // 4: dim glow with random lightning flashes
    FX_STARS,           // 5: random twinkling stars
    FX_WATCHFUL_BLINK,  // 6: normal glow that blinks off now and then
    FX_HEARTBEAT,       // 7: slow double "lub-dub" pulse
    FX_COMET,           // 8: a bright comet with a fading tail chases along the LEDs
    FX_EMBER,           // 9: smooth random flicker, like glowing embers
    FX_AURORA,          // 10: two slow waves of light drifting through each other
    FX_SCANNER,         // 11: a bright dot sweeps back and forth, like a scanner
    FX_FAIRY,           // 12: every other LED swaps on and off, like fairy lights
    FX_SWELL,           // 13: one huge slow ocean swell rolling through
    // --- SLOW effects: these also blend smoothly BETWEEN the palette colours ---
    FX_CROSSFADE,       // 14: the whole block slowly melts from one palette colour into the next
    FX_CURRENTS,        // 15: a smooth gradient of palette colours slowly flows along the LEDs
    FX_FOG,             // 16: dim, slow, overlapping waves of colour, like mist rolling in
    FX_GAZE,            // 17: slowly opens to full glow, holds, slowly closes (eyes: one after the other)
    FX_RIPPLE,          // 18: soft rings of light spread outward from the middle of the tentacles
    FX_SURF,            // 19: waves that crash in with a bright edge and slowly drain away
    // --- special effects (these use the normal scrolling palette) ---
    FX_TWIN_COMETS,     // 20: two comets run in opposite directions and pass through each other
    FX_GLITCH,          // 21: reality tearing: dim static with random bright flashes
    FX_DIM,             // 22: a dim, slowly breathing glow (put it on the tentacles so the eyes stand out)
    FX_BLOOM,           // 23: soft rings spread out from the middle of the SIGIL (tentacles 1-9)
    FX_SPIN,            // 24: a bright segment circles round the 9 sigil LEDs
    NUM_EFFECTS
};
typedef uint8_t EffectMode;

#define FIRST_SMOOTH_FX FX_CROSSFADE   // effects FIRST_SMOOTH_FX..LAST_SMOOTH_FX blend smoothly between palette colours
#define LAST_SMOOTH_FX FX_SURF

enum {
    BLOCK_EYES,
    BLOCK_TENTACLES_1_TO_9,
    BLOCK_TENTACLES_10_TO_17,
    NUM_LED_BLOCKS
};
typedef uint8_t LedBlock;

// Pages 0-5 map onto blocks: page/2 = block, even = palette, odd = animation.
enum MenuPage {
    PAGE_EYES_COLOR, PAGE_EYES_EFFECT,
    PAGE_T1_COLOR,   PAGE_T1_EFFECT,
    PAGE_T2_COLOR,   PAGE_T2_EFFECT,
    PAGE_BRIGHTNESS, PAGE_EYE_BRIGHTNESS, PAGE_SPEED,
    NUM_PAGES
};

enum { UI_RUN, UI_MENU };
typedef uint8_t UiState;

#define NUM_BRIGHTNESS_LEVELS 4
#define NUM_SPEED_LEVELS 3
const uint8_t BRIGHTNESS_TABLE[NUM_BRIGHTNESS_LEVELS] = { 255, 150, 80, 35 };
const uint8_t SPEED_INC[NUM_SPEED_LEVELS] = { 1, 2, 4 };   // 2 = the original 200 ms scroll step
const uint32_t AUTO_CYCLE_TABLE[NUM_CYCLE_LEVELS] = { 4000UL, 8000UL, 20000UL, 60000UL };   // ms per scene

// Hidden message, sent in Morse on the eyes. . = dot, - = dash, space = letter gap, / = word gap.
const char MORSE_MSG[] = ".. .-/.. .-/-.-. - .... ..- .-.. .... ..-/..-. .... - .- --. -.";   // IA IA CTHULHU FHTAGN

// ---- Scene table ----------------------------------------------------------
// One row per scene: palette and animation for { eyes, tentacles 1-9, tentacles 10-17 }.
const uint8_t SCENE_MODE[NUM_PRESETS][NUM_LED_BLOCKS] = {
    { CTHULHU_RISES,            CTHULHU_RISES,            CTHULHU_RISES },             //  1 Cthulhu Rises
    { SLEEPING_IN_RLYEH,        SLEEPING_IN_RLYEH,        SLEEPING_IN_RLYEH },         //  2 Sleeping in R'lyeh
    { THE_DEEP_ONES,            THE_DEEP_ONES,            THE_DEEP_ONES },             //  3 The Deep Ones
    { ABYSSAL_TIDE,             ABYSSAL_TIDE,             ABYSSAL_TIDE },              //  4 Abyssal Tide
    { SHADOW_OVER_INNSMOUTH,    SHADOW_OVER_INNSMOUTH,    SHADOW_OVER_INNSMOUTH },     //  5 Shadow over Innsmouth
    { CALL_OF_CTHULHU,          CALL_OF_CTHULHU,          CALL_OF_CTHULHU },           //  6 Call of Cthulhu
    { GREAT_OLD_ONE,            GREAT_OLD_ONE,            GREAT_OLD_ONE },             //  7 Great Old One
    { ELDER_GODS,               ELDER_GODS,               ELDER_GODS },                //  8 Elder Gods
    { WHEN_THE_STARS_ARE_RIGHT, WHEN_THE_STARS_ARE_RIGHT, WHEN_THE_STARS_ARE_RIGHT },  //  9 When the Stars Are Right
    { ELDRITCH_SPECTRUM,        ELDRITCH_SPECTRUM,        ELDRITCH_SPECTRUM },         // 10 Eldritch Spectrum
    { THE_DEEP_ONES,            SLEEPING_IN_RLYEH,        THE_DEEP_ONES },             // 11 Dagon (green eyes, blue sigil)
    { SHADOW_OVER_INNSMOUTH,    ELDRITCH_SPECTRUM,        CALL_OF_CTHULHU },           // 12 Nyarlathotep (chaos)
    { ABYSSAL_TIDE,             WHEN_THE_STARS_ARE_RIGHT, ABYSSAL_TIDE },              // 13 Yog-Sothoth
    { GREAT_OLD_ONE,            ELDER_GODS,               GREAT_OLD_ONE },             // 14 Necronomicon
    { WHEN_THE_STARS_ARE_RIGHT, SHADOW_OVER_INNSMOUTH,    WHEN_THE_STARS_ARE_RIGHT },  // 15 Starry Wisdom
    { SLEEPING_IN_RLYEH,        SLEEPING_IN_RLYEH,        SLEEPING_IN_RLYEH },         // 16 Dreaming in R'lyeh (slow)
    { ABYSSAL_TIDE,             ABYSSAL_TIDE,             ABYSSAL_TIDE },              // 17 The Drowned City (slow)
    { GREAT_OLD_ONE,            GREAT_OLD_ONE,            GREAT_OLD_ONE },             // 18 Sunken Fire (slow)
    { THE_DEEP_ONES,            SLEEPING_IN_RLYEH,        THE_DEEP_ONES },             // 19 Deep Ones' Lullaby (slow)
    { WHEN_THE_STARS_ARE_RIGHT, WHEN_THE_STARS_ARE_RIGHT, WHEN_THE_STARS_ARE_RIGHT },  // 20 Cosmic Drift (slow)
    { ELDRITCH_SPECTRUM,        ELDRITCH_SPECTRUM,        ELDRITCH_SPECTRUM },         // 21 Madness Creeping (slow)
    { THE_DEEP_ONES,            ABYSSAL_TIDE,             ABYSSAL_TIDE },              // 22 The Watcher in the Deep (bright blinking green eyes, dim tentacles)
    { ELDER_GODS,               ELDER_GODS,               ELDER_GODS },                // 23 Elder Sign (slow)
    { CALL_OF_CTHULHU,          ELDER_GODS,               ELDER_GODS },                // 24 Crimson Stare (bright red eyes, dim red tentacles)
    { CTHULHU_AWAKENED,         CTHULHU_AWAKENED,         CTHULHU_AWAKENED },          // 25 The Slow Awakening (slow)
    { ABYSSAL_TIDE,             ABYSSAL_TIDE,             ABYSSAL_TIDE },              // 26 Ripples of R'lyeh
    { THE_DEEP_ONES,            ABYSSAL_TIDE,             SLEEPING_IN_RLYEH },         // 27 Crashing Waves
    { SHADOW_OVER_INNSMOUTH,    SHADOW_OVER_INNSMOUTH,    SHADOW_OVER_INNSMOUTH },     // 28 The Whisperer in Darkness
    { CALL_OF_CTHULHU,          CALL_OF_CTHULHU,          CALL_OF_CTHULHU },           // 29 The Twin Cults
    { ELDER_GODS,               ELDER_GODS,               ELDER_GODS },                // 30 Pulse of the Elder Things
    { CTHULHU_AWAKENED,         CTHULHU_AWAKENED,         CTHULHU_AWAKENED },          // 31 Cthulhu Stirs
    { CTHULHU_RISES,            CTHULHU_RISES,            CTHULHU_RISES },             // 32 Cthulhu Awakens (drawn specially)
    { CTHULHU_RISES,            CTHULHU_RISES,            CTHULHU_RISES },             // 33 The Yellow Sign (drawn specially)
};
const uint8_t SCENE_EFFECT[NUM_PRESETS][NUM_LED_BLOCKS] = {
    { FX_HEARTBEAT,        FX_SPIN,         FX_TWIN_COMETS },  //  1 beating eyes, a segment circling the sigil, twin comets in the curls
    { FX_SLUMBER,          FX_AURORA,       FX_RISING_TIDE },  //  2 slow and dreamy, light waves in the sigil, a tide rolling through the curls
    { FX_WATCHFUL_BLINK,   FX_BLOOM,        FX_HEARTBEAT },  //  3 watching, rings blooming in the sigil, pulsing curls
    { FX_SLUMBER,          FX_STORM,        FX_SURF },  //  4 storm at sea: LIGHTNING in the sigil, surf in the curls
    { FX_WATCHFUL_BLINK,   FX_GLITCH,       FX_COMET },  //  5 static in the sigil, a comet in the curls
    { FX_HEARTBEAT,        FX_SCANNER,      FX_EMBER },  //  6 beating eyes, a scanner in the sigil, embers in the curls
    { FX_SLUMBER,          FX_EMBER,        FX_RIPPLE },  //  7 fire and rings
    { FX_WATCHFUL_BLINK,   FX_SPIN,         FX_COMET },  //  8 a segment circling the sigil, a comet in the curls
    { FX_HEARTBEAT,        FX_STARS,        FX_GLITCH },  //  9 stars and static
    { FX_DRIFT,            FX_TWIN_COMETS,  FX_SURF },  // 10 colourful comets and surf
    { FX_SLUMBER,          FX_SURF,         FX_SCANNER },  // 11 surf in the sigil, scanner in the curls
    { FX_GLITCH,           FX_FAIRY,        FX_TWIN_COMETS },  // 12 glitching eyes, fairy lights and twin comets
    { FX_SWELL,            FX_STARS,        FX_RIPPLE },  // 13 swelling eyes, stars and rings
    { FX_EMBER,            FX_SWELL,        FX_SCANNER },  // 14 embers, a slow swell through the sigil, scanner
    { FX_STARS,            FX_COMET,        FX_FAIRY },  // 15 twinkling eyes, a comet in the sigil, fairy lights
    { FX_GAZE,             FX_CROSSFADE,    FX_FOG },  // 16 eyes slowly open; sigil melts colour to colour
    { FX_FOG,              FX_CURRENTS,     FX_FOG },  // 17 mist, with a flowing gradient in the sigil
    { FX_SLUMBER,          FX_GAZE,         FX_CURRENTS },  // 18 slow fire: the sigil opens LED by LED, smouldering gradient
    { FX_SLUMBER,          FX_BLOOM,        FX_SWELL },  // 19 breathing eyes, rings blooming in the sigil, long ocean swell
    { FX_CROSSFADE,        FX_CURRENTS,     FX_CROSSFADE },  // 20 slow orange/white drift
    { FX_CROSSFADE,        FX_SWELL,        FX_CURRENTS },  // 21 slow rainbow creeping through everything
    { FX_WATCHFUL_BLINK,   FX_DIM,          FX_DIM },  // 22 bright watching eyes, dim glowing tentacles
    { FX_GAZE,             FX_RIPPLE,       FX_FOG },  // 23 red glow, slow open and close, rings in the sigil
    { FX_DRIFT,            FX_DIM,          FX_DIM },  // 24 bright steady eyes, dim glowing tentacles
    { FX_GAZE,             FX_CURRENTS,     FX_FOG },  // 25 slow: eyes open, teal gradient flows, sea mist
    { FX_SLUMBER,          FX_BLOOM,        FX_RIPPLE },  // 26 rings blooming in the sigil, rings spreading across the curls
    { FX_WATCHFUL_BLINK,   FX_SURF,         FX_SURF },  // 27 waves crashing in
    { FX_WATCHFUL_BLINK,   FX_STORM,        FX_GLITCH },  // 28 LIGHTNING in the sigil, static in the curls
    { FX_HEARTBEAT,        FX_SPIN,         FX_TWIN_COMETS },  // 29 a segment circling the sigil, two comets crossing, red
    { FX_HEARTBEAT,        FX_RIPPLE,       FX_RIPPLE },  // 30 a beating heart with rings of red
    { FX_HEARTBEAT,        FX_BLOOM,        FX_SURF },  // 31 teal rings blooming in the sigil, surf in the curls
    { FX_DRIFT,            FX_DRIFT,        FX_DRIFT },  // 32 (unused - drawn specially)
    { FX_DRIFT,            FX_DRIFT,        FX_DRIFT },  // 33 (unused - drawn specially)
};

// Ambient layers: extra life that every scene gets on top of its animations.
//   TWINKLE = random LEDs flare up like stars, FADE = a slow wave of dimming and brightening
//   rolls through the LEDs, SURGE = the animation speeds up and slows down over ~20 s.
#define LAYER_TWINKLE 1
#define LAYER_FADE 2
#define LAYER_SURGE 4
#define LAYER_SLOWER 8      // runs the animations slower than the base speed (not used by the slow dreamy scenes)
const uint8_t SCENE_LAYER[NUM_PRESETS] = {
    13, 15, 12, 14, 13, 14, 15, 13, 14, 13,   //  1-10
    14, 12, 15, 14, 12, 7, 6, 6, 7, 6,        // 11-20
    7, 7, 7, 7, 7, 13, 15, 12, 15, 13,        // 21-30
    14, 7, 0 };                        // 31, 32 (Awakens), 33 (Yellow Sign): own looks



// ---- Settings (saved) -----------------------------------------------------
// blockMode/blockEffect are the LIVE look (copied from the current scene).
LedMode blockMode[NUM_LED_BLOCKS] = { CTHULHU_RISES, CTHULHU_RISES, CTHULHU_RISES };
EffectMode blockEffect[NUM_LED_BLOCKS] = { FX_DRIFT, FX_DRIFT, FX_DRIFT };        // = Cthulhu Awakens (the default scene)
// Everything that is saved lives in this one array; the index IS the EEPROM address.
// Defaults: Cthulhu Awakens, MY CUSTOM = scene 1, normal brightness / speed, no favourites.
uint8_t cfg[19] = { EEPROM_MAGIC, SCENE_AWAKENS,
                    CTHULHU_RISES, CTHULHU_RISES, CTHULHU_RISES,
                    FX_HEARTBEAT, FX_AURORA, FX_COMET,
                    0, 0, 1, 0, 1,
                    0, 0, 0, 0, 0,
                    LAYER_TWINKLE | LAYER_FADE };
#define currentScene cfg[1]
#define customMode (cfg + 2)           // MY CUSTOM palettes, one per block
#define customEffect (cfg + 5)         // MY CUSTOM animations
#define brightnessLevel cfg[8]
#define eyeBrightnessLevel cfg[9]      // eyes are scaled by this on top of the main brightness
#define speedLevel cfg[10]
#define autoCycle cfg[11]
#define cycleLevel cfg[12]             // index into AUTO_CYCLE_TABLE (default 8 s)
#define favMask (cfg + 13)             // one bit per scene: favourite for auto-cycle
#define customLayer cfg[18]           // ambient layers of MY CUSTOM
// MY CUSTOM scene (scene 17) - edited from the menu, saved in EEPROM.
uint8_t liveLayer = LAYER_TWINKLE | LAYER_FADE;       // ambient layers of the scene on screen

// ---- Runtime state --------------------------------------------------------
UiState uiState = UI_RUN;
uint8_t menuPage = 0;
bool menuSettings = false;           // the settings menu (brightness, eye brightness, speed)
uint32_t lastMenuActivity = 0;

uint32_t overlayUntil = 0;           // page / value indicator on the tentacle LEDs
uint8_t overlayCount = 0;
uint8_t overlay2Count = 0;           // LEDs up to this count (after the first group) use the 2nd colour
uint8_t overlay2G = 0, overlay2R = 0, overlay2B = 0;
uint8_t overlayG = 0, overlayR = 0, overlayB = 0;

uint32_t eggUntil = 0;               // easter egg
uint8_t eggPresses = 0;
uint32_t eggWindowStart = 0;

uint8_t ledIndex = 0;                // scroll position
uint8_t stepAcc = 0;
uint8_t surgeAcc = 0;                // fractional animation speed
uint16_t phase = 0;                  // free-running animation phase
uint16_t slow = 0;                   // phase for the slow colour fades; wraps at 10240 so the colours never jump
uint8_t stormFlash = 0;
uint16_t stormGap = 0;               // frames until the next strike 
uint8_t blinkFrames = 0;
uint16_t blinkCountdown = 100;
uint8_t sparkle[NUM_SLOTS];
uint8_t emberLevel[NUM_SLOTS];       // current ember brightness per LED
uint8_t emberTarget[NUM_SLOTS];      // brightness it is drifting towards
uint16_t rngState = 0xACE1;
uint32_t lastAutoCycle = 0;
bool morseActive = false;            // hidden Morse message
bool morseOn = false;                // eyes lit right now
uint8_t morseIdx = 0;
uint32_t morseNextAt = 0;
uint32_t lastShortAt = 0;            // when the last short press ended
bool blankOut = false;               // LEDs dark while a tap-tap-hold shortcut is being held (hides the scene steps)
uint8_t tapState = 0;                // auto-cycle state when the taps began: 0 = off, 1-4 = step length
uint8_t tapMoves = 0;                // how many of those taps moved the scene on
uint8_t tapRun = 0;                  // quick taps in a row (for the tap-tap-hold shortcuts)

// Forward declarations (the original relied on the Arduino auto-prototypes)
void sendByte(uint8_t byte, uint8_t ledPin);
void sendColor(uint8_t g, uint8_t r, uint8_t b, uint8_t ledPin);

// ---------------------------------------------------------------------------
//  Scenes
// ---------------------------------------------------------------------------

// Make 'scene' the live look.
void applyScene(uint8_t scene) {
    currentScene = scene;
    liveLayer = (scene == SCENE_CUSTOM) ? customLayer : SCENE_LAYER[scene];
    for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
        if (scene == SCENE_CUSTOM) {
            blockMode[i] = customMode[i];
            blockEffect[i] = customEffect[i];
        } else {
            blockMode[i] = (LedMode)SCENE_MODE[scene][i];
            blockEffect[i] = (EffectMode)SCENE_EFFECT[scene][i];
        }
    }
}

// ---------------------------------------------------------------------------
//  Saved settings (EEPROM). eeprom_update_byte only writes changed bytes,
//  so the EEPROM wears very little.
//  Layout: [0]=magic [1]=scene [2-4]=custom palettes [5-7]=custom animations
//          [8]=brightness [9]=eye brightness [10]=speed [11]=auto-cycle
//          [12]=auto-cycle time [13-17]=favourite scenes (bit mask) [18]=custom layers
// ---------------------------------------------------------------------------
void saveSettings() {
    for (uint8_t i = 0; i < sizeof(cfg); i++)
        eeprom_update_byte((uint8_t*)(EEPROM_ADDR + i), cfg[i]);
}

// Largest allowed value + 1 for each saved byte (0 = anything goes); bad data is ignored.
const uint8_t LIMIT[19] = { 0, 0,
    NUM_LED_MODES, NUM_LED_MODES, NUM_LED_MODES,
    NUM_EFFECTS, NUM_EFFECTS, NUM_EFFECTS,
    NUM_BRIGHTNESS_LEVELS, NUM_BRIGHTNESS_LEVELS, NUM_SPEED_LEVELS, 2, NUM_CYCLE_LEVELS,
    0, 0, 0, 0, 0,
    16 };

void loadSettings() {
    if (eeprom_read_byte((const uint8_t*)EEPROM_ADDR) != EEPROM_MAGIC) return; // nothing saved yet
    for (uint8_t i = 2; i < sizeof(cfg); i++) {
        uint8_t v = eeprom_read_byte((const uint8_t*)(EEPROM_ADDR + i));
        if (!LIMIT[i] || v < LIMIT[i]) cfg[i] = v;
    }
    uint8_t sc = eeprom_read_byte((const uint8_t*)(EEPROM_ADDR + 1));
    if (sc < NUM_SCENES) applyScene(sc);
}

void saveScene() { eeprom_update_byte((uint8_t*)(EEPROM_ADDR + 1), currentScene); }

// Favourite scenes (for auto-cycle).
bool isFavorite(uint8_t scene) { return (favMask[scene >> 3] >> (scene & 7)) & 1; }

bool flipFavorite() {                            // mark / unmark the scene on screen; true = now a favourite
    favMask[currentScene >> 3] ^= (1 << (currentScene & 7));
    return isFavorite(currentScene);
}

uint8_t sceneStep(uint8_t from, int8_t d) {      // scene number d steps away (wraps round)
    return (uint8_t)(from + NUM_SCENES + d) % NUM_SCENES;
}

// Next scene for auto-cycle: the next favourite, or simply the next scene if none are marked.
uint8_t nextAutoScene() {
    bool any = false;
    for (uint8_t i = 0; i < FAV_BYTES; i++) if (favMask[i]) any = true;
    uint8_t sc = currentScene;
    for (uint8_t k = 0; k < NUM_SCENES; k++) {
        sc = sceneStep(sc, 1);
        if (!any || isFavorite(sc)) return sc;
    }
    return currentScene;
}

// Everything back to the default look: Cthulhu Awakens, custom = scene 1, normal settings.
void resetToDefaults() {
    for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
        customMode[i] = (LedMode)SCENE_MODE[0][i];
        customEffect[i] = (EffectMode)SCENE_EFFECT[0][i];
    }
    for (uint8_t i = 8; i < 18; i++) cfg[i] = 0;     // brightness, auto-cycle off, no favourites ...
    speedLevel = 1;
    cycleLevel = 1;
    customLayer = SCENE_LAYER[0];
    applyScene(SCENE_AWAKENS);
    saveSettings();
}

// ---------------------------------------------------------------------------
//  Colour palettes. One row per LedMode, 5 colours each, stored as {G, R, B}
//  (the order the LEDs want). WHEN_THE_STARS_ARE_RIGHT only uses its first 4.
// ---------------------------------------------------------------------------
const uint8_t PALETTE[NUM_LED_MODES][5][3] = {
    { {0,200,0}, {0,255,0}, {165,90,0}, {75,40,0}, {255,255,0} },  // GREAT_OLD_ONE: Light Red/Bright Red/Light Brown/Dark Brown/Yellow
    { {0,200,0}, {0,255,0}, {0,150,0}, {0,100,0}, {255,255,0} },  // CALL_OF_CTHULHU: Light Red/Medium Red/Dark Red/Deep Red/Yellow
    { {128,0,255}, {75,0,150}, {0,0,200}, {0,0,100}, {0,255,0} },  // SHADOW_OVER_INNSMOUTH: Light Purple/Dark Purple/Light Blue/Dark Blue/Red
    { {200,0,0}, {150,0,0}, {100,0,0}, {50,0,0}, {255,255,0} },  // THE_DEEP_ONES: Light Green/Medium Light Green/Medium Green/Dark Green/Yellow-Green
    { {128,64,128}, {0,0,255}, {0,0,200}, {0,0,100}, {255,255,255} },  // ABYSSAL_TIDE: Light Purple/Dark Purple/Light Blue/Dark Blue/White
    { {0,255,0}, {0,200,0}, {0,150,0}, {0,100,0}, {10,40,0} },  // ELDER_GODS: Bright Red/Medium Red/Dark Red/Deeper Red/Dark Brown
    { {165,255,0}, {140,255,0}, {100,200,0}, {255,255,255}, {0,0,0} },  // WHEN_THE_STARS_ARE_RIGHT: Light Orange/Medium Orange/Dark Orange/White
    { {0,255,0}, {255,255,0}, {165,255,0}, {0,255,0}, {128,0,128} },  // ELDRITCH_SPECTRUM: Red/Yellow/Orange/Green/Purple
    { {0,255,0}, {255,0,0}, {0,0,255}, {165,255,0}, {128,0,128} },  // CTHULHU_RISES: Red/Green/Blue/Orange/Purple
    { {0,0,255}, {0,0,200}, {0,0,100}, {0,0,50}, {255,255,255} },  // SLEEPING_IN_RLYEH: Light Blue/Medium Blue/Dark Blue/Deep Blue/White
    { {160,10,90}, {255,0,16}, {110,0,60}, {40,0,30}, {210,40,150} },  // CTHULHU_AWAKENED: Teal (tentacles)/Cthulhu green (eyes)/Sea green/Deep teal/Pale aqua
};

uint8_t paletteSize(LedMode m) { return (m == WHEN_THE_STARS_ARE_RIGHT) ? 4 : 5; }

const uint8_t* getModeColor(LedMode mode, uint8_t index) {
    return PALETTE[mode][index % paletteSize(mode)];
}

uint8_t col[3];                // the colour (G, R, B) the two functions below work out

// The colour of LED 'i' of a block. The eyes step through the palette together;
// each tentacle block spreads the palette over its own LEDs and scrolls it with 'step'.
void blockPaletteColor(LedBlock block, uint8_t i, uint8_t step) {
    uint8_t idx = step;
    if (block != BLOCK_EYES)
        idx = (uint8_t)((uint16_t)(i + step) * 256u /
              ((block == BLOCK_TENTACLES_1_TO_9) ? NUM_TENTACLE_LEDS_ONE_TO_NINE : NUM_TENTACLE_LEDS_TEN_TO_SEVENTEEN));
    const uint8_t* c = getModeColor(blockMode[block], idx);
    col[0] = c[0]; col[1] = c[1]; col[2] = c[2];
}

// ---------------------------------------------------------------------------
//  LED output
// ---------------------------------------------------------------------------

void sendColor(uint8_t g, uint8_t r, uint8_t b, uint8_t ledPin) {
    sendByte(g, ledPin);
    sendByte(r, ledPin);
    sendByte(b, ledPin);
}

void sendByte(uint8_t byte, uint8_t ledPin) {
    for (int i = 0; i < 8; i++) {
        if (byte & (1 << (7 - i))) {
            PORTA.OUTSET = (1 << ledPin);
            _delay_us(0.4);
            PORTA.OUTCLR = (1 << ledPin);
            _delay_us(0.85);
        } else {
            PORTA.OUTSET = (1 << ledPin);
            _delay_us(0.2);
            PORTA.OUTCLR = (1 << ledPin);
            _delay_us(0.9);
        }
    }
}

void colorWipe(uint8_t green, uint8_t red, uint8_t blue, uint8_t ledPin, uint8_t numLeds) {
    for (uint8_t i = 0; i < numLeds; i++) {
        sendColor(green, red, blue, ledPin);
        _delay_us(500);
    }
}

// ---------------------------------------------------------------------------
//  Small helpers
// ---------------------------------------------------------------------------

bool timeLeft(uint32_t until) {            // true while millis() is before 'until'
    return (int32_t)(until - millis()) > 0;
}

uint8_t rand8() {                          // tiny xorshift pseudo-random generator
    rngState ^= rngState << 7;
    rngState ^= rngState >> 9;
    rngState ^= rngState << 8;
    return (uint8_t)(rngState ^ (rngState >> 8));
}

uint8_t scale8(uint8_t v, uint8_t s) {     // v * s / 255 (s = 255 leaves v unchanged)
    return ((uint16_t)v * ((uint16_t)s + 1)) >> 8;
}

uint8_t absd(uint8_t a, uint8_t b) { return (a > b) ? a - b : b - a; }   // distance between two numbers

uint8_t tri8(uint8_t x) {                  // triangle wave 0..254
    return (x < 128) ? (uint8_t)(x << 1) : (uint8_t)((255 - x) << 1);
}

LedMode nextMode(LedMode m)       { return (m + 1 >= NUM_LED_MODES) ? 0 : m + 1; }
EffectMode nextEffect(EffectMode e) { return (e + 1 >= NUM_EFFECTS) ? 0 : e + 1; }


// In the menu, pages 1-6 dim every block except the one being edited.
bool blockIsDimmed(LedBlock block) {
    return (uiState == UI_MENU) && (menuPage < PAGE_BRIGHTNESS) && (block != (LedBlock)(menuPage / 2));
}

// Show 'count' LEDs on the tentacle chain in a fixed color for a moment.
void showOverlay(uint8_t count, uint8_t g, uint8_t r, uint8_t b, uint32_t ms) {
    overlayCount = count;
    overlayG = g; overlayR = r; overlayB = b;
    overlay2Count = 0;
    overlayUntil = millis() + ms;
}

// A number as white LEDs (scene number, or a palette / animation number in the
// menu). Past 17 all 17 LEDs light up; the white ones count the numbers beyond
// 17 and the rest are teal (20 = 3 white + 14 teal).
void showCount(uint8_t n, uint32_t ms, bool fav) {
    uint8_t g = fav ? 70 : 60, r = fav ? 90 : 60, b = fav ? 0 : 60;   
    if (n <= NUM_TENTACLE_LEDS) {
        showOverlay(n, g, r, b, ms);
    } else {
        showOverlay(n - NUM_TENTACLE_LEDS, g, r, b, ms);
        overlay2Count = NUM_TENTACLE_LEDS;
        overlay2G = 50; overlay2R = 0; overlay2B = 40;
    }
}

// ---------------------------------------------------------------------------
//  Smooth colour (used by the slow effects)
// ---------------------------------------------------------------------------

uint8_t blend8(uint8_t a, uint8_t b, uint8_t f) {          // a -> b as f goes 0 -> 255
    return (b >= a) ? a + (((uint16_t)(b - a) * f) >> 8)
                    : a - (((uint16_t)(a - b) * f) >> 8);
}

// t is in 8.8 fixed point: every 256 steps = one palette entry. The color
// between two entries is a smooth mix of them, so nothing ever snaps.
void smoothColor(LedMode mode, int16_t t) {
    uint8_t n = paletteSize(mode);
    uint16_t u = (uint16_t)(t + 10240) % ((uint16_t)n * 256);   // +10240 (a multiple of every span) keeps it positive
    uint8_t idx = (uint8_t)(u >> 8);
    uint8_t f = (uint8_t)u;
    const uint8_t* c0 = getModeColor(mode, idx);
    const uint8_t* c1 = getModeColor(mode, (idx + 1 == n) ? 0 : idx + 1);
    for (uint8_t k = 0; k < 3; k++) col[k] = blend8(c0[k], c1[k], f);
}

// Where in the palette a slow effect is, for the LED at chain position 'pos'.
int16_t smoothT(EffectMode fx, uint8_t pos) {
    switch (fx) {
        case FX_CROSSFADE: return slow + pos * 10;          // nearly the same colour everywhere
        case FX_CURRENTS:  return slow - pos * 70;          // a gradient that flows along the chain
        case FX_FOG:       return (slow >> 1) + pos * 30;   // even slower, gentle gradient
        case FX_RIPPLE: {                                   // color changes with distance from the middle
            uint8_t d = absd(pos, 8);
            return (slow >> 1) + d * 40;
        }
        case FX_SURF:      return (slow >> 1) - pos * 30;
        default:           return (slow >> 1) + pos * 20;   // FX_GAZE
    }
}

// ---------------------------------------------------------------------------
//  Animations
// ---------------------------------------------------------------------------

// Advance everything that changes from frame to frame.
void stepAnimation() {
    uint8_t inc = SPEED_INC[speedLevel];
    // Animation speed in sixteenths of normal: SLOWER scenes drift between about 0.5x and 0.95x,
    // and SURGE scenes additionally swell and ebb (0.5x to about 1.4x) every 10 s or so.
    uint8_t rate = 16;
    if (liveLayer & LAYER_SLOWER) rate = 8 + (tri8((uint8_t)(slow >> 1)) >> 5);
    if (liveLayer & LAYER_SURGE) {
        rate = (uint8_t)(rate * (5 + (tri8((uint8_t)slow) >> 5))) >> 3;
        if (rate < 8) rate = 8;      // never below half speed: slower would make the motion stall and stutter
    }
    surgeAcc += rate * inc;
    uint8_t spd = surgeAcc >> 4;
    surgeAcc &= 15;
    phase += spd;
    slow += inc;
    if (slow >= 10240) slow -= 10240;
    stepAcc += spd;
    if (stepAcc >= 10) {                       // normal speed: every 5 frames = 200 ms
        stepAcc -= 10;
        if (++ledIndex >= NUM_TENTACLE_LEDS) ledIndex = 0;
    }

    // Storm: random lightning. Odd counts show white, so one strike flickers.
    bool rare = !timeLeft(eggUntil);                              // every lightning scene: one strike every 2-4 minutes
    if (stormFlash > 0) {
        stormFlash--;
    } else if (rare && stormGap) {
        stormGap--;
    } else if (rare || rand8() < 6) {                  // (the egg: a strike every ~1.5 s)
        stormFlash = 3 + (rand8() & 3) * 2;
        if (rare) stormGap = 3000 + rand8() * 12;        // scenes: one strike every 2-4 minutes
    }

    // Stars: fade every LED, light a random one now and then.
    if (rand8() < 40) sparkle[scale8(rand8(), NUM_SLOTS - 1)] = 255;

    // Embers: every LED drifts smoothly towards a random brightness.
    for (uint8_t s = 0; s < NUM_SLOTS; s++) {
        if (sparkle[s]) sparkle[s] -= (sparkle[s] >> 3) + 1;
        if (rand8() < 50) emberTarget[s] = 60 + scale8(rand8(), 195);
        if (emberTarget[s] > emberLevel[s]) emberLevel[s] += (emberTarget[s] - emberLevel[s] + 3) >> 2;
        else                                emberLevel[s] -= (emberLevel[s] - emberTarget[s] + 3) >> 2;
    }

    // Blink: every 2-6 seconds everything with the blink animation goes dark briefly.
    if (blinkFrames > 0) {
        blinkFrames--;
    } else if (blinkCountdown == 0) {
        blinkFrames = 4;                       // about 160 ms
        blinkCountdown = 50 + rand8();
    } else {
        blinkCountdown--;
    }
}

// A soft ring of light: age < 0 = approaching, 0.. = just passed (bright, fading).
uint8_t ringLevel(int16_t age) {
    if (age >= -32 && age < 0)  return 15 + (age + 32) * 7;          // soft leading edge
    if (age >= 0 && age < 96)   return 255 - ((age * 5) >> 1);       // glowing tail
    return 15;
}

uint8_t wrapN(uint8_t x, uint8_t n) {      // x mod n without a divide (x is always small)
    while (x >= n) x -= n;
    return x;
}

uint8_t lvl(uint8_t base, uint8_t amp, uint8_t v) {      // base + v scaled to 0..amp
    return base + (((uint16_t)v * amp) >> 8);
}

// Brightness (0-255) that an animation gives to one LED.
// pos = place along the whole chain (eyes 0-1, tentacles 0-16), slot = unique LED number.
// lag = phase offset so the two eyes do not move in lock-step (0 for tentacles).
uint8_t effectLevel(EffectMode fx, uint8_t pos, uint8_t slot, uint8_t lag) {
    switch (fx) {
        case FX_SLUMBER:
            return lvl(20, 235, tri8((uint8_t)(phase - lag)));
        case FX_RISING_TIDE:
            return lvl(15, 240, tri8((uint8_t)((phase << 1) - pos * 20 - lag)));
        case FX_STORM:
            return (stormFlash & 1) ? 255 : 40;
        case FX_STARS:
            return lvl(10, 245, sparkle[slot]);
        case FX_WATCHFUL_BLINK:
            return blinkFrames > 0 ? 0 : 255;
        case FX_HEARTBEAT: {                      // "lub-dub", then a rest
            uint8_t u = (uint8_t)((phase >> 1) % 40);
            uint8_t d1 = absd(u, 4);
            uint8_t d2 = absd(u, 12);
            uint8_t v = 25;
            if (d1 < 6) v = 255 - d1 * 38;
            if (d2 < 6 && (uint8_t)(200 - d2 * 30) > v) v = 200 - d2 * 30;
            return v;
        }
        case FX_COMET:
        case FX_TWIN_COMETS: {                    // a comet round the 17 LEDs (twin: a second one runs the other way)
            uint8_t p = wrapN(pos, NUM_TENTACLE_LEDS);
            uint8_t d = wrapN(ledIndex + NUM_TENTACLE_LEDS - p, NUM_TENTACLE_LEDS);
            if (fx == FX_TWIN_COMETS) {
                uint8_t d2 = wrapN(p + ledIndex, NUM_TENTACLE_LEDS);
                if (d2 < d) d = d2;
            }
            return (d < 6) ? (uint8_t)(255 - d * 42) : 12;
        }
        case FX_EMBER:
            return emberLevel[slot];
        case FX_AURORA: {                         // two waves moving in opposite directions
            uint8_t a = tri8((uint8_t)(phase + pos * 30));
            uint8_t b = tri8((uint8_t)(pos * 47 - phase - (phase >> 1)));
            return lvl(15, 240, (a + b) >> 1);
        }
        case FX_SCANNER: {                        // bright dot bouncing from end to end
            uint8_t t = (phase >> 2) % 32;
            uint8_t head = (t < 16) ? t : 31 - t;         // 0..15 and back
            head += head >> 3;                            // stretch to 0..16
            uint8_t d = absd(pos, head);
            return (d == 0) ? 255 : (d == 1) ? 110 : (d == 2) ? 40 : 8;
        }
        case FX_FAIRY:                            // alternate LEDs swap on and off
            return (((slot + (phase >> 5)) & 1) ? 255 : 25);
        case FX_SWELL:                            // very slow, very wide wave
            return lvl(20, 235, tri8((uint8_t)((phase >> 1) - pos * 9 - lag)));
        case FX_RIPPLE:
        case FX_BLOOM: {                          // soft ring leaving the middle (RIPPLE: LED 9 of 17, BLOOM: the middle of the sigil)
            uint8_t c = (fx == FX_BLOOM) ? 4 : 8;
            uint8_t d = absd(pos, c) * 16;       // distance from the middle, 1/16 LED units
            return ringLevel((int16_t)(uint8_t)((phase >> 1) + 288 - d) - 32);   // wraps cleanly
        }
        case FX_SURF: {                           // sudden bright wave front, slowly draining behind it
            uint8_t q = 255 - (uint8_t)((phase << 1) - pos * 16 - lag);
            uint8_t v = ((uint16_t)q * q) >> 8;
            return (v < 12) ? 12 : v;
        }
        case FX_GLITCH: {
            uint8_t rv = rand8();
            return (rv < 20) ? 255 : (rv < 45) ? 90 : 25;
        }
        case FX_SPIN: {                           // bright head circling the 9 sigil LEDs, tail behind it
            uint8_t d = wrapN((uint8_t)((phase >> 3) % 9) + 9 - wrapN(pos, 9), 9);
            return (d < 4) ? (uint8_t)(255 - d * 60) : 12;
        }
        case FX_DIM:                              // dim glow that slowly breathes
            return lvl(18, 36, tri8((uint8_t)((phase >> 2) + pos * 12 - lag)));
        case FX_CURRENTS:                         // gentle shimmer on top of the flowing colours
            return lvl(190, 65, tri8((uint8_t)((phase >> 1) - pos * 14)));
        case FX_FOG: {                            // two very slow waves (about 20 s each)
            uint8_t a = tri8((uint8_t)((phase >> 2) + pos * 17 + lag));
            uint8_t b = tri8((uint8_t)(pos * 41 - (phase >> 3)));
            return lvl(10, 150, (a + b) >> 1);
        }
        case FX_GAZE: {                           // open (2.5 s) - hold (4 s) - close (4 s) - dark (1.5 s)
            uint16_t p = phase - lag - pos * 12;
            uint8_t c = (p >> 3) % 64;
            if (c < 16) return 8 + c * 15;
            if (c < 36) return 255;
            if (c < 56) return 8 + (55 - c) * 12;
            return 8;
        }
        default:                                  // FX_DRIFT and FX_CROSSFADE: full brightness
            return 255;
    }
}

// Colours are first worked out into these buffers (interrupts ON, so the
// millis() timer keeps ticking), then clocked out in one quick burst with
// interrupts OFF. Keeping that burst short stops millis() losing ticks.
uint8_t prevBuf[NUM_SLOTS * 3];        // last frame shown, for the gliding fades
uint8_t frameBuf[NUM_SLOTS * 3];       // the 17 tentacle pixels first, then the 2 eyes (G, R, B each)
uint8_t fillPos = 0;
#define EYE_PIXELS (frameBuf + NUM_TENTACLE_LEDS * 3)

void pushPixel(uint8_t g, uint8_t r, uint8_t b) {
    if (fillPos < sizeof(frameBuf)) { frameBuf[fillPos++] = g; frameBuf[fillPos++] = r; frameBuf[fillPos++] = b; }
}

// Queue a colour after applying the brightness settings.
void sendOut(uint8_t g, uint8_t r, uint8_t b, bool isEye) {
    uint8_t br = BRIGHTNESS_TABLE[brightnessLevel];
    if (isEye) br = scale8(br, BRIGHTNESS_TABLE[eyeBrightnessLevel]);
    pushPixel(scale8(g, br), scale8(r, br), scale8(b, br));
}

// Compute and send one LED of a normal block.
// The ambient layers (fade wave, twinkles) applied to the brightness m of one LED.
uint8_t layerLevel(uint8_t m, uint8_t pos, uint8_t slot) {
    if (liveLayer & LAYER_FADE)                           // a slow wave of dimming rolls through the LEDs
        m = scale8(m, lvl(70, 185, tri8((uint8_t)((phase >> 2) - pos * 10))));
    if ((liveLayer & LAYER_TWINKLE) && sparkle[slot] > m) m = sparkle[slot];   // random flares
    return m;
}

void sendLed(LedBlock block, uint8_t local, uint8_t pos, uint8_t slot, uint8_t lag) {
    EffectMode fx = blockEffect[block];
    // Menu, palette page of this block: show the palette itself, steady, one colour per LED.
    bool swatch = (uiState == UI_MENU) && (menuPage < PAGE_BRIGHTNESS) && !(menuPage & 1) && (block == menuPage / 2);
    if (swatch && block != BLOCK_EYES) {
        const uint8_t* c = getModeColor(blockMode[block], local);
        col[0] = c[0]; col[1] = c[1]; col[2] = c[2];
    }
    else if (fx >= FIRST_SMOOTH_FX && fx <= LAST_SMOOTH_FX && !swatch) smoothColor(blockMode[block], smoothT(fx, pos));
    else                       blockPaletteColor(block, local, ledIndex);
    uint8_t m = swatch ? 255 : effectLevel(fx, pos, slot, lag);
    if (!swatch) m = layerLevel(m, pos, slot);           // palette preview stays steady
    if (fx == FX_STORM && (stormFlash & 1)) col[0] = col[1] = col[2] = 255;   // lightning is white
    uint8_t sh = blockIsDimmed(block) ? MENU_DIM_SHIFT : 0;
    for (uint8_t k = 0; k < 3; k++) col[k] = scale8(col[k], m) >> sh;
    sendOut(col[0], col[1], col[2], block == BLOCK_EYES);
}

// How the badge is drawn: normal scenes, the "Cthulhu Awakens" scene, or the
// hidden easter egg (Awakens plus lightning).
#define LOOK_NORMAL 0
#define LOOK_AWAKEN 1
#define LOOK_AWAKEN_STORM 2
#define LOOK_MORSE 3
#define LOOK_HIDDEN 4      // the "Yellow Sign" scene
#define YELLOW_BURST (slow < 16)            // its strobe: once per 10240 steps (every ~3.5 min at normal speed)

void sendTeal(uint8_t m) {                 // the teal of the Awakens look at brightness m
    sendOut(scale8(160, m), scale8(10, m), scale8(90, m), false);
}

void showEyes(uint8_t look) {
    for (uint8_t i = 0; i < NUM_EYE_LEDS; i++) {
        if (look == LOOK_MORSE) {    // eyes ARE the Morse lamp
            if (morseOn) sendOut(255, 0, 16, true);
            else         sendOut(0, 0, 0, true);
        } else if (look == LOOK_HIDDEN) {   // yellow eyes taking turns
            uint8_t m = tri8((uint8_t)((phase << 1) + i * 128));
            sendOut(scale8(200, m), scale8(255, m), 0, true);
        } else if (look != LOOK_NORMAL) {   // eyes pulse bright Cthulhu-green
            uint8_t p = layerLevel(tri8((uint8_t)(phase << 2)), i * 6, i);
            if (look == LOOK_AWAKEN_STORM && (stormFlash & 1)) sendOut(255, 255, 255, true);
            else sendOut(p, 0, p >> 4, true);
        } else {
            sendLed(BLOCK_EYES, i, i * 6, i, i * 96);   // eyes are offset from each other
        }
    }
}

// The tentacle LEDs are one chain, so both blocks are sent in order 1..17.
void showTentacles(uint8_t look) {
    bool overlay = timeLeft(overlayUntil);
    for (uint8_t i = 0; i < NUM_TENTACLE_LEDS; i++) {
        if (overlay && i < overlayCount) {            // menu / scene indicator
            if ((i % 5) == 4) pushPixel(0, 90, 60);   // every 5th LED is magenta
            else              pushPixel(overlayG, overlayR, overlayB);
        } else if (overlay && i < overlay2Count) {
            pushPixel(overlay2G, overlay2R, overlay2B);
        } else if (look == LOOK_MORSE) {              // tentacles glow softly with the eyes
            sendTeal(morseOn ? 70 : 6);
        } else if (look == LOOK_HIDDEN) {             // purple <-> yellow wave winding through the tentacles
            uint8_t w = tri8((uint8_t)((phase << 2) - i * 24));
            if (YELLOW_BURST) w = ((slow >> 1) & 1) ? 255 : 0;       // every ~3.5 min: purple / yellow strobe
            sendOut(blend8(20, 220, w), blend8(110, 255, w), blend8(200, 0, w), false);
        } else if (look != LOOK_NORMAL) {             // teal wave (lightning only in the egg)
            if (look == LOOK_AWAKEN_STORM && (stormFlash & 1)) {
                sendOut(255, 255, 255, false);
            } else {
                uint8_t m = layerLevel(tri8((uint8_t)((phase << 2) - i * 20)), i, NUM_EYE_LEDS + i);
                if (look == LOOK_AWAKEN_STORM) m = scale8(m, 110);   // dark stormy sky, so the lightning stands out
                sendTeal(m);
            }
        } else if (i < NUM_TENTACLE_LEDS_ONE_TO_NINE) {
            sendLed(BLOCK_TENTACLES_1_TO_9, i, i, NUM_EYE_LEDS + i, 0);
        } else {
            sendLed(BLOCK_TENTACLES_10_TO_17, i - NUM_TENTACLE_LEDS_ONE_TO_NINE, i, NUM_EYE_LEDS + i, 0);
        }
    }
}

// Scale the whole frame down if the LEDs would draw more than MAX_LED_CURRENT_MA.
uint16_t sumBuf(const uint8_t* b, uint8_t n) {
    uint16_t t = 0;
    while (n--) t += *b++;
    return t;
}

void scaleBuf(uint8_t* b, uint8_t n, uint8_t f) {
    while (n--) { *b = scale8(*b, f); b++; }
}

void limitCurrent() {
    uint16_t total = sumBuf(frameBuf, sizeof(frameBuf));   // at most 14535
    uint16_t mA = total * 4 / 51;                  // 20 mA per colour channel at 255
    if (mA <= MAX_LED_CURRENT_MA) return;
    uint8_t f = (uint8_t)((MAX_LED_CURRENT_MA * 128u) / mA) * 2;   // 16-bit maths only
    scaleBuf(frameBuf, sizeof(frameBuf), f);
}

void renderFrame() {
    uint8_t look = LOOK_NORMAL;
    if (morseActive) look = LOOK_MORSE;
    else if (currentScene == SCENE_YELLOW) look = LOOK_HIDDEN;
    else if (timeLeft(eggUntil)) look = LOOK_AWAKEN_STORM;
    else if (currentScene == SCENE_AWAKENS) look = LOOK_AWAKEN;
    fillPos = 0;
    showTentacles(look);     // work out all colours (interrupts stay on)
    showEyes(look);
    if (blankOut) {                                  // dark during a shortcut hold
        scaleBuf(frameBuf, sizeof(frameBuf), 0);
    }
    // Glide: every LED moves half way to its new brightness each frame, so steps and
    // jumps melt into smooth fades. Skipped for lightning, menus and number displays.
    bool glide = !blankOut && uiState == UI_RUN && !timeLeft(overlayUntil) && look <= LOOK_AWAKEN && blockEffect[BLOCK_TENTACLES_1_TO_9] != FX_STORM;
    for (uint8_t i = 0; i < sizeof(frameBuf); i++) {
        uint8_t c = frameBuf[i], p = prevBuf[i];
        if (glide) c = (p + c + (c > p)) >> 1;
        frameBuf[i] = prevBuf[i] = c;
    }
    limitCurrent();

    // Interrupts are off only while a strip is clocked out (about 0.5 ms), so
    // the millis() tick cannot stretch an LED bit - and cannot be lost either.
    noInterrupts();
    for (uint8_t i = 0; i < NUM_TENTACLE_LEDS; i++)
        sendColor(frameBuf[i * 3], frameBuf[i * 3 + 1], frameBuf[i * 3 + 2], TENTACLE_LED_ARRAY);
    interrupts();
    _delay_us(500); // latch

    noInterrupts();
    for (uint8_t i = 0; i < NUM_EYE_LEDS; i++)
        sendColor(EYE_PIXELS[i * 3], EYE_PIXELS[i * 3 + 1], EYE_PIXELS[i * 3 + 2], EYE_LED_ARRAY);
    interrupts();
    _delay_us(500); // latch
}

void fillAll(uint8_t g, uint8_t r, uint8_t b) {      // every LED one colour (0,0,0 = all off)
    colorWipe(g, r, b, EYE_LED_ARRAY, NUM_EYE_LEDS);
    colorWipe(g, r, b, TENTACLE_LED_ARRAY, NUM_TENTACLE_LEDS);
}

// Quick blink of every LED in one colour - confirms entering/leaving the menu etc.
void blinkAll(uint8_t g, uint8_t r, uint8_t b, uint8_t times) {
    for (uint8_t t = 0; t < times; t++) {
        fillAll(g, r, b);
        _delay_ms(120);
        fillAll(0, 0, 0);
        _delay_ms(120);
    }
}

void blinkOnOff(bool on, uint8_t times) { blinkAll(on ? 80 : 0, on ? 0 : 80, 0, times); }   // green = on, red = off

// ---------------------------------------------------------------------------
//  Setup / minibadge clock
// ---------------------------------------------------------------------------

// Hidden Morse message. One element (dot or dash) per step; times are scheduled
// from the previous step so the rhythm never drifts.
void startMorse() {
    morseActive = true;
    morseOn = false;
    morseIdx = 0;
    morseNextAt = millis() + 600;          // short pause, then it starts
}

void stopMorse() {
    morseActive = false;
    morseOn = false;
    lastAutoCycle = millis();
}

void updateMorse() {
    if (!morseActive || timeLeft(morseNextAt)) return;
    if (morseOn) {                         // element finished: 1 unit of dark
        morseOn = false;
        uint32_t gap = MORSE_UNIT_MS;
        char nx = MORSE_MSG[morseIdx];
        if (nx == ' ')      { gap += 2UL * MORSE_UNIT_MS; morseIdx++; }   // letter gap = 3 units
        else if (nx == '/') { gap += 6UL * MORSE_UNIT_MS; morseIdx++; }   // word gap = 7 units
        morseNextAt += gap;
    } else {                               // start the next dot or dash
        char c = MORSE_MSG[morseIdx++];
        if (c == 0) { stopMorse(); return; }
        morseOn = true;
        morseNextAt += (c == '-') ? 3UL * MORSE_UNIT_MS : 1UL * MORSE_UNIT_MS;
    }
}

bool isButtonPressed() {
    return (PORTA.IN & (1 << BUTTON_PIN)) == 0;
}

// Minibadge clock: 1 second with H high / L low, then 1 second with H low / L high.
bool clockPhase = false;

void setMinibadgeClock(bool phaseHigh) {
    if (phaseHigh) {
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_H_PIN);
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_L_PIN);
    } else {
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_H_PIN);
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_L_PIN);
    }
}

void updateMinibadgeClock() {
    clockPhase = !clockPhase;      // our own state - no need to read the pin back
    setMinibadgeClock(clockPhase);
}

void setup() {
    // Set eye LED array and tentacle LED array as output
    PORTA.DIRSET = (1 << EYE_LED_ARRAY) | (1 << TENTACLE_LED_ARRAY);

    // Set button pin as input with pull-up resistor
    PORTA.DIRCLR = (1 << BUTTON_PIN);
    PORTA.OUTSET = (1 << BUTTON_PIN);

    // Set minibadge clock pins as output
    PORTA.DIRSET = (1 << MINIBADGE_CLK_CTRL_H_PIN) | (1 << MINIBADGE_CLK_CTRL_L_PIN);

    setMinibadgeClock(clockPhase);   // start with a defined clock state

    // Ensure all LEDs are off at the start
    fillAll(0, 0, 0);

    // Power-up hold: 8 s (eyes turn red) then let go = reset.
    bool didReset = false;
    if (isButtonPressed()) {
        uint16_t held = 0;
        while (isButtonPressed() && held < RESET_HOLD_MS) {
            _delay_ms(10);
            held += 10;
        }
        if (held >= RESET_HOLD_MS) {
            colorWipe(0, 80, 0, EYE_LED_ARRAY, NUM_EYE_LEDS);                                // red eyes
            while (isButtonPressed()) _delay_ms(10);                                         // wait for let-go
            resetToDefaults();
            blinkAll(80, 0, 0, 3);                                                           // green blinks = reset done
            didReset = true;
        }
    }
    if (!didReset) loadSettings();
    _delay_ms(100);
}

// ---------------------------------------------------------------------------
//  Button handling (non-blocking, short press vs long press)
// ---------------------------------------------------------------------------

void showScene() {
    showCount(currentScene + 1, SCENE_OVERLAY_MS, isFavorite(currentScene));
}

void showPageNumber() {
    if (menuSettings) showOverlay(menuPage + 1 - PAGE_BRIGHTNESS, 60, 100, 0, PAGE_OVERLAY_MS);   // orange row = settings page
    else              showOverlay(menuPage + 1, 0, 0, 100, PAGE_OVERLAY_MS);                       // blue row = theme page
}

void showValue(uint8_t n) {
    showCount(n, VALUE_OVERLAY_MS, false);                    // white row = chosen value
}

void closeMenu() {
    uiState = UI_RUN;
    overlayUntil = 0;
    lastAutoCycle = millis();   // auto-cycle waits a full interval after the menu
    saveSettings();
}

// Change the value shown on the current menu page.
void menuShortPress() {
    lastMenuActivity = millis();
    if (menuPage < PAGE_BRIGHTNESS) {
        overlayUntil = 0;                      // show the change at once (the page number row goes away)
        // Editing: copy the scene you are looking at into MY CUSTOM (the last scene) first.
        if (currentScene != SCENE_CUSTOM) {
            if (currentScene >= SCENE_AWAKENS) applyScene(0);   // Awakens and the Yellow Sign have no palette to edit
            for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
                customMode[i] = blockMode[i];
                customEffect[i] = blockEffect[i];
            }
            customLayer = liveLayer;
            currentScene = SCENE_CUSTOM;
        }
        LedBlock blk = (LedBlock)(menuPage / 2);
        if ((menuPage & 1) == 0) {
            blockMode[blk] = nextMode(blockMode[blk]);
            customMode[blk] = blockMode[blk];
        } else {
            blockEffect[blk] = nextEffect(blockEffect[blk]);
            customEffect[blk] = blockEffect[blk];
            if (blockEffect[blk] == FX_STORM) {          // picking lightning: one strike now, so you can see it
                stormFlash = 7;
                stormGap = 3000 + rand8() * 12;
            }
        }
    } else {                                          // brightness, eye brightness, speed
        uint8_t* p = &cfg[8 + menuPage - PAGE_BRIGHTNESS];   // their places in the saved settings
        if (++*p >= ((menuPage == PAGE_SPEED) ? NUM_SPEED_LEVELS : NUM_BRIGHTNESS_LEVELS)) *p = 0;
        showValue(*p + 1);
    }
}

// Shortcuts (run mode): mark / unmark the scene you are on as a favourite,
// and start / stop auto-cycling. Both save straight away.
void toggleFavorite() {
    bool fav = flipFavorite();
    saveSettings();
    blinkOnOff(fav, 1);                  // green blink = now a favourite, red = no longer
    showScene();                                              // number shows yellow for a favourite
}

// 3 taps + hold steps through: off -> 4 s -> 8 s -> 20 s -> 60 s -> off.
// Green blinks = the step length (1 = 4 s ... 4 = 60 s), 2 red blinks = off.
void toggleCycling() {
    uint8_t st = tapState;                  // the first tap of a running cycle already stopped it
    st = (st >= NUM_CYCLE_LEVELS) ? 0 : st + 1;
    autoCycle = (st != 0);
    if (st) cycleLevel = st - 1;
    lastAutoCycle = millis();
    saveSettings();
    blinkOnOff(autoCycle, autoCycle ? st : 2);
}

void onShortPress() {
    if (morseActive) { stopMorse(); return; }          // any press stops the Morse message
    if (uiState == UI_MENU) {
        menuShortPress();
        return;
    }
    uint32_t now = millis();
    tapRun = (tapRun && (now - lastShortAt) < TAP_GAP_MS) ? tapRun + 1 : 1;   // for the tap-tap-hold shortcuts
    if (tapRun == 1) { tapMoves = 0; tapState = autoCycle ? cycleLevel + 1 : 0; }
    lastShortAt = now;
    if (autoCycle && !timeLeft(eggUntil)) {            // cycling: one press STAYS on this scene
        autoCycle = false;
        eeprom_update_byte((uint8_t*)(EEPROM_ADDR + 11), 0);
        saveScene();
        showScene();
        eggPresses = 0;
        return;
    }
    // Run mode: next scene
    tapMoves++;
    applyScene(sceneStep(currentScene, 1));
    showScene();                                     // white row = scene number (yellow = favourite)
    saveScene();

    // Easter egg: EGG_PRESSES quick presses
    if (eggPresses == 0 || (now - eggWindowStart) > EGG_WINDOW_MS) {
        eggPresses = 0;
        eggWindowStart = now;
    }
    eggPresses++;
    if (eggPresses >= EGG_PRESSES) {
        eggPresses = 0;
        applyScene(sceneStep(currentScene, -EGG_PRESSES));  // undo the scene steps
        overlayUntil = 0;
        saveScene();
        eggUntil = now + EGG_DURATION_MS;
    }
}

void onLongPress(uint32_t pressStart) {
    blankOut = false;
    if (morseActive) { stopMorse(); return; }
    // Tap-and-hold shortcuts: 1 tap + hold = settings menu, 2 = favourite, 3 = start/stop
    // cycling, 4 = Morse message. The taps moved the scene on, so that is undone first.
    uint8_t taps = (tapRun && (pressStart - lastShortAt) < TAP_GAP_MS) ? tapRun : 0;
    tapRun = 0;
    eggPresses = 0;
    if (uiState == UI_RUN && taps >= 1 && taps <= 4) {
        applyScene(sceneStep(currentScene, -tapMoves));   // undo the scene steps the taps made
        overlayUntil = 0;
        saveScene();
        if (taps == 2)      toggleFavorite();
        else if (taps == 3) toggleCycling();
        else if (taps == 4) startMorse();
        if (taps != 1) return;
    }
    if (uiState == UI_RUN) {
        uiState = UI_MENU;
        menuSettings = (taps == 1);
        menuPage = menuSettings ? PAGE_BRIGHTNESS : 0;
        lastMenuActivity = millis();
        blinkAll(80, 80, 80, 2);
        showPageNumber();
    } else {
        lastMenuActivity = millis();
        menuPage++;
        if (menuPage >= (menuSettings ? NUM_PAGES : PAGE_BRIGHTNESS)) {      // left the last page: close and save
            closeMenu();
            blinkAll(80, 80, 80, 1);
        } else {
            showPageNumber();
        }
    }
}

void handleButton() {
    static bool wasPressed = false;
    static bool longHandled = false;
    static uint32_t pressStart = 0;
    static uint32_t releasedAt = 0;

    bool pressed = isButtonPressed();
    uint32_t now = millis();

    if (pressed && !wasPressed) {
        if ((now - releasedAt) < 40) return;      // contact bounce right after letting go: ignore
        wasPressed = true;
        longHandled = false;
        pressStart = now;
    } else if (pressed && wasPressed) {
        if (tapRun >= 1 && (pressStart - lastShortAt) < TAP_GAP_MS && (now - pressStart) >= 300) blankOut = true;
        if (!longHandled && (now - pressStart) >= LONG_PRESS_MS) {
            longHandled = true;
            onLongPress(pressStart);
        }
    } else if (!pressed && wasPressed) {
        wasPressed = false;
        blankOut = false;
        releasedAt = now;
        if (!longHandled && (now - pressStart) >= DEBOUNCE_MS) {
            onShortPress();
        }
    }

    // Leave the menu (and save) if nobody touches the button for a while
    if (uiState == UI_MENU && (now - lastMenuActivity) >= MENU_TIMEOUT_MS) {
        closeMenu();
    }
}

// ---------------------------------------------------------------------------
//  Main loop
// ---------------------------------------------------------------------------

void loop() {
    static uint32_t lastClockUpdate = 0;
    const uint32_t clockUpdateInterval = 1000; // 1 second per half-cycle
    static uint32_t lastFrameTime = 0;

    uint32_t now = millis();

    // Minibadge clock first, so nothing below can delay it.
    // The schedule advances by exactly 1000 ms each time (no drift).
    if ((now - lastClockUpdate) >= clockUpdateInterval) {
        lastClockUpdate += clockUpdateInterval;
        if ((now - lastClockUpdate) >= clockUpdateInterval) lastClockUpdate = now;  // fell far behind (menu blink): resync
        updateMinibadgeClock();
    }

    handleButton();
    now = millis();

    // Auto-cycle: step through the scenes. Paused in the menu and during the egg.
    if (autoCycle && uiState == UI_RUN && !timeLeft(eggUntil) && !morseActive &&
        (now - lastAutoCycle) >= AUTO_CYCLE_TABLE[cycleLevel]) {
        lastAutoCycle = now;
        applyScene(nextAutoScene());                   // not saved, so it never wears the EEPROM
    }

    updateMorse();

    // Draw one animation frame
    if ((now - lastFrameTime) >= FRAME_MS) {
        lastFrameTime = now;
        stepAnimation();
        renderFrame();
    }
}


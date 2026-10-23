# Cthulu Minibadge Holder

*"In his house at R'lyeh, dead Cthulhu waits dreaming."*

<img src="/images/cthulu_badge_leds_multi_1.png" width="500" />


## Assembly Instructions

You should have the following items:

- Badge PCB
- (12) 8 pin headers
- (6) Minibadge kits
- 3D printed monolith badge stand
- Sticker
- Instruction book

The Cthulu Minibadge Holder is mostly assembled. The only required assembly is to solder the 12 minibadge headers to the front. Be careful while soldering these that you don't damage any of the components on the back.

Power on by connecting to USB and verifying that the switch is in the lower position. Try to use a low profile USB connector so that the minibadge holder sits level in the monolith stand.

## Operating Instructions

The minibadge holder has three LED groups:

- the eyes (2 LEDs)
- the sigil (9 LEDs)
- the tentacle curls (8 LEDs)

It has one button, and everything is done with taps and holds.

A new or reset badge starts on the Cthulhu Awakens Scene (scenes are listed below). When the badge is powered off all custom settings are saved.

### Button Controls

| Action | What it does |
| --- | --- |
| Short press | Next scene. A row of LEDs shows the scene number, white, or yellow if the scene is a favourite. |
| Hold (0.8 s) | Open the Theme Menu. |
| 1 tap + hold | Open the Settings Menu. |
| 2 taps + hold | Marks or unmarks the current scene as a favorite. Green blink = added, red blink = removed. |
| 3 taps + hold | Start auto-cycling (see below). |
| 4 taps + hold | Play the Morse message on the eyes ("IA IA CTHULHU FHTAGN"). Any press stops it. |
| 5 quick taps | Play The Cthulhu Awakens show with lightning (12 s). |

"Tap + hold" means tap quickly, then hold on the last tap.

### Scene number display

- Scenes 1–17 light that many LEDs.
- Above 17, all 17 LEDs light up. The white or yellow ones count how far past 17 you are, and the rest are teal.
- The menus use the same display for palette and animation numbers.

### Auto-cycling

Auto-cycling switches scenes by itself. There's not a menu for it. You set it with 3 taps + hold: tap quickly three times, then hold the button down at least 0.8s. Each time you do this, the badge moves to the next step:

| Step | Blinks | Switches scene every |
| --- | --- | --- |
| Off | 2 red | never |
| 1 | 1 green | 4 seconds |
| 2 | 2 green | 8 seconds |
| 3 | 3 green | 20 seconds |
| 4 | 4 green | 60 seconds |

After step 4, the next 3 taps + hold turns it off again, and the steps repeat.

- **Which scenes:** it cycles through scenes that have been marked as favorites. If you haven't marked any, it cycles through every scene.
- **To stop it:** one short press stays on the scene you're looking at and stops the cycling. You can also keep stepping with 3 taps + hold until you reach off.
- **To take a scene out of the cycle:** unmark it with 2 taps + hold.
- **Saved:** the step you choose is kept when the badge is powered off.

### LED Scenes

| # | Scenes |
| --- | --- |
| 1–15 | Cthulhu Rises, Sleeping in R'lyeh, The Deep Ones, Abyssal Tide, Shadow over Innsmouth, Call of Cthulhu, Great Old One, Elder Gods, When the Stars Are Right, Eldritch Spectrum, Dagon, Nyarlathotep, Yog-Sothoth, Necronomicon, Starry Wisdom |
| 16–25 (slow) | Dreaming in R'lyeh, The Drowned City, Sunken Fire, Deep Ones' Lullaby, Cosmic Drift, Madness Creeping, The Watcher in the Deep, Elder Sign, Crimson Stare, The Slow Awakening |
| 26–31 (special effects) | Ripples of R'lyeh, Crashing Waves, Whisperer in Darkness, The Twin Cults, Pulse of Elder Things, Cthulhu Stirs |
| 32 | Cthulhu Awakens |
| 33 | The Yellow Sign |
| 34 | CUSTOM (your own theme.. ) |

Every scene also has some extra effects built in:

- **Twinkle:** random LEDs flare like stars.
- **Fade:** a slow wave of dimming and brightening.
- **Surge:** the animation speeds up and slows down every 10 s or so.
- **Slower:** scenes 1–15 and 26–31 run slower than the base speed.
- **Lightning:** scenes 4 and 28, and the 5-tap show. In the two scenes it strikes when you pick the scene, then once every 2–4 minutes. In the 5-tap show it strikes often.

### Menus

- Short press changes the value on the current page.
- Hold goes to the next page. After the last page the menu closes and your choices are saved.
- If you press nothing for 15 seconds, the menu closes and saves.

### Scene Menu (Hold 0.8 sec)

This menu changes CUSTOM only. On entry, a row of blue LEDs shows the page number (every 5th LED is magenta to help you count). The block (ex: Eye LEDs) you're editing stays bright and the others dim.

When you enter the CUSTOM menu, the current scene is copied into the CUSTOM slot. You can then edit it directly and it will automatically save.

Use a short press to move through the menu options. For instance, if on Page 1 (eyes palette) you'll see the eye LED colors change as you cycle through them via short button presses. To move to the next page; hold the button (0.8 seconds). All pages are listed below. There is a page for each LED block color and animation / effects. So you'll edit each one individually as you step through the pages.

| Page | Edits |
| --- | --- |
| 1 | Eyes palette |
| 2 | Eyes animation |
| 3 | Sigil palette |
| 4 | Sigil animation |
| 5 | Tentacle Curls palette |
| 6 | Tentacle Curls animation |

- Palette pages show the palette as a steady swatch, one colour per LED.
- Animation pages play the animation live.
- Choosing Storm (lightning) fires one strike straight away so you can see it took. In normal running it still strikes only every few minutes.

#### Palette options (press to step through; wraps back to 1)

1. Great Old One: reds, olive-greens, yellow
2. Call of Cthulhu: reds, bright to deep, yellow
3. Shadow over Innsmouth: sky blue to deep blue, red
4. The Deep Ones: greens, light to dark, yellow
5. Abyssal Tide: pale aqua, blues, white
6. Elder Gods: reds, bright to deep, dark red-brown
7. When the Stars Are Right: oranges, white
8. Eldritch Spectrum: red, yellow, orange, teal
9. Cthulhu Rises: red, green, blue, orange, teal
10. Sleeping in R'lyeh: blues, light to deep, white
11. Cthulhu Awakened: teal, Cthulhu green, sea green, deep teal, pale aqua

#### Animation options (press to step through; wraps back to 1)

1. Drift: the original scrolling palette
2. Slumber: slow breathing
3. Rising tide: a wave of brightness rolls down the tentacles
4. Storm: dim glow with lightning
5. Stars: random twinkling
6. Watchful blink: normal glow that blinks off now and then
7. Heartbeat: slow double "lub-dub" pulse
8. Comet: a bright comet with a fading tail
9. Ember: smooth random flicker, like embers
10. Aurora: two slow waves of light drifting through each other
11. Scanner: a bright dot sweeps back and forth
12. Fairy: every other LED swaps on and off
13. Swell: one huge slow ocean swell
14. Crossfade: slowly melts from one palette colour to the next
15. Currents: a smooth gradient of palette colours flows along
16. Fog: dim, slow, overlapping waves, like mist
17. Gaze: slowly opens, holds, slowly closes (eyes take turns)
18. Ripple: soft rings spread out from the middle of the tentacles
19. Surf: waves crash in with a bright edge and drain away
20. Twin comets: two comets run in opposite directions and pass through each other
21. Glitch: dim static with random bright flashes
22. Dim: a dim, slowly breathing glow
23. Bloom: soft rings spread out from the middle of the sigil
24. Spin: a bright segment circles round the 9 sigil LEDs

Animations 14–19 also blend smoothly between the palette colours.

### Settings Menu (1 tap + hold)

These settings apply to all scenes, not just CUSTOM. On entry, a row of orange LEDs shows the page number. The value shows as a row of white LEDs, where 1 LED means the first level. As before, short press to cycle through each page option, and long press to go to the next page. For example, if you want to change the speed to fast you would: Long Press (enter Page 1 - brightness), Long Press (enter Page 2 - Eye Brightness), Long Press (enter Page 3 - Speed). Short Press to step from Normal Speed to Fast Speed. Long Press to exit menu or let it time out in 15 seconds.

| Page | Setting | Levels (default in bold) |
| --- | --- | --- |
| 1 | Brightness (all LEDs) | 1 = **brightest**, 2, 3, 4 = dimmest |
| 2 | Eye brightness (relative to the rest) | 1 = **same as the rest**, 2, 3, 4 = dimmest |
| 3 | Speed | 1 = slow, 2 = **normal**, 3 = fast |

### Power-up reset

1. Hold the button while plugging the badge in.
2. After 8 seconds the eyes turn red.
3. Let go. Everything resets to default, including favourites, CUSTOM and the settings, and the badge blinks green 3 times.

If you let go before 8 seconds, the badge just starts normally.

## Programming

The microcontroller is an Attiny814. You can program using the 3-pin sneaky connector (doesn't require soldering).

- Pin 1: Power
- Pin 2: UDPI
- Pin 3: Ground

There are multiple ways to program so if you already have your method then you are good to go. If you have not programmed an ATtiny with UDPI then I recommend following the instructions located here to get started: https://wolles-elektronikkiste.de/en/using-megatinycore.

If you use the method described here with Arduino, you do not need to connect a resistor in series with pin 6 because this is already installed on the board.

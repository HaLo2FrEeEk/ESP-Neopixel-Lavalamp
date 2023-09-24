# ESP-Neopixel-Lavalamp
Simple animation to simulate the effect of a lava lamp on a string (or ring) of WS2812 "Neopixels." 1000% work in progress

I recently refactored some things, made the animation run on a Ticker timer rather than a roll-my-own setup. Seems much more reliable. Also changed up the wifi reconnect loop so that it will give up and restart after 3 tries.

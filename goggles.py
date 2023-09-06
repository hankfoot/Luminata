import board
import busio
import digitalio
import math
import random
import time
import neopixel
from rainbowio import colorwheel
from adafruit_bluefruit_connect.packet import Packet
from adafruit_bluefruit_connect.color_packet import ColorPacket
from adafruit_bluefruit_connect.button_packet import ButtonPacket
pixel_pin = board.D0
num_pixels = 38
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=0.05, auto_write=False)
pixels.fill((0,0,0))
pixels.show()
RXO_PIN = board.D2
uart = busio.UART(board.D4, RXO_PIN, baudrate=9600, timeout=0)

#       COLOR UTILITIES
pattern = 0
mode = "primary"
primaryColor = (255,0,0)
secondaryColor = (0,255,255)
BLACK = (0,0,0)

def dim(dimmer):
    for i in range(num_pixels):
        pixels[i] = [math.floor(pixel * dimmer) for pixel in pixels[i]]

#       COLOR MODES

def infinite(sleep):
    infinite_helper(range(0, 19), sleep)
    infinite_helper(range(37, 18, -1), sleep)

def infinite_helper(inRange, sleep):
    for i in inRange:
        dim(0.8)
        if mode == "primary":
            pixels[i] = primaryColor
        elif mode == "secondary":
            pixels[i] = primaryColor if i < 19 else secondaryColor
        elif mode == "rainbow":
            pixels[i] = colorwheel((i / num_pixels) * 256)
        pixels.show()
        time.sleep(sleep)




def twinkle(sleep):
    spawnDelay = 1

    # Spawn 8 stars
    for i in range(0, 8):
        # Spawn star
        idx = random.randint(0,num_pixels - 1)
        if mode == "primary":
            pixels[idx] = primaryColor
        elif mode == "secondary":
            pixels[idx] = primaryColor if i % 2 == 0 else secondaryColor
        elif mode == "rainbow":
            pixels[idx] = colorwheel(random.randint(0,255))

        # Sleeping
        for j in range(0, 1):
            dim(0.9)
            pixels.show()
            time.sleep(sleep)


def swirl(sleep):
    for i in range(0,19):
        dim(0.8)

        leftIdx = (i+8)%19
        if mode == "primary" or mode == "secondary":
            pixels[leftIdx] = primaryColor
        elif mode == "rainbow":
            pixels[leftIdx] = colorwheel((leftIdx + (num_pixels / 2))/ num_pixels / 2  * 256)

        rightIdx = 37 - i
        if mode == "primary":
            pixels[rightIdx] = primaryColor
        elif mode == "secondary":
            pixels[rightIdx] = secondaryColor
        elif mode == "rainbow":
            pixels[rightIdx] = colorwheel(((rightIdx - 19) / num_pixels / 2) * 256)
        pixels.show()
        time.sleep(sleep)

breath = 0
def breathe(sleep):

    global breath

    steps = 30

#     pixels.fill(BLACK)
#     pixels.show()

    outColor = BLACK

    if mode == "primary":
        outColor = primaryColor
    elif mode == "secondary":
        if (breath % 2) == 0:
            outColor = secondaryColor
        else:
            outColor = primaryColor

        breath = (breath + 1) % 2
    elif mode == "rainbow":
        outColor = colorwheel(random.randint(0,255))

    for i in range(0,steps):

        sin = math.sin((i / steps) * 3.1415926)
        if mode != "rainbow":
            pixels.fill([math.floor(sin * out) for out in outColor])
        else:
            pixels.fill(outColor)
            pixels.fill([math.floor(sin * pixel) for pixel in pixels[0]])
        pixels.show()
        time.sleep(sleep)

    pixels.fill(BLACK)
    pixels.show()

    time.sleep(2)

flasher = 0
def flash(sleep):

    global flasher

    if mode == "primary":
        pixels.fill(primaryColor)
    elif mode == "secondary":
        if (flasher % 2) == 0:
            pixels.fill(primaryColor)
        else:
            pixels.fill(secondaryColor)
        flasher = (flasher + 1) % 2
    elif mode == "rainbow":
        pixels.fill(colorwheel(random.randint(0,255)))

    pixels.show()
    time.sleep(sleep)
    pixels.fill(BLACK)
    pixels.show()
    time.sleep(sleep)

def on(sleep):
    for i in range(num_pixels):

        if mode == "primary":
            pixels[i] = primaryColor
        elif mode == "secondary":
            if i >= num_pixels / 2:
                pixels[i] = secondaryColor
            else:
                pixels[i] = primaryColor
        elif mode == "rainbow":
            pixels[i] = colorwheel((i / num_pixels) * 256)

        time.sleep(sleep)
        pixels.show()

#
#       MAIN LOOP
#

def change_pattern(modifier):
    global pattern
    pattern = (pattern + modifier) % 6

autoCycle = False
def change_mode(name):
    global mode
    global primaryFlag
    mode = name
    primaryFlag = True

primaryFlag = True
def change_color(color):
    global primaryFlag
    global primaryColor
    global secondaryColor

    if mode is not "secondary":
        primaryColor = color
    else:
        if primaryFlag:
            primaryColor = color
        else:
            secondaryColor = color
        primaryFlag = not primaryFlag

timeOfLastAutoSwitch = 0
while True:
    packet = Packet.from_stream(uart)
    # Color input
    if(isinstance(packet, ColorPacket)):
        print(packet.color)
        change_color(packet.color)
    # Mode config
    if(isinstance(packet, ButtonPacket) and packet.pressed):
        print (packet.button)
        if packet.button == ButtonPacket.BUTTON_1:
            change_mode("primary")
        elif packet.button == ButtonPacket.BUTTON_2:
            change_mode("secondary")
        elif packet.button == ButtonPacket.BUTTON_3:
            change_mode("rainbow")
        elif packet.button == ButtonPacket.BUTTON_4:
            autoCycle = not autoCycle
        elif packet.button == ButtonPacket.LEFT:
            change_pattern(-1)
            autoCycle = False
        elif packet.button == ButtonPacket.RIGHT:
            change_pattern(1)
            autoCycle = False
    if pattern == 0:
        on(0.02)
    elif pattern == 1:
        flash(1)
    elif pattern == 2:
        swirl(0)
    elif pattern == 3:
        breathe(0.2)
#     elif pattern == 4:
#         theatre(.5)
    elif pattern == 4:
        twinkle(0)
    elif pattern == 5:
        infinite(.01)

    if autoCycle and ((time.monotonic() - timeOfLastAutoSwitch) >= 16):
        change_pattern(1)
        timeOfLastAutoSwitch = time.monotonic()

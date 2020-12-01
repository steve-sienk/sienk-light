# sienk-light

## (pronounced: "sync" light)

A flashlight that synchronizes to the color of something.

|![Sienk Light Demo](/images/sienk-light-opt.gif)|
|:--:| 
|*[Demo Video](https://drive.google.com/file/d/1o6r3euRBh2TEadCkdSuhpiGL3R8_MlUx/view?usp=sharing)* |
|*[Yellow-Pink-Blue](https://drive.google.com/file/d/11HomMsZ_Elw0OLXgeIn2DAkY_ZugXq_D/view?usp=sharing)* *[Yellow-None-GreenObject](https://drive.google.com/file/d/1jFYFQPeXQzQvFiQWbKSMMF025evTNrl_/view?usp=sharing)*|
| Note: these are hosted on my Google drive, you may have to download to view |


## Overview:
This project uses some basic parts I had lying around:
- An arduino
  - because arduinos are easy to work with and don't consume much power.
- A distance sensor 
  - to determine when something is nearby.
  - (It wasn't strictly necessary, though my brother said "make it scan in real-time!", and so I did. He's right, it's way cooler.)
- A color sensor
  - to find the color.
- An LED ring
  - to show the color.
- A battery
  - because portable is cooler.
- An old flashlight
  - something to put it all in.
- A transistor (you might not need this).
  - I used one because my color sensor wasn't turning on and off as I expected. I thought it was a power issue -- though looking back, I think the real issue I had was two different things mapped to a single output pin on the arduino (some old NeoPixel code, and a color sensor control wire), oops.

### My Parts
- Adafruit [Feather M4 Express](https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51) 
  - Most other Arduino based controllers should work with the code in here, though it seemed to fit well inside my old flashlight. I liked the proto-board on it for soldering things.
- [Adafruit time-of-flight distance sensor](https://learn.adafruit.com/adafruit-vl53l0x-micro-lidar-distance-sensor-breakout)
  - I wanted to try out this distance sensor. It works really well! It was super easy to work with. It sits below the little drilled out hole in the lens of the flashlight. It worked okay through the lens, though I think it would pick up the partial reflection in the lens and show low values (less than ~40mm) when nothing else had a stronger reflection on the other side of the lens. 
- [OSEPP Color Sensor](https://www.osepp.com/electronic-modules/sensor-modules/58-color-sensor-module)
  - I've had this sensor sitting in one of my parts boxes for a long time. I finally had a chance to use it!
- General purpose NPN 2222 transistor
  - It's been a while since my college's electronics lab, and I found [this guide](https://create.arduino.cc/projecthub/105448/simple-npn-transistor-switch-control-with-cooling-fan-499f90) helpful in wiring things up.

## More pictures!
| !  |  ! | ! |
:-------------------------:|:-------------------------:|:-------------------------:
![Sienk Light](/images/sienk-light.jpeg) | ![Top View](/images/top.jpeg) | ![Ready](/images/ready.jpeg)

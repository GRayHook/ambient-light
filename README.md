# Ambient light

## What?

Simple daemon that scan screen and calculate main color of screen with adjusting of saturation.

## Build

just type

    $ make
    
## Run

    $ ./build/ambient_light
    
## Install

Install it to `/usr/loca/bin`:
    
    # make install
    
    
## Features

It can send main color to [g13 driver](https://github.com/ecraven/g13) and [g19 driver](https://github.com/GRayHook/Logitech-G19-Linux-Daemon)(have not pushed commits yet).

## How to add my transport?

Code that implements sending data to another app i call "transport". See `src/transports`. You have to implement at least one method of `color_handler_t` (`src/color.h`). `prepare` and `exit` is not necessary, but `handle` us required. And don't forget to add your transport to `src/handlers.h`.

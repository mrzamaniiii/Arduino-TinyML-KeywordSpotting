Keyword Spotting (KWS) on Arduino Nano 33 BLE Sense

## Overview
This repository contains the implementation of a Keyword Spotting (KWS) system designed for low-power embedded devices.

### 1. Data Acquisition
* Sampling: Pulse-Density Modulation (PDM) sampled at 16 kHz.
* Format: Converted to 16-bit Pulse Code Modulation (PCM) samples.

### 2. Feature Engineering
* FFT & Power Spectrum: Converting time-domain signals to frequency domain.
* Mel Filterbank: Scaling frequencies according to human auditory perception.
* Log-Energy & DCT-II: Compressing dynamic range and decorrelating features.

### 3. Deep Learning Model
* Type: Convolutional Neural Network (CNN).
* Framework: TensorFlow Lite for Microcontrollers.

## Hardware 
* Microcontroller: Arduino Nano 33 BLE Sense Rev2.
* Processor: Nordic nRF52840 (ARM Cortex-M4 @ 64MHz).
* Memory: 256 KB RAM / 1 MB Flash.

#!/usr/bin/env bash
cd glitcher && cmake . && make && make clean && cd ..
cd lpc && cmake . &&  make && make clean && cd ..

#!/bin/bash

if [[ -n $TMUX ]]; then
  echo > serial.log
  tmux split-window -h 'qemu-system-i386 -kernel build/kernel/nos/kernel.elf -display curses -monitor telnet:localhost:4444,server -s -S -serial file:serial.log'
  # tmux split-window -h 'qemu-system-i386 -kernel build/kernel/nos/kernel.elf -display curses -monitor stdio -s -S -serial file:serial.log'
  tmux select-pane -L
  tmux split-window -v 'tail -f serial.log'
  tmux select-pane -R
  tmux split-window -v 'sleep 1; nc localhost 4444'
  tmux select-pane -L
  tmux select-pane -U
else
  qemu-system-i386 -kernel build/kernel/nos/kernel.elf -display curses -monitor telnet:localhost:4444,server -s -S
fi

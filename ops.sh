#!/bin/bash

if [[ -n $TMUX ]]; then
  echo > serial.log
  tmux split-window -h 'qemu-system-i386 -kernel build/kernel.bin -display curses -monitor telnet:localhost:4444,server -s -S -serial file:serial.log'
  # tmux split-window -h 'qemu-system-i386 -kernel build/kernel.bin -display curses -monitor stdio -s -S -serial file:serial.log'
  tmux select-pane -L
  tmux split-window -v 'tail -f serial.log'
  tmux select-pane -R
  tmux split-window -v 'sleep 1; nc localhost 4444'
  tmux select-pane -L
  tmux select-pane -U
else
  qemu-system-i386 -kernel build/kernel.bin -display curses -monitor telnet:localhost:4444,server -s -S
fi

# nm build/kernel.bin | awk '{print "0x"$1,$3}' > nos.sym

# c010023a:       66 87 db                xchg   %bx,%bx
# lb 0xc010023a
# xp /12wx 0x104000
# xp /12wx 0x104c00

#read_dbg_dword(0xc0104c04)
#BOCHS>xp /12wx 0x104c00
#[bochs]:
#0x0000000000104c00 <KERNEL_BEGIN_PHYS+4c00>:	0x000000e3	0x004000e3	0x00800083	0x00c00083
#0x0000000000104c10 <KERNEL_BEGIN_PHYS+4c20>:	0x00704003	0x00705003	0x00706003	0x00707003
#0x0000000000104c20 <KERNEL_BEGIN_PHYS+4c40>:	0x00708063	0x00709003	0x0070a003	0x0070b003

# 0x400000 + (772 * 4096)

# send_dbg_command("info tab")

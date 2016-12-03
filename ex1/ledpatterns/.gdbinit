target remote :2331
set output-radix 16
set mem inaccessible-by-default off
#set remote memory-read-packet-size 1200
#set remote memory-read-packet-size fixed
mon speed 4000
mon endian little
mon reset 0


define hook-quit
    set confirm off
end

define px
    printf "eax: %d\n", $eax 
    printf "ebx: %d\n", $ebx 
    printf "ecx: %d\n", $ecx 
    printf "edx: %d\n", $edx 
end

define prx
    printf "rax: %d\n", $rax 
    printf "rbx: %d\n", $rbx 
    printf "rcx: %d\n", $rcx 
    printf "rdx: %d\n", $rdx 
end

define pn
    printf "r0: 0x%x\t%d\n", $r0, $r0
    printf "r1: 0x%x\t%d\n", $r1, $r1
    printf "r2: 0x%x\t%d\n", $r2, $r2
    printf "r3: 0x%x\t%d\n", $r3, $r3
    printf "r4: 0x%x\t%d\n", $r4, $r4
    printf "r5: 0x%x\t%d\n", $r5, $r5
    printf "r6: 0x%x\t%d\n", $r6, $r6
    printf "r7: 0x%x\t%d\n", $r7, $r7
    printf "r8: 0x%x\t%d\n", $r8, $r8
    printf "r9: 0x%x\t%d\n", $r9, $r9
end

define pstack
    printf "-08(%%rbp): %d\n", $rbp-0x8
    x /w  $rbp-0x8 
    printf "-16(%%rbp): %d\n", $rbp-0x16
    printf "-24(%%rbp): %d\n", $rbp-0x24
    printf "-32(%%rbp): %d\n", $rbp-0x32
end

# color the first line of the disassembly - default is green, if you want to change it search for
# SETCOLOR1STLINE and modify it :-)
set $SETCOLOR1STLINE = 0
# set to 0 to remove display of objectivec messages (default is 1)
set $SHOWOBJECTIVEC = 1
# set to 0 to remove display of cpu registers (default is 1)
set $SHOWCPUREGISTERS = 1
# set to 1 to enable display of stack (default is 0)
set $SHOWSTACK = 0
# set to 1 to enable display of data window (default is 0)
set $SHOWDATAWIN = 0
# set to 0 to disable colored display of changed registers
set $SHOWREGCHANGES = 1
# set to 1 so skip command to execute the instruction at the new location
# by default it EIP/RIP will be modified and update the new context but not execute the instruction
set $SKIPEXECUTE = 0
# if $SKIPEXECUTE is 1 configure the type of execution
# 1 = use stepo (do not get into calls), 0 = use stepi (step into calls)
set $SKIPSTEP = 1
# show the ARM opcodes - change to 0 if you don't want such thing (in x/i command)
set $ARMOPCODES = 1
# x86 disassembly flavor: 0 for Intel, 1 for AT&T
set $X86FLAVOR = 0
# use colorized output or not
set $USECOLOR = 1
# to use with remote KDP
set $KDP64BITS = -1
set $64BITS = 0

set confirm off
set verbose off
set history filename ~/.gdb_history
set history save

.pos 0x0
                 ld   $0x1028, r5         # sp init
                 ld   $0xfffffff4, r0     # r0 = -12
                 add  r0, r5              # allocate space on stack (3 spaces)
                 ld   $0x200, r0          # r0 = 0x200 (address)
                 ld   0x0(r0), r0         # r0 = value @ 0x200 = i
                 st   r0, 0x0(r5)         # store value @ 0x200 onto stack
                 ld   $0x204, r0          # r0 = 0x204 (address)
                 ld   0x0(r0), r0         # r0 = value @ 0x204 = j
                 st   r0, 0x4(r5)         # store value @ 0x204 onto stack
                 ld   $0x208, r0          # r0 = 0x208 (address)
                 ld   0x0(r0), r0         # r0 = value @ 0x208 = k
                 st   r0, 0x8(r5)         # store vlaue @ 0x208 onto stack
                 gpc  $6, r6              # r6 = pc + 6
                 j    0x300               # goto 0x300
                 ld   $0x20c, r1          # r1 = 0x20c (address)
                 st   r0, 0x0(r1)         # value @ 0x20c = r0 (return value)
                 halt                     # halt
.pos 0x200
                 .long 17                 # i
                 .long 0x0000000b         # j
                 .long 0x0000000c         # k
                 .long 0x0000000d         # l
.pos 0x300
                 ld   0x0(r5), r0         # r0 = i
                 ld   0x4(r5), r1         # r1 = j
                 ld   0x8(r5), r2         # r2 = k
                 ld   $0xfffffff6, r3     # r3 = -10
                 add  r3, r0              # i (r0) -= 10
                 mov  r0, r3              # r3 = i
                 not  r3                  
                 inc  r3                  # r3 = -(i - 10)
                 bgt  r3, L6              # goto L6 if i < 10 [-(i - 10) > 0]
                 mov  r0, r3              # r3 = i
                 ld   $0xfffffff8, r4     # r4 = -8
                 add  r4, r3              # r3 = i - 8
                 bgt  r3, L6              # goto L6 if i - 8 > 0
                 ld   $0x400, r3          # r3 = 0x400 
                 j    *(r3, r0, 4)        # goto jmptable[i-10]
.pos 0x330
                 add  r1, r2              # k = j + k           0x330 i = 10
                 br   L7                  # goto L7             0x332 
                 not  r2                  #                     0x334
                 inc  r2                  # k = -k              0x336
                 add  r1, r2              # k = k + j
                 br   L7                  # goto L7
                 not  r2                  
                 inc  r2                  # k = -k
                 add  r1, r2              # k += j
                 bgt  r2, L0              # goto L0 if (k - j) > 0
                 ld   $0x0, r2            # r2 = 0
                 br   L1                  # goto L1
L0:              ld   $0x1, r2            # r2 = 1
L1:              br   L7                  # goto L7
                 not  r1                  
                 inc  r1                  # j = -j
                 add  r2, r1              # j = k - j
                 bgt  r1, L2              # goto L2 if (k-j) > 0
                 ld   $0x0, r2            # r2 = 0
                 br   L3                  # goto L3
L2:              ld   $0x1, r2            # r2 = 1
L3:              br   L7                  # goto L7
                 not  r2                  
                 inc  r2                  # k = -k
                 add  r1, r2              # k = j - k
                 beq  r2, L4              # goto L4 if k == j
                 ld   $0x0, r2            # r2 = 0
                 br   L5                  # goto L5
L4:              ld   $0x1, r2            # r2 = 1
L5:              br   L7                  # goto L7
# DEFAULT
L6:              ld   $0x0, r2            # case 6: i < 10 || i > 18. r2 = 0
                 br   L7                  # goto L7
# CONT
L7:              mov  r2, r0              # r0 = r2
                 j    0x0(r6)             # return
.pos 0x400
# jmptable
                 .long 0x00000330         # i = 10
                 .long 0x00000384         # i = 11
                 .long 0x00000334         # i = 12
                 .long 0x00000384         # i = 13
                 .long 0x0000033c         # i = 14
                 .long 0x00000384         # i = 15
                 .long 0x00000354         # i = 16
                 .long 0x00000384         # i = 17
                 .long 0x0000036c         # i = 18
.pos 0x1000
# stack space
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         
                 .long 0x00000000         

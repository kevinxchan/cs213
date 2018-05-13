.pos 0x100
start:           ld   $sb, r5             # r5 = &sb
                 inca r5                  # &sb += 4
                 gpc  $0x6, r6            # r6 = pc + 6
                 j    main                # goto main
                 halt                     
f:               deca r5                  # &sb -= 4
                 ld   $0x0, r0            # r0 = j = 0
                 ld   0x4(r5), r1         # r1 = arg from stack
                 ld   $0x80000000, r2     # r2 = 0x80000000
f_loop:          beq  r1, f_end           # goto f_end if r1 == 0
                 mov  r1, r3              # r3 = r1
                 and  r2, r3              # r3 = r2 & r3 (checking if signed?)
                 beq  r3, f_if1           # goto f_if1 if r3 == 0
                 inc  r0                  # j++
f_if1:           shl  $1, r1              # r1 << 2
                 br   f_loop              # goto f_loop
f_end:           inca r5                  # &sb += 4
                 j    0x0(r6)             # return
main:            deca r5                  # &sb -= 4
                 deca r5                  # &sb -= 4 (making room on stack)
                 st   r6, 0x4(r5)         # store ra (r6)
                 ld   $0x8, r4            # r4 = i = 8
main_loop:       beq  r4, main_end        # goto main_end if r4 == 0
                 dec  r4                  # i--
                 ld   $x, r0              # r0 = &x
                 ld   (r0, r4, 4), r0     # r0 = x[i]
                 deca r5                  # &sb -= 4
                 st   r0, 0x0(r5)         # store x[i] onto stack
                 gpc  $0x6, r6            # r6 = pc + 6
                 j    f                   # goto f
                 inca r5                  # &sb += 4
                 ld   $y, r1              # r1 = &y
                 st   r0, (r1, r4, 4)     # y[i] = j
                 br   main_loop           # goto main_loop
main_end:        ld   0x4(r5), r6         # r6 = ra
                 inca r5                  # &sb += 4
                 inca r5                  # &sb += 4
                 j    0x0(r6)             # end
.pos 0x2000
x:               .long 0x1                
                 .long 0x2                
                 .long 0x3                
                 .long 0xffffffff         
                 .long 0xfffffffe         
                 .long 0x0                
                 .long 0xb8               
                 .long 0x1444dbe2         
y:               .long 0x1                
                 .long 0x1                
                 .long 0x2                
                 .long 0x20               
                 .long 0x1f               
                 .long 0x0                
                 .long 0x4                
                 .long 0xe                
.pos 0x8000
                 .long 0x0                # These are here so you can see (some of) the stack contents.
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x1                
                 .long 0x0                
sb:              .long 0x110              

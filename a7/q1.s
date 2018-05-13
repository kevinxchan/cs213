.pos 0x0
                 ld   $sb, r5             # r5 = &sb
                 inca r5                  # r5 = &sb + 4
                 gpc  $0x6, r6            # r6 = pc + 6
                 j    0x300               # goto main
                 halt                     # halt
.pos 0x100
                 .long 0x1000             
.pos 0x200 # adderHelper
                 ld   0x0(r5), r0         # r0 = *0x804c = add
                 ld   0x4(r5), r1         # r1 = *0x8050 = index
                 ld   $0x100, r2          # r2 = 0x100 (&aptr)
                 ld   0x0(r2), r2         # r2 = 0x1000 (aptr)
                 ld   (r2, r1, 4), r3     # r3 = aptr[r1]
                 add  r3, r0              # r0 = add + aptr[index]
                 st   r0, (r2, r1, 4)     # a[index] = add (3) + a[index]
                 j    0x0(r6)             
.pos 0x300
                 ld   $0xfffffff4, r0     # r0 = -12
                 add  r0, r5              # &sb - 12
                 st   r6, 0x8(r5)         # sb[0] = r6 = ra
                 ld   $0x1, r0            # r0 = 1
                 st   r0, 0x0(r5)         # sb[2] = 1
                 ld   $0x2, r0            # r0 = 2
                 st   r0, 0x4(r5)         # sb[1] = 2
                 ld   $0xfffffff8, r0     # r0 = -8
                 add  r0, r5              # offset sb by 8 (0x804c)
                 ld   $0x3, r0            # r0 = add = 3
                 st   r0, 0x0(r5)         # 0x804c = 3
                 ld   $0x4, r0            # r0 = index = 4
                 st   r0, 0x4(r5)         # 0x8050 = 4
                 gpc  $0x6, r6            # r6 = ra
                 j    0x200               # goto adderHelper (0x200)
                 ld   $0x8, r0            # r0 = 8
                 add  r0, r5              # r5 = &sb += 8 (0x8054)
                 ld   0x0(r5), r1         # r1 = *0x8054 = 1
                 ld   0x4(r5), r2         # r2 = *0x8058 = 2
                 ld   $0xfffffff8, r0     # r0 = -8
                 add  r0, r5              # deca r5 twice (0x804c)
                 st   r1, 0x0(r5)         # 0x804c = 1
                 st   r2, 0x4(r5)         # 0x805c = 2
                 gpc  $0x6, r6            # r6 = pc + 6
                 j    0x200               # goto adderHelper
                 ld   $0x8, r0            # r0 = 8
                 add  r0, r5              # &sb += 8
                 ld   0x8(r5), r6         # r6 = ra
                 ld   $0xc, r0            # r0 = 12
                 add  r0, r5              # &sb += 12 (this is clearing all stack space)
                 j    0x0(r6)             # goto ra (return)
.pos 0x1000
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x5                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
                 .long 0x0                
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
                 .long 0x1                
                 .long 0x2                
                 .long 0x1                
                 .long 0x2                
sb:              .long 0x10               

.pos 0x100			
                ld $t, r0					# r0 = address of t
                ld $array, r1				# r1 = address of array[]
                ld 0x0(r1), r2				# r2 = array[0]
                ld 0x4(r1), r3              # r3 = array[1]
                st r2, 0x0(r0)				# t = array[0]
				ld 0x4(r1), r4				# r4 = array[1]
				st r4, 0x0(r1)				# array[0] = array[1]
				ld 0x0(r0), r5   			# r5 = t
                st r5, 0x4(r1)				# array[1] = t
                halt
.pos 0x1000
t:				.long 0 # the variable t
.pos 0x2000
array:			.long 1 # array[0] = 1
				.long 2 # array[1] = 2

.pos 0x100
start:		ld $sb, r5			# sp = address of last word of stack
			inca r5				# sp = address of word AFTER stack
			gpc $6, r6			# r6 = pc + 6
main:		j copy				# call copy()
			halt 				# halt


.pos 0x200
copy:		deca r5				# allocate callee part of copy's frame
			st r6, (r5)			# save ra on stack

			ld $-12, r0			# r0 = -12 = -(size of callee part of copy's frame)
			add r0, r5			# allocate space for local variables on copy's frame
			ld $0, r0			# r0 = i = 0
			ld $src, r1			# r1 = &src
copy_loop:	ld (r1, r0, 4), r2	# r2 = src[i]
			beq r2, copy_end	# goto copy_end if src[i] == 0
			st r2, (r5, r0, 4)	# dst[i] = src[i]
			inc r0				# i++
			br copy_loop

copy_end:	ld $0, r1			# r0 = 0
			st r1, (r5, r0, 4)	# dst[i] = 0
			st r0, 8(r5)		# store i onto stack
			ld $12, r0			# r0 = 12 = size of callee part of copy's frame
			add r0, r5			# deallocate callee parts of copy's frame
			ld (r5), r6			# r6 = ra
			j (r6)				# return




.pos 0x1000
# data area
src:		.long 0x1010		# 0x1000
			.long 0x1010		# 0x1004
			.long 0x1010		# 0x1008
			.long 0x1010		# 0x100c
			.long 0x00ffffff  	# 0x1010, BEGIN VIRUS!!!!! 
			.long 0xffff6001	# r0 = -1, r1 = r0
			.long 0x60126023	# r2 = r1, r3 = r2
			.long 0x60346045	# r4 = r3, r5 = r4
			.long 0x60566067	# r6 = r5, r7 = r6
			.long 0xf0000000	# halt

.pos 0x8000
st:			.long 0
			.long 0
			.long 0
sb:			.long 0
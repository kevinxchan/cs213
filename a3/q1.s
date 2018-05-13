.pos 0x100 

	# STATEMENT 1: i = a[3]
	ld $i, r0						# r0 = address of i
	ld $a, r1						# r1 = address of a
	ld $3, r2						# r2 = 3
	ld (r1, r2, 4), r2				# r2 = a[3]
	st r2, (r0)						# i = a[3]	

	# STATEMENT 2: i = a[i]			
	ld (r0), r2						# r2 = (value of) i
	ld (r1, r2, 4), r3				# r3 = a[i]
	st r3, (r0)						# i = a[i]

	# STATEMENT 3: p = &j
	ld $j, r0						# r0 = address of j
	ld $p, r2						# r2 = address of p
	st r0, (r2)						# p = &j

	# STATEMENT 4: *p = 4
	ld $4, r3						# r3 = 4
	st r3, (r0)						# *p = 4 (j = 4)

	# STATEMENT 5: p = &a[a[2]]
	ld $2, r0						# r0 = 2
	ld (r1, r0, 4), r3				# r3 = a[2]
	shl $2, r3						# r3 = r3 << 2 (r3 * 4)
	add r1, r3						# r3 = a[0] + 4*a[2] (offset)
	st r3, (r2)						# p = &a[a[2]]

	# STATEMENT 6: *p = *p + a[4]
	ld (r2), r0						# r0 = &a[a[2]]
	ld (r0), r3						# r3 = a[a[2]]
	ld $4, r4						# r4 = 4
	ld (r1, r4, 4), r4				# r4 = a[4]
	add r3, r4						# r4 = *p + a[4]
	st r4, (r0)						# *p = *p + a[4] 

	halt



.pos 0x200
# data area

i:	.long 0		# i
j:	.long 0		# j
p:	.long 0		# p (pointer)
a:	.long 1		# a[0]
	.long 2 	# a[1]
	.long 3 	# a[2]
	.long 4 	# a[3]
	.long 5 	# a[4]
	.long 6 	# a[5]
	.long 7 	# a[6]
	.long 8 	# a[7]
	.long 9 	# a[8]
	.long 10 	# a[9]
.pos 0x100

	# STATEMENT 1: tmp = 0
	ld $tmp, r0				# r0 = address of tmp
	ld $0, r1				# r1 = 0
	st r1, (r0)				# tmp = 0

	# STATEMENT 2: tos = 0
	ld $tos, r0				# r0 = address of tos
	st r1, (r0)				# tos = 0

	# STATEMENT 3: s[tos] = a[0]
	ld $a, r0				# r0 = address of a
	ld $tos, r1				# r1 = address of tos
	ld (r1), r2				# r2 = (value of) tos
	ld $s, r3				# r3 = address of s
	ld 0x0(r0), r4			# r4 = a[0]
	st r4, (r3, r2, 4)		# s[tos] = a[0]

	# STATEMENT 4: tos++
	inc r2					# r2 = r2 + 1
	st r2, (r1)				# tos = tos + 1

	# STATEMENT 5: s[tos] = a[1]
	ld 0x4(r0), r4			# r4 = a[1]
	st r4, (r3, r2, 4)		# s[tos] = a[1]

	# STATEMENT 5: tos++
	inc r2					# r2 = r2 + 1
	st r2, (r1)				# tos = tos + 1

	# STATEMENT 6: s[tos] = a[2]
	ld 0x8(r0), r4			# r4 = a[2]
	st r4, (r3, r2, 4)		# s[tos] = a[2]

	# STATEMENT 6 AND 7: tos++; tos--
	inc r2					# r2 = r2 + 1
	st r2, (r1)				# tos = tos + 1
	dec r2					# r2 = r2 - 1
	st r2, (r1)				# tos = tos - 1

	# STATEMENT 8: tmp = s[tos]
	ld $tmp, r0				# r0 = address of tmp
	ld (r1), r4				# r4 = tos
	ld (r3, r4, 4), r5		# r5 = s[tos]
	st r5, (r0)				# tmp = s[tos]

	# STATEMENT 9: tos--
	dec r2					# r2 = r2 - 1
	st r2, (r1)				# tos = tos - 1

	# STATEMENT 10: tmp = tmp + s[tos]
	ld (r1), r4				# r4 = tos
	ld (r3, r4, 4), r5		# r5 = s[tos]
	ld (r0), r6				# r6 = tmp
	add r5, r6				# r6 = tmp + s[tos]
	st r6, (r0)				# tmp = tmp + s[tos]

	# STATEMENT 11: tos--
	dec r2					# r2 = r2 - 1
	st r2, (r1)				# tos = tos - 1

	# STATEMENT 12: tmp = tmp + s[tos]
	ld (r1), r4				# r4 = tos
	ld (r3, r4, 4), r5		# r5 = s[tos]
	ld (r0), r6				# r6 = tmp
	add r5, r6				# r6 = tmp + s[tos]
	st r6, (r0)				# tmp = tmp + s[tos]

	halt

.pos 0x200
# Data area
a:		.long 1 	# a[0]
		.long 2		# a[1]
		.long 3		# a[2]
s:		.long 4		# s[0]
		.long 5		# s[1]
		.long 6		# s[2]
		.long 7		# s[3]
		.long 8		# s[4]
tos:	.long 0		# tos
tmp:	.long 0		# tmp

.pos 0x100
			ld $a, r0			# r0 = a's address
			ld $b, r1			# r1 = b's address
			ld 0x0(r1), r2		# r2 = b's VALUE
			ld 0x0(r1), r3		# r2 = b's VALUE (manipulate)
			inc r3				# r2 = r2 + 1 (b = b + 1)
			inca r3				# r2 = r2 + 4
			shr $1, r3			# r2 = r2 >> 2 (/ 2)
			and r2, r3			# r3 = r3 & r2
			shl $2, r3			# r3 = r3 << 2
			st r3, (r0)			# a = r3
			halt
.pos 0x1000
a:			.long 0				# the variable a
.pos 0x2000
b:			.long 0xf 			# the variable b
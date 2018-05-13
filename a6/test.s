.pos 0x100
start:		ld $0, r0			# r0 = 0
			ld $1, r1			# r1 = 1
			gpc $6, r6			# r6 = pc of next instruction
			j foo				# goto foo()
			ld $0, r2			# r2 = 2
			ld $1, r2			# r2 = 1
			ld $2, r2			# r2 = 2
			bgt r0, true		# goto true if r1 > 0
			beq r1, true		# goto true if r0 == 0
			j 2(r6)				# skip br start
			br start			# goto start
			ld $2, r1			# r1 = 2
			halt

.pos 0x200
true:		ld $5, r5			# r5 = 5
			halt

.pos 0x2000
foo:		ld $4, r4			# r4 = 4
			j (r6)				# return


.pos 0x100
			ld $0, r0				# r0 = 0 = i'
			ld $a, r1				# r1 = &a
			ld $5, r2				# r2 = 5
			ld $0, r4				# r4 = 0 = s'
			not r2
			inc r2					# r2 = -5
loop:		mov r0, r3				# r3 = i'
			add r2, r3				# r3 = i' - 5
			beq r3, done			# goto done if i' == 5
			ld (r1, r0, 4), r3		# r3 = a[i']
			bgt r3, then			# goto then if a[i'] > 0
else:		inc r0					# i'++
			br loop					# goto loop
then:		add r3, r4				# s' += a[i']
			inc r0					# i'++
			br loop					# goto loop
done:		ld $i, r2				# r2 = &i
			st r0, (r2)				# i = i'
			ld $s, r2				# r2 = &s
			st r4, (r2)				# s = s'
			halt 



.pos 0x2000
# Data area
i:		.long 10			# i
a:		.long 10			# a[0]
		.long -30			# a[1]
		.long -12			# a[2]
		.long 4				# a[3]
		.long 8				# a[4]
s:		.long 0				# s

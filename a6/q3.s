.pos 0x100
main:            ld $stack, r5					# r5 = stack pointer

				ld $n, r0                      # r0 = &n
                 ld (r0), r0                    # r0 = n
                 not r0
                 inc r0                         # r0 = -n
	              st r0, (r5)					# stack[0] = -n
                 ld   $s, r1                    # r1 = &s
                 ld   0x0(r1), r1               # r1 = s = &s->id (base->id)
                 ld $20, r7                     # r7 = 20

# procedure to calculate average grades for each student in the array
listavg:         ld (r5), r0					# r0 = stack[0]
				beq r0, pre_outer          		# goto pre_outer if r0 == n

                 ld   $0x4, r0                  # r0 = 4 = grade.size()
                 not  r0                  
                 inc  r0                        # r0 = -4 = i'
                 inca r1                         # r1 = &s->grade[0]
                 ld   $0x0, r2                  # r2 = 0 = i
                 mov  r2, r3                    # r3 = 0 = avg
averagehelper:   beq  r0, avg                   # goto avg if r0 = grade.size()
                 ld   (r1, r2, 4), r4           # r4 = s->grade[i]
                 add  r4, r3                    # avg += s->grade[i]
                 inc  r2                        # i'++
                 inc  r0                        # grade.size()--
                 br   averagehelper             # goto loop

avg:             shr  $2, r3                    # avg /= 4
                 st   r3, 16(r1)                # s->average = avg
                 add r7, r1                     # r1 = &s[i]->id

                 ld (r5), r0					# r0 = n
                 inc r0                         # n--
                 st r0, (r5)					# stack[0] = updated n
                 br listavg                     # goto list

# to swap two adjacent students
pre_outer:		ld $n, r0						# r0 = &n
				ld (r0), r0						# r0 = n = i
				st r0, (r5)						# stack[0] = n
				br pre_inner_first				# first iteration

bs_outer_loop:	ld 4(r5), r0					# r0 = updated i
				beq r0, end						# goto end if i == 0
				dec r0							# i--
				st r0, 4(r5)					# stack[1] = i
				br pre_inner_else				# goto pre_inner_else

pre_inner_first:ld $1, r1						# r1 = 1 = j
				deca r5
				st r0, (r5)						# stack[1] = -(n - 1)
				deca r5							
				st r1, (r5)						# stack[2] = j
				br bs_inner_loop				# goto inner loop

pre_inner_else: ld $1, r1						# r1 = 1 = j
				st r1, (r5)						# stack[2] = j
				br bs_inner_loop				# goto inner loop

bs_inner_loop:	ld (r5), r1						# r1 = j 
				ld 4(r5), r0					# r0 = i
				not r1
				inc r1							# r1 = -j
				add r0, r1						# i - j LOAD I FROM STACK
				beq r1, bs_outer_loop			# goto bs_outer_loop if (i - j) == 0 TODO - UPDATE FROM VALUES IN STACK!!
				not r1
				inc r1
				bgt r1, bs_outer_loop

pre_swap:       ld $6, r0						# r0 = 6	TO LOOP THROUGH STRUCTS
				not r0
				inc r0							# r0 = -6
				ld $s, r1						# r1 = &s
				ld (r1), r1						# r1 = s = &s[0]->id 
				ld (r5), r4						# r4 = j
				br iterate						# goto iterate

cont:			ld $0, r7						# r7 = 0 	OFFSET FOR STRUCT ELEMENTS
				ld 20(r1), r2					# r2 = s[0]->average (A[j]->average)
				# ld $-24, r3						# r3 = -24 TODO - CHANGE FROM STATIC OFFSET TO DYNAMIC (USE J-1 AND J)
				# add r1, r3						# r3 = &s[j-1]->average (&A[j-1]->average)
				ld 20(r3), r3					# r3 = s[i]->average (A[j])
				br if 							# goto if

#### SKIP IF J == 0 ####
iterate:		#ld 4(r5), r7					# r7 = i
				#add r4, r7						# r7 = i - j
				beq r4, cont					# goto cont if j == 0
				# goto cont if i - j == 0 TODO - CHANGE DO J - I INSTEAD

				inca r1
				inca r1
				inca r1
				inca r1
				inca r1
				inca r1							# r1 = s = &A[j] where j starts at 1

				mov r1, r3						# r3 = &A[j]

				deca r3
				deca r3
				deca r3
				deca r3
				deca r3
				deca r3							# r3 = &A[j-1]

				dec r4							# j'--
				br iterate						# goto iterate

if:				not r2
				inc r2							# r2 = - s[j]->average
				add r3, r2						# s[j-1]->average - s[j]->average
				bgt r2, then 					# goto swap if s[j-1].average > s[j].average
				br else							# goto else 

then:			gpc $6, r6						# pc += 6
				j swap 							# goto swap helper
				ld (r5), r1						# r1 = j
				inc r1							# j++
				st r1, (r5)						# stack[2] = j
				br bs_inner_loop				# goto bs_inner_loop

# HELPER FUNCTION FOR SWAP
swap:			beq r0, post_swap				# goto post_swap if r0 == 6
				ld $s, r1						# r1 = &s
				ld (r1), r1						# r1 = s = &s->id

				ld (r5), r4						# r4 = j
				br iterate_swap					# goto iterate_swap

swap_cont:		ld $24, r2                      # r2 = 24 (offset to next element)
                mov r1, r3						# r3 = s = &s->id
                add r2, r3						# r3 = &s[i]->id
                ld (r3, r7, 4), r4				# r4 = s[i]->id
                ld (r1, r7, 4), r2				# r2 = tmp = s->id
                st r4, (r1, r7, 4)				# s = s[i]
                st r2, (r3, r7, 4)				# s[i] = tmp

                inca r1							# r1 = s[i]
                inc r0							# r0++
                inc r7							# r7++
                br swap							# goto swap

iterate_swap:	dec r4							# j--
				beq r4, swap_cont				# goto swap_cont if j == 0
				ld $24, r2						# r2 = 24
				add r2, r1						# r1 = &s->id = &s[j]->id
				#dec r4							# j--
				br iterate_swap					# goto iterate_swap

post_swap:		j (r6)							# return 

else:			ld (r5), r1						# r1 = j
				inc r1							# j++
				st r1, (r5)						# stack[2] = j
				br bs_inner_loop				# goto bs_inner_loop

end:            ld $n, r0						# r0 = &n
				ld (r0), r0						# r0 = n
				shr $1, r0						# r0 = floor(n/2)
				# inc r0							# r0 = floor(n/2) + 1 (median position)
				ld $s, r1						# r1 = &s
				ld (r1), r1						# r1 = s = &s->id
				ld $m, r2						# r2 = &m
				ld $24, r3						# r3 = 24

median:			beq r0, done					# goto done if r0 == 0
				add r3, r1						# r1 = &s[i]->id
				dec r0							# r0--
				br median						# goto median

done:			ld (r1), r1						# r1 = s[i]->id
				st r1, (r2)						# m = s[i]->id
				halt


.pos 0x2000
n:               .long 7                  # number of students
m:               .long 0x0                # put the answer here
s:               .long base0              # address of the array
base0:           .long 9999               # student ID INDEX 0
                 .long 10                 # grade 0
                 .long 20                 # grade 1
                 .long 30                 # grade 2
                 .long 40                 # grade 3
                 .long 0                  # computed average (25)
base2:           .long 2222               # student ID INDEX 2
                 .long 20                 # grade 0
                 .long 20                 # grade 1
                 .long 20                 # grade 2
                 .long 20                 # grade 3
                 .long 0                  # computed average (20)
base1:           .long 1111               # student ID INDEX 1
                 .long 10                 # grade 0
                 .long 10                 # grade 1
                 .long 10                 # grade 2
                 .long 10                 # grade 3
                 .long 0                  # computed average (10)
base3:           .long 3333               # student ID INDEX 3
                 .long 30                 # grade 0
                 .long 30                 # grade 1
                 .long 30                 # grade 2
                 .long 30                 # grade 3
                 .long 0                  # computed average (30)
base4:           .long 4444               # student ID INDEX 4
                 .long 0                 # grade 0
                 .long 0                 # grade 1
                 .long 0                 # grade 2
                 .long 0                 # grade 3
                 .long 0                  # computed average (0)
base5:			.long 5555 				# student ID
				.long 80 				# grade 0
				.long 60 				# grade 1
				.long 78 				# grade 2
				.long 90 				# grade 3
				.long 0 				# computed average
base6:			.long 6666 				# student ID
				.long 50 				# grade 0
				.long 20 				# grade 1
				.long 90 				# grade 2
				.long 100 				# grade 3
				.long 0 				# computed average

.pos 0x3000
				.long 0					# j
				.long 0					# i
stack:			.long 0					# n


# mp1.S - missile-command version
# Good luck, and have fun!

.data
	
    # Constants for accessing the fields of a struct missile, 
    # struct missile is defined in rtc.h and mp1.h

	NEXT 	  = 0
	X	  = 4
	Y	  = 8
	VX	  = 12
	VY	  = 16
	DEST_X	  = 20
	DEST_Y	  = 24
	EXPLODED  = 28
	C	  = 32

    # Character to draw for an explosion - '@'

	EXPLOSION = 64


# Data shared between this file and rtc.c helper functions

# This '.globl' directive makes these symbols visible externally
.globl mp1_missile_list, base_alive, mp1_score

mp1_missile_list:	.long 0x0	# Head of list pointer
base_alive:		.long 0x0	# Array of 3 bytes, plus a padding byte
mp1_score:		.long 0x0	# Player's current score


# Data private to this  file

base_pic:     	    .string "/^^^\\"	# Picture of a live base
dead_base_pic: 	    .string "xxxxx"	# Picture of a dead base
crosshairs_x:		.long 0x0	# X-position of the crosshairs
crosshairs_y:		.long 0x0	# Y-position of the crosshairs

.text

# void mp1_poke(void);
# You can use this function to write to video memory.
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX

mp1_poke:
		movl    vmem_base_addr(,1),%edx
		movb    %cl,(%edx,%eax,1)
		ret

#   ----------------- Exported functions ---------------------

# void mp1_rtc_tasklet(unsigned long garbage);
# Performs three tasks:
#	(1) updates the list of missiles (implement this in update_missiles,
#           below, and call it from here).
#	(2) Redraw the bases - they may have been overwritten with missiles
#	(3) Redraw the crosshairs - it may have been overwritten with missiles
# Inputs   : none
# Outputs  : none
# Registers: Standard C calling convention

.globl mp1_rtc_tasklet
mp1_rtc_tasklet:
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%eax			# save everything
		pushl	%ebx
		pushl	%ecx
		pushl	%edx
		pushl	%esi
		pushl	%edi
		pushfl
		call	update_missiles
		call	draw_cities_xhairs
		popfl
		popl	%edi
		popl	%esi
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%eax			# restore everything
		leave
		ret

# int mp1_ioctl(unsigned long arg, unsigned int cmd)
# The dispatch function for the MP1 ioctls - should use the cmd argument
# and a jumptable to execute one of the specific ioctls implemented below.
# Inputs   : unsigned long arg - parameter to the mp1_ioctl_....
#          : unsigned int cmd  - specifies which mp1_ioctl_... function 
#          :                     to execute
# Outputs  : Returns an integer - depends on which ioctl() function is called
# Registers: Standard C calling convention

.ioctljtab:
		.long	mp1_ioctl_startgame
		.long	mp1_ioctl_addmissile
		.long	mp1_ioctl_movexhairs
		.long	mp1_ioctl_getstatus
		.long	mp1_ioctl_endgame

.globl mp1_ioctl
mp1_ioctl:
		pushl	%ebp
		movl	%esp, %ebp
		movl	12(%ebp), %eax		# [eax = cmd]
		cmpl	$0, %eax		# cmd < 0?
		jl	.invalid
		cmpl	$4, %eax		# cmd > 4?
		jg	.invalid
		lea	.ioctljtab, %edx	# [edx = jtab ptr]
		movl	(%edx,%eax,4), %edx	# get function addr...
		jmp	*%edx			# ...and jump there
	.invalid:
		movl	$-1, %eax		# return -1
		leave
		ret


# ----------------- Functions private to this file -------------------

draw_pixel:					# wrapper for "mp1_poke"
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%eax			# [callee save]
		pushl	%ebx
		pushl	%ecx
		pushl	%edx			# "mull" and "poke" clobber edx
		movl	8(%ebp), %eax		# args: (y, x, long_char, shift_flag)
		movl	12(%ebp), %ebx
		movl	16(%ebp), %ecx
		movl	20(%ebp), %edx
		cmpl	$0, %edx		# shift?
		je	.noshift
		shrl	$16, %eax
		shrl	$16, %ebx
	.noshift:
		imull	$80, %eax		# 80 y
		addl	%ebx, %eax		# 80 y + x
		imull	$2, %eax		# 2 (80 y + x) = offset
		call	mp1_poke
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%eax			# [callee restore]
		leave
		ret

remove_missile:
		pushl	$1
		pushl	$' '
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# - erase: draw(y, x, ' ', shift=True)
		movl	NEXT(%edi), %ebx	# 	[ebx = next]
		cmpl	$0, %esi
		je	.firstitem
		movl	%ebx, NEXT(%esi)	# - delete (middle): esi (-> edi) -> ebx
		jmp	.rmvdone
	.firstitem:
		cmpl	$0, NEXT(%edi)
		je	.finalitem		# prev = esi, next = 0
		movl	%ebx, mp1_missile_list	# - delete (first): MLIST (-> edi) -> ebx
		jmp	.rmvdone
	.finalitem:
		movl	$0, mp1_missile_list	# - delete (final): MLIST (-> edi) -> 0
	.rmvdone:
		pushl	%edi
		call	mp1_free		# - free(*this)
		addl	$20, %esp		# 	discard everything
		movl	%ebx, %edi		# 	for continuing traversal
		ret

update_missiles:
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi
		movl	mp1_missile_list, %edi	# [esi = prev] [edi = this]
		movl	$0, %esi		# signal of first item
	.traverse:
		cmpl	$0, %edi		# last element?
		je	.travdone
		movl	Y(%edi), %eax		# STEP 1: out of range?
		movl	X(%edi), %ebx		# 	[eax = y] [ebx = x]
		xorl	%ecx, %ecx		# 	[ecx = OOR flag]
		cmpl	$0x00000000, %eax
		jge	.yge0flg		# 	y >= 0?
		movl	$0x00000000, %eax
		movl	$1, %ecx
	.yge0flg:
		cmpl	$0x0018FFFF, %eax
		jle	.yle24flg		# 	y <= 24?
		movl	$0x0018FFFF, %eax
		movl	$1, %ecx
	.yle24flg:
		cmpl	$0x00000000, %ebx
		jge	.xge0flg		# 	x >= 0?
		movl	$0x00000000, %ebx
		movl	$1, %ecx
	.xge0flg:
		cmpl	$0x004FFFFF, %ebx
		jle	.xle79flg		# 	x <= 79?
		movl	$0x004FFFFF, %ebx
		movl	$1, %ecx
	.xle79flg:
		cmpl	$0, %ecx
		je	.inrange		# 	Handle OOR missiles
		call	remove_missile		# 	remove(y, x, *prev)
		jmp	.travnext
	.inrange:				# STEP 2: exploding?
		cmpl	$0, EXPLODED(%edi)
		je	.flying			# 	flying if not exploding
		movl	DEST_Y(%edi), %ecx	# 	[ecx = dest_y]
		movl	DEST_X(%edi), %edx	# 	[edx = dest_x]
		cmpl	%ecx, %eax
		jne	.flying			# 	flying if not at dest
		cmpl	%edx, %ebx
		jne	.flying
		pushl	%eax
		pushl	%ebx			# 	would be useful for removing
		pushl	%edi
		call	missile_explode		# 	explode(*this)
		popl	%edi
		cmpl	$0, %eax
		je	.silent			# 	notify unless "explode" returns 0
		call	mp1_notify_user		# 	[clobbering ecx, edx is fine]
	.silent:
		popl	%ebx
		popl	%eax
		decl	EXPLODED(%edi)
		cmpl	$0, EXPLODED(%edi)
		jne	.expcont		# 	otherwise, keep exploding
		call	remove_missile		# 	remove(y, x, *prev)
		jmp	.travnext
	.expcont:
		pushl	$1
		pushl	EXPLOSION(%edi)
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# 	draw(y, x, EXPLOSION, shift=True)
		addl	$16, %esp
		jmp	.travnext
	.flying:				# STEP 3: flying? (see "movexhairs")
		movl	VY(%edi), %ecx		# 	eax | ebx | ecx | edx | esi | edi
		movl	VX(%edi), %edx		# 	            dy    dx
		addl	%ecx, %eax		# 	 y'
		addl	%edx, %ebx		# 	       x'
		movl	Y(%edi), %ecx		# 	             y
		movl	X(%edi), %edx		# 	                   x
		movl	%eax, Y(%edi)
		movl	%ebx, X(%edi)
		shrl	$16, %eax		# 	 y'H
		shrl	$16, %ebx		# 	       x'H
		shrl	$16, %ecx		# 	             yH
		shrl	$16, %edx		# 	                   xH
		cmpl	%eax, %ecx
		jne	.redrawmsl
		cmpl	%ebx, %edx
		jne	.redrawmsl
		jmp	.nredrawmsl
	.redrawmsl:
		pushl	$0
		pushl	$' '
		pushl	%edx
		pushl	%ecx
		call	draw_pixel		# 	draw(yH, xH, ' ', shift=False)
		addl	$16, %esp
	.nredrawmsl:
		pushl	$0
		pushl	C(%edi)
		pushl	%ebx
		pushl	%eax			# 	always...
		call	draw_pixel		# 	draw(y'H, x'H, C, shift=False)
		addl	$16, %esp
	.travnext:
		movl 	%edi, %esi
		movl	NEXT(%esi), %edi	# next element
		jmp	.traverse
	.travdone:
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret

draw_city:
		cmpl	$0, %edx
		jne	.alive
		lea	dead_base_pic, %eax
		jmp	.drawmain
	.alive:
		lea	base_pic, %eax		# [eax = ptr to pic]
	.drawmain:
		movl	$5, %ecx		# [ecx = down counter]
		pushl	$0
	.drawloop:
		pushl	(%eax)
		pushl	%esi
		pushl	$24
		call	draw_pixel		# draw(24, x, *pic, shift=False)
		addl	$12, %esp
		incl	%eax
		incl	%esi
		decl	%ecx
		cmpl	$0, %ecx
		jne	.drawloop
		addl	$4, %esp		# discard the final 0
		ret

draw_cities_xhairs:
		pushl	%ebp
		movl	%esp, %ebp
		movl	base_alive, %ebx	# [ebx = base flags]
		movl	%ebx, %edx
		andl	$0x000000FF, %edx	# [edx = state vector]
		movl	$18, %esi		# [esi = x coord]
		call	draw_city		# 1st city appears at (18, 24)
		movl	%ebx, %edx
		andl	$0x0000FF00, %edx
		movl	$38, %esi
		call	draw_city		# 2nd city appears at (38, 24)
		movl	%ebx, %edx
		andl	$0x00FF0000, %edx
		movl	$58, %esi
		call	draw_city		# 3rd city appears at (58, 24)
		movl	crosshairs_y, %eax
		movl	crosshairs_x, %ebx
		shrl	$16, %eax
		shrl	$16, %ebx
		pushl	$0
		pushl	$'+'
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# draw crosshairs
		addl	$16, %esp
		leave
		ret

mp1_ioctl_startgame:
		movl	$0, mp1_missile_list
		movl	$0, mp1_score
		movl	$0x00FFFFFF, base_alive
		movl	$0x00280000, crosshairs_x
		movl	$0x000C8000, crosshairs_y
		leave
		ret

mp1_ioctl_addmissile:
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi
		pushl	%ecx			# [caller save]
		pushl	%edx
		pushl	$36
		call	mp1_malloc
		addl	$4, %esp
		popl	%edx			# [caller restore]
		popl	%ecx
		cmpl	$0, %eax		# malloc failed?
		je	.mallocfail
		movl	%eax, %edi		# [edi = "to" addr]
		movl	8(%ebp), %esi		# [esi = "from" addr]
		pushl	%ecx
		pushl	%edx			# [caller save]
		pushl	$36
		pushl	%esi
		pushl	%edi
		call	mp1_copy_from_user	# copy(to, from, 36)
		addl	$12, %esp
		popl	%edx			# [caller restore]
		popl	%ecx
		cmpl	$0, %eax		# memcpy failed?
		jne	.u2kcpyfail
		movl	mp1_missile_list, %eax
		movl	%eax, NEXT(%edi)	# update linked list
		movl	%edi, mp1_missile_list
		movl	$0, %eax		# return 0
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret
	.u2kcpyfail:
		pushl	%ecx			# [caller save]
		pushl	%edx			# [we're sure that eax = 0]
		pushl	%edi
		call	mp1_free
		addl	$4, %esp
		popl	%edx
		popl	%ecx			# [caller restore]
	.mallocfail:
		movl	$-1, %eax		# return -1
		leave
		ret

mp1_ioctl_movexhairs:
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi
		movl	8(%ebp), %ecx		# eax | ebx | ecx | edx | esi | edi
		movl	8(%ebp), %edx
		shrl	$16, %ecx		#             dy
		andl	$0xFFFF, %edx		#                   dx
		movl	crosshairs_y, %eax	#  y
		movl	crosshairs_x, %ebx 	#        x
		addl	%ecx, %eax		#  y'
		addl	%edx, %ebx		#        x'
		cmpl	$0x00000000, %eax
		jge	.yge0			# ensure y' >= 0
		movl	$0x00000000, %eax
	.yge0:
		cmpl	$0x0018FFFF, %eax
		jle	.yle24			# ensure y' <= 24
		movl	$0x0018FFFF, %eax
	.yle24:
		cmpl	$0x00000000, %ebx
		jge	.xge0			# ensure x' >= 0
		movl	$0x00000000, %ebx
	.xge0:
		cmpl	$0x004FFFFF, %ebx
		jle	.xle79			# ensure x' <= 79
		movl	$0x004FFFFF, %ebx
	.xle79:					#  y' |  x' | dy  | dx  |     |
		movl	%eax, %ecx		#              y'
		movl	%ebx, %edx		#                    x'
		movl	crosshairs_y, %esi	#                          y
		movl	crosshairs_x, %edi	#                                x
		shrl	$16, %ecx		#              y'H
		shrl	$16, %edx		#                    x'H
		shrl	$16, %esi		#                          yH
		shrl	$16, %edi		#                                xH
		cmpl	%ecx, %esi
		jne	.redrawxhr
		cmpl	%edx, %esi
		jne	.redrawxhr
		jmp	.nredrawxhr
	.redrawxhr:
		pushl	$0
		pushl	$' '
		pushl	%edi
		pushl	%esi
		call	draw_pixel		# draw(y'H, x'H, '+', shift=False)
		addl	$12, %esp
		pushl	$'+'
		pushl	%edx
		pushl	%ecx
		call	draw_pixel		# draw(y, x, ' ', shift=False)
		addl	$16, %esp
	.nredrawxhr:
		movl	%eax, crosshairs_y	# store new crosshairs
		movl	%ebx, crosshairs_x
		movl	$0, %eax		# return 0
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret

mp1_ioctl_getstatus:
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi
		movl	mp1_score, %ebx		# [ebx = status value]
		andl	$0x0000FFFF, %ebx	# mask out higher 16 bits
		movl	base_alive, %ecx	# [ecx = base flags]
		movl	%ecx, %edx
		andl	$0x000000FF, %edx
		cmpl	$0, %edx
		je	.c1down
		orl	$0x00010000, %ebx	# bit 16 set = city 1 alive
	.c1down:
		movl	%ecx, %edx
		andl	$0x0000FF00, %edx
		cmpl	$0, %edx
		je	.c2down
		orl	$0x00020000, %ebx	# bit 17 set = city 2 alive
	.c2down:
		movl	%ecx, %edx
		andl	$0x00FF0000, %edx
		cmpl	$0, %edx
		je	.c3down
		orl	$0x00040000, %ebx	# bit 18 set = city 3 alive
	.c3down:
		movl	mp1_score, %ecx		# backup
		movl	%ebx, mp1_score
		lea	mp1_score, %esi		# [lea = "from"] (kernel label)
		movl	8(%ebp), %edi		# [edi = "to" addr]
		pushl	%ecx			# [caller save]
		pushl	%edx
		pushl	$4
		pushl	%esi
		pushl	%edi
		call	mp1_copy_to_user	# copy(to, from, 4)
		addl	$12, %esp
		popl	%edx
		popl	%ecx			# [caller restore]
		movl	%ecx, mp1_score		# restore
		cmpl	$0, %eax
		jne	.k2ucpyfail
		movl	$0, %eax		# return 0
		jmp	.gstdone
	.k2ucpyfail:
		movl	$-1, %eax		# return -1
	.gstdone:
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret

mp1_ioctl_endgame:
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi
		movl	mp1_missile_list, %esi	# similar to ".traverse"
		movl	NEXT(%esi), %esi	# translate to "struct *missile"
	.cleanup:
		cmpl	$0, %esi
		je	.clupdone
		movl	NEXT(%esi), %edi	# store "next" before freeing
		pushl	%eax
		pushl	%ecx			# [caller save]
		pushl	%edx
		pushl	%esi
		call	mp1_free
		addl	$4, %esp
		popl	%edx
		popl	%ecx			# [caller restore]
		popl	%eax
		movl	%edi, %esi
		jmp	.cleanup
	.clupdone:
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret


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

# CALLING HIERARCHY
# Order | Function
#    1  | mp1_rtc_tasklet
#    5  |     update_missiles
#    4  |         remove_missile
#    7  |     draw_cities_xhairs
#    6  |         draw_city
#    2  | mp1_ioctl
#    8  |     mp1_ioctl_startgame
#    9  |     mp1_ioctl_addmissile
#   10  |     mp1_ioctl_movexhairs
#   11  |     mp1_ioctl_getstatus
#   12  |     mp1_ioctl_endgame
#    3  | draw_pixel

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
		pushl	%ebx
		pushl	%esi			# [callee save]
		pushl	%edi
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
		pushl	%edx
		movl	8(%ebp), %eax		# [eax = y]
		movl	12(%ebp), %ebx		# [ebx = x]
		movl	16(%ebp), %ecx		# [ecx = long_char]
		movl	20(%ebp), %edx		# [edx = shift_flag]
		cmpl	$0, %edx
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
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%eax			# [callee save]
		pushl	%ebx
		pushl	%ecx
		pushl	%edx
		pushl	%esi
		pushl	%edi
		movl	8(%ebp), %eax		# [eax = y]
		movl	12(%ebp), %ebx		# [ebx = x]
		movl	16(%ebp), %ecx		# [ecx = prev] - validity is uncertain
		movl	20(%ebp), %edx		# [edx = this] - must be valid, so
		movl	NEXT(%edx), %esi	# [esi = next] - must also be valid
		movl	mp1_missile_list, %edi	# [edi = head]
		pushl	$1
		pushl	$' '
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# - erase: draw(y, x, ' ', shift=True)
		addl	$16, %esp
		cmpl	%edx, %edi
		je	.headelt		# then "prev" is ignored
		movl	%esi, NEXT(%ecx)	# - delete (middle): ecx (-> edx) -> esi
		jmp	.rmvdone
	.headelt:
		movl	%esi, mp1_missile_list	# - delete (first): MLIST (-> edx) -> esi
	.rmvdone:
		pushl	%edx
		call	mp1_free		# - free(*this)
		addl	$4, %esp
		popl	%edi
		popl	%esi
		popl	%edx
		popl	%ecx
		popl	%ebx
		popl	%eax			# [callee restore]
		leave
		ret

update_missiles:
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%ebx			# [callee save]
		pushl	%esi
		pushl	%edi			# 	eax | ebx | ecx | edx | esi | edi
		movl	mp1_missile_list, %edi	# 	                        prev  this
	.traverse:
		cmpl	$0, %edi		# (last element?)
		je	.travdone		# STEP 0: now exploding?
		movl	Y(%edi), %eax		# 	 y
		movl	X(%edi), %ebx		# 	       x
		cmpl	$0, EXPLODED(%edi)
		jne	.exploding		# 	(otherwise, update coords)
		movl	VY(%edi), %ecx		# 	            dy
		movl	VX(%edi), %edx		# 	                  dx
		addl	%ecx, %eax		# 	 y'
		addl	%edx, %ebx		# 	       x'
		xorl	%ecx, %ecx		# 	            flag
		cmpl	$0x00000000, %eax	# STEP 1: out of range?
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
		je	.inrange		# 	(handle OOR cases)
		pushl	NEXT(%edi)
		pushl	%edi
		pushl	%esi
		pushl	%ebx
		pushl	%eax
		call	remove_missile		# 	remove(y', x', *prev, *this)
		addl	$16, %esp
		popl	%edi			# 	(prevents reading from NUL)
		jmp	.travnextprm
	.inrange:				# STEP 2: about to explode?
		movl	DEST_Y(%edi), %ecx	# 	             yD
		movl	%eax, %edx		# 	                   y'
		shrl	$16, %edx		# 	                   y'H
		cmpl	%ecx, %edx
		jne	.flying
		movl	DEST_X(%edi), %ecx	# 	             xD
		movl	%ebx, %edx		# 	                   x'
		shrl	$16, %edx		# 	                   x'H
		cmpl	%ecx, %edx
		jne	.flying
	.exploding:
		pushl	%eax			# 	eax will be overwritten by "explode"
		pushl	%edi
		call	missile_explode		# 	explode(*this)
		popl	%edi
		cmpl	$0, %eax
		je	.expsilent		# 	(notify unless "explode" returns 0)
		call	mp1_notify_user
	.expsilent:
		popl	%eax
		decl	EXPLODED(%edi)
		cmpl	$0, EXPLODED(%edi)
		jne	.expcont		# 	(otherwise, keep exploding)
		pushl	NEXT(%edi)
		pushl	%edi
		pushl	%esi
		pushl	%ebx
		pushl	%eax
		call	remove_missile		# 	remove(y', x', *prev, *this)
		addl	$16, %esp
		popl	%edi
		jmp	.travnextprm
	.expcont:
		pushl	$0
		pushl	$' '
		pushl	DEST_X(%edi)
		pushl	DEST_Y(%edi)
		call	draw_pixel
		addl	$16, %esp
		pushl	$1
		pushl	$EXPLOSION
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# 	draw(y', x', EXPLOSION, shift=True)
		addl	$16, %esp
		jmp	.travnext
	.flying:				# STEP 3: flying?
		movl	Y(%edi), %ecx		# 	 y' |  x' |     |     | prev| this
		movl	X(%edi), %edx		# 	             y     x
		movl	%eax, Y(%edi)
		movl	%ebx, X(%edi)		# 	(store new coords)
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
		cmpl	$0, %esi
		je	.travdone
		movl	NEXT(%esi), %edi
	.travnextprm:				# (for post-removal cases)
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
		movl	$40, crosshairs_x
		movl	$12, crosshairs_y
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret

mp1_ioctl_addmissile:
		pushl	%ecx			# [caller save]
		pushl	%edx
		pushl	$36
		call	mp1_malloc		# eax = malloc(36)
		addl	$4, %esp
		cmpl	$0, %eax		# malloc failed?
		je	.mallocfail
		movl	%eax, %edi		# [edi = "to" addr]
		pushl	$36
		pushl	8(%ebp)			# ["from" addr]
		pushl	%edi
		call	mp1_copy_from_user	# copy(to, from, 36)
		addl	$12, %esp
		cmpl	$0, %eax		# memcpy failed?
		jne	.u2kcpyfail
		movl	mp1_missile_list, %eax
		movl	%eax, NEXT(%edi)	# update linked list
		movl	%edi, mp1_missile_list
		movl	$0, %eax		# return 0
		popl	%edx
		popl	%ecx			# [caller restore]
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret
	.u2kcpyfail:
		pushl	%eax
		call	mp1_free
		addl	$4, %esp
	.mallocfail:
		movl	$-1, %eax		# return -1
		popl	%edx
		popl	%ecx			# [caller restore]
		popl	%edi
		popl	%esi
		popl	%ebx			# [callee restore]
		leave
		ret

mp1_ioctl_movexhairs:
		movl	8(%ebp), %eax		# eax | ebx | ecx | edx
		movl	8(%ebp), %ebx
		sarl	$16, %eax		# dy
		andl	$0xFFFF, %ebx		#       dx
		movswl	%bx, %ebx		# (a little tricky here...)
		movl	crosshairs_y, %ecx	#              y
		movl	crosshairs_x, %edx 	#                    x
		addl	%ecx, %eax		#  y'
		addl	%edx, %ebx		#        x'
		cmpl	$0, %eax
		jge	.yge0			# ensure y' >= 0
		movl	$0, %eax
	.yge0:
		cmpl	$24, %eax
		jle	.yle24			# ensure y' <= 24
		movl	$24, %eax
	.yle24:
		cmpl	$0, %ebx
		jge	.xge0			# ensure x' >= 0
		movl	$0, %ebx
	.xge0:
		cmpl	$79, %ebx
		jle	.xle79			# ensure x' <= 79
		movl	$79, %ebx
	.xle79:					#  y' |  x' |  y  |  x
		cmpl	%eax, %ecx
		jne	.redrawxhr
		cmpl	%ebx, %edx
		jne	.redrawxhr
		jmp	.nredrawxhr
	.redrawxhr:
		pushl	$0
		pushl	$' '
		pushl	%edx
		pushl	%ecx
		call	draw_pixel		# draw(y, x, ' ', shift=False)
		addl	$12, %esp
		pushl	$'+'
		pushl	%ebx
		pushl	%eax
		call	draw_pixel		# draw(y', x', '+', shift=False)
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
		lea	mp1_score, %esi		# [esi = "from"] (kernel label)
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
		movl	mp1_missile_list, %esi	# similar to ".traverse"
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

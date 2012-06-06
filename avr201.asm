;**** A P P L I C A T I O N   N O T E   A V R 2 0 1 ***************************
;*
;* Title		: 16bit multiply routines using hardware multiplier
;* Version		: V2.0
;* Last updated		: 10 Jun, 2002
;* Target		: Any AVR with HW multiplier
;*
;* Support email	: avr@atmel.com
;*
;* DESCRIPTION
;* 	This application note shows a number of examples of how to implement
;*	16bit multiplication using hardware multiplier. Refer to each of the
;*	funtions headers for details. The functions included in this file
;*	are :
;*
;*	mul16x16_16	- Multiply of two 16bits numbers with 16bits result.
;*	mul16x16_32	- Unsigned multiply of two 16bits numbers with 32bits
;*			  result.
;*	mul16x16_24	- Unsigned multiply of two 16bits numbers with 24bits
;*			  result.
;*	muls16x16_32	- Signed multiply of two 16bits numbers with 32bits
;*			  result.
;*	muls16x16_24	- Signed multiply of two 16bits numbers with 24bits
;*			  result.
;*	mac16x16_24	- Signed multiply accumulate of two 16bits numbers
;*			  with a 24bits result.
;*	mac16x16_32	- Signed multiply accumulate of two 16bits numbers
;*			  with a 32bits result.
;*	fmuls16x16_32	- Signed fractional multiply of two 16bits numbers
;*			  with 32bits result.
;*	fmac16x16_32	- Signed fractional multiply accumulate of two 16bits
;*			  numbers with a 32bits result.
;*
;******************************************************************************



;******************************************************************************
;*
;* FUNCTION
;*	mul16x16_16
;* DECRIPTION
;*	Multiply of two 16bits numbers with 16bits result.
;* USAGE
;*	r17:r16 = r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	9 + ret
;*	Words :		6 + ret
;*	Register usage: r0, r1 and r16 to r23 (8 registers)
;* NOTE
;*	Full orthogonality i.e. any register pair can be used as long as
;*	the result and the two operands does not share register pairs.
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

mul16x16_16:
	mul	r22, r20		; al * bl
	movw	r17:r16, r1:r0
	mul	r23, r20		; ah * bl
	add	r17, r0
	mul	r21, r22		; bh * al
	add	r17, r0
	ret


;******************************************************************************
;*
;* FUNCTION
;*	mul16x16_32
;* DECRIPTION
;*	Unsigned multiply of two 16bits numbers with 32bits result.
;* USAGE
;*	r19:r18:r17:r16 = r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	17 + ret
;*	Words :		13 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;* NOTE
;*	Full orthogonality i.e. any register pair can be used as long as
;*	the 32bit result and the two operands does not share register pairs.
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

mul16x16_32:
	clr	r2
	mul	r23, r21		; ah * bh
	movw	r19:r18, r1:r0
	mul	r22, r20		; al * bl
	movw	r17:r16, r1:r0
	mul	r23, r20		; ah * bl
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	mul	r21, r22		; bh * al
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	ret


;******************************************************************************
;*
;* FUNCTION
;*	mul16x16_24
;* DECRIPTION
;*	Unsigned multiply of two 16bits numbers with 24bits result.
;* USAGE
;*	r18:r17:r16 = r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	14 + ret
;*	Words :		10 + ret
;*	Register usage: r0 to r1, r16 to r18 and r20 to r23 (9 registers)
;* NOTE
;*	Full orthogonality i.e. any register pair can be used as long as
;*	the 24bit result and the two operands does not share register pairs.
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

mul16x16_24:
	mul		r23, r21		; ah * bh
	mov		r18, r0
	mul		r22, r20		; al * bl
	movw	r17:r16, r1:r0
	mul		r23, r20		; ah * bl
	add		r17, r0
	adc		r18, r1
	mul		r21, r22		; bh * al
	add		r17, r0
	adc		r18, r1
	ret


;******************************************************************************
;*
;* FUNCTION
;*	muls16x16_32
;* DECRIPTION
;*	Signed multiply of two 16bits numbers with 32bits result.
;* USAGE
;*	r19:r18:r17:r16 = r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	19 + ret
;*	Words :		15 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;* NOTE
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

muls16x16_32:
	clr	r2
	muls	r23, r21		; (signed)ah * (signed)bh
	movw	r19:r18, r1:r0
	mul	r22, r20		; al * bl
	movw	r17:r16, r1:r0
	mulsu	r23, r20		; (signed)ah * bl
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	mulsu	r21, r22		; (signed)bh * al
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	ret


;******************************************************************************
;*
;* FUNCTION
;*	muls16x16_24
;* DECRIPTION
;*	Signed multiply of two 16bits numbers with 24bits result.
;* USAGE
;*	r18:r17:r16 = r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	14 + ret
;*	Words :		10 + ret
;*	Register usage: r0 to r1, r16 to r18 and r20 to r23 (9 registers)
;* NOTE
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

muls16x16_24:
	muls	r23, r21		; (signed)ah * (signed)bh
	mov		r18, r0
	mul		r22, r20		; al * bl
	movw	r17:r16, r1:r0
	mulsu	r23, r20		; (signed)ah * bl
	add		r17, r0
	adc		r18, r1
	mulsu	r21, r22		; (signed)bh * al
	add		r17, r0
	adc		r18, r1
	ret


;******************************************************************************
;*
;* FUNCTION
;*	mac16x16_24
;* DECRIPTION
;*	Signed multiply accumulate of two 16bits numbers with
;*	a 24bits result.
;* USAGE
;*	r18:r17:r16 += r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	16 + ret
;*	Words :		12 + ret
;*	Register usage: r0 to r1, r16 to r18 and r20 to r23 (9 registers)
;*
;******************************************************************************

mac16x16_24:
	muls	r23, r21		; (signed)ah * (signed)bh
	add	r18, r0

	mul	r22, r20		; al * bl
	add	r16, r0
	adc	r17, r1
	adc	r18, r2

	mulsu	r23, r20		; (signed)ah * bl
	add	r17, r0
	adc	r18, r1

	mulsu	r21, r22		; (signed)bh * al
	add	r17, r0
	adc	r18, r1

	ret



;******************************************************************************
;*
;* FUNCTION
;*	mac16x16_32
;* DECRIPTION
;*	Signed multiply accumulate of two 16bits numbers with
;*	a 32bits result.
;* USAGE
;*	r19:r18:r17:r16 += r23:r22 * r21:r20
;* STATISTICS
;*	Cycles :	23 + ret
;*	Words :		19 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;*
;******************************************************************************

mac16x16_32:
	clr	r2

	muls	r23, r21		; (signed)ah * (signed)bh
	add	r18, r0
	adc	r19, r1

	mul	r22, r20		; al * bl
	add	r16, r0
	adc	r17, r1
	adc	r18, r2
	adc	r19, r2

	mulsu	r23, r20		; (signed)ah * bl
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	mulsu	r21, r22		; (signed)bh * al
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	ret

mac16x16_32_method_B:			; uses two temporary registers
					; (r4,r5), but reduces cycles/words
					; by 1
	clr	r2

	muls	r23, r21		; (signed)ah * (signed)bh
	movw	r5:r4,r1:r0

	mul	r22, r20		; al * bl

	add	r16, r0
	adc	r17, r1
	adc	r18, r4
	adc	r19, r5

	mulsu	r23, r20		; (signed)ah * bl
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	mulsu	r21, r22		; (signed)bh * al
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	ret


;******************************************************************************
;*
;* FUNCTION
;*	fmuls16x16_32
;* DECRIPTION
;*	Signed fractional multiply of two 16bits numbers with 32bits result.
;* USAGE
;*	r19:r18:r17:r16 = ( r23:r22 * r21:r20 ) << 1
;* STATISTICS
;*	Cycles :	20 + ret
;*	Words :		16 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;* NOTE
;*	The routine is non-destructive to the operands.
;*
;******************************************************************************

fmuls16x16_32:
	clr	r2
	fmuls	r23, r21		; ( (signed)ah * (signed)bh ) << 1
	movw	r19:r18, r1:r0
	fmul	r22, r20		; ( al * bl ) << 1
	adc	r18, r2
	movw	r17:r16, r1:r0
	fmulsu	r23, r20		; ( (signed)ah * bl ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	fmulsu	r21, r22		; ( (signed)bh * al ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	ret


;******************************************************************************
;*
;* FUNCTION
;*	fmac16x16_32
;* DECRIPTION
;*	Signed fractional multiply accumulate of two 16bits numbers with
;*	a 32bits result.
;* USAGE
;*	r19:r18:r17:r16 += (r23:r22 * r21:r20) << 1
;* STATISTICS
;*	Cycles :	25 + ret
;*	Words :		21 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;*
;******************************************************************************

fmac16x16_32:
	clr	r2

	fmuls	r23, r21		; ( (signed)ah * (signed)bh ) << 1
	add	r18, r0
	adc	r19, r1

	fmul	r22, r20		; ( al * bl ) << 1
	adc	r18, r2
	adc	r19, r2
	add	r16, r0
	adc	r17, r1
	adc	r18, r2
	adc	r19, r2

	fmulsu	r23, r20		; ( (signed)ah * bl ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	fmulsu	r21, r22		; ( (signed)bh * al ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	ret

fmac16x16_32_method_B:			; uses two temporary registers
					; (r4,r5), but reduces cycles/words
					; by 2
	clr	r2

	fmuls	r23, r21		; ( (signed)ah * (signed)bh ) << 1
	movw	r5:r4,r1:r0
	fmul	r22, r20		; ( al * bl ) << 1
	adc	r4, r2

	add	r16, r0
	adc	r17, r1
	adc	r18, r4
	adc	r19, r5

	fmulsu	r23, r20		; ( (signed)ah * bl ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2
	fmulsu	r21, r22		; ( (signed)bh * al ) << 1
	sbc	r19, r2
	add	r17, r0
	adc	r18, r1
	adc	r19, r2

	ret


;**** End of File ****


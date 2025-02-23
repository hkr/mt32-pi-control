    .text
    .global _mcdrv_version
	.global _mcdrv_release
	.global _mcdrv_send_midi

_mcdrv_release:
	moveq.l #0, d0
	trap #4
	rts

_mcdrv_version:
	move.l #0, a0
	move.l a0,-(sp)
	dc.w    $FF20             * Supervisor mode
	addq.l #4,sp
    movea.l $24*4.w, a0
    moveq.l #-1, d0           * Default return value: -1	
    cmpa.l  #0xFFFFFF, a0     * Check if the vector is valid
    bhi     version_ret       * If higher, jump to version_ret
    cmpi.l  #'-MCD', -12(a0)  * Check for resident header
    bne     version_ret
    cmpi.l  #'RV0-', -8(a0)   * Check version identifier
    bne     version_ret
    move.l  -(a0), d0         * Retrieve version number
version_ret:	
    rts

_mcdrv_send_midi:
	move.l 8(sp), a0
	move.l 4(sp), d1
	move.l #38, d0
	trap #4
	move.l a0, d0
	rts
	
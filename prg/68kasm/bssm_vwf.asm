*********************************************************************
*          VWF font hack for Bishoujo Senshi Sailor Moon            *
*                                                                   *
* Code currently occupies 18F800-18FC00                             *
*********************************************************************
  
* 32-bit offset of current printing position in VRAM (from the base
* position at EEAA). Can be treated as a 16-bit value for our purposes
* (probably).
textPrintVramPos        equ $FFFFEE9C

* 32-bit offset of base printing position in VRAM, preformatted as a
* VDP DMA command.
baseTextPrintVramPos    equ $FFFFEEAA
* ... except of course we can't do a 32-bit access at that location,
* so here's the same thing split into words
* oh wait the 68000 doesn't have this restriction.
* ARM on the brain.
hiBaseTextPrintVramPos  equ $FFFFEEAA
loBaseTextPrintVramPos  equ $FFFFEEAC

* The command executed to start the DMA (sum of previous two values)
textDmaCommand          equ $FFFFEE94

* Offset in font data of character to print
printingCharacterOffset equ $FFFFEE8C

* ROM offset of font pattern data (4 patterns / 128 bytes per character)
fontPatternData         equ $00188970

* ROM offset of font width table
fontWidthData           equ $0018FC00

* RAM offset of 64-byte font tile write buffer.
* Note: This is not guaranteed to be initialized at the start of each
* scene! Have the script terminator opcode clear this and reset the
* printing position to be safe.
* 
* In fact, this area of RAM is used for a 68k->VDP copy, intended to zero
* out the nametables. It needs to be cleared at the termination of the
* script or visual artifacts will occur.
charPutBuf              equ $FFFFD400

cpSize                  equ $40
cpClrSize               equ $F

* Current pixel-pair position in charPutBuf
*charPutPos              equ $FFFFD440

buf0Addr                equ $FFFFD400
buf1Addr                equ $FFFFD440
buf2Addr                equ $FFFFD480
bufDirtyArray           equ $FFFFD4C0   * Byte array where nonzero entries
                                        * indicate buffers needing redraw
bufPos                  equ $FFFFD4D0   * Position in active buffer
lastActivBuf            equ $FFFFD4D2   * Index number of last active buffer
activBuf                equ $FFFFD4D4   * Index number of current buffer

  *********************************************************************
  * set up subtitles for intro voice
  *********************************************************************

voiceSubsGrpPtr         equ $3FF000
voiceSubsGrpDmaCmd      equ $40000080   * VRAM 0000 = patterns start

voiceSubsMapPtr         equ $3FF004
voiceSubsMapDmaCmd      equ $40000083   * VRAM C000 = plane A start
voiceSubsMapW           equ 64
voiceSubsMapH           equ 32
voiceSubsMapSize        equ $1000
kosinskiDecmp           equ $2D62

  org $25EC
  jsr introVoiceSetup   * call our new setup routine

  org $3FF100

  *********************************************************************
  * generic DMA transfer routine
  *
  * a0 = source
  *
  * d0 = size
  * d5 = dst DMA command
  *********************************************************************

  doDmaGeneric:

  movem.l   d0-d7/a0-a6,-(a7)

  * get mode set register 2 command from RAM FFFD02
  move.w  ($FFFFFD02),d6
  * set bit 4 (dma enable)
  bset    #4,d6
  
  * prepare for VDP control port access
  lea     $C00004,a6
  
  * set dma length
  
  * halve source size
  lsr.w #1,d0
  
  * low byte
  move.w #$9300,d1
  move.b d0,d1
  move.w d1,(a6)
  
  * high byte
  lsr.w #8,d0
  move.w #$9400,d1
  move.b d0,d1
  move.w d1,(a6)
  
  * set up high byte of dma source commands
  move.w  #$9500,d1
  move.w  #$9600,d2
  move.w  #$9700,d3
  
  * get address of source data
*  lea     charPutBuf,a0
  
  * stop z80 (in preparation for 68000 freeze during dma)
  move.w  #$100,$A11100
  * set auto data increment to 2
  move.w  #$8F02,(a6)
  * write mode set register 2
  move.w  d6,(a6)
  
  * halve source address to match DMA format
  move.l  a0,d0
  lsr.l   #1,d0
  
  * copy each byte to corresponding command and execute it
  
  * low
  move.b  d0,d1
  move.w  d1,(a6)
  
  * middle
  lsr.l   #8,d0
  move.b  d0,d2
  move.w  d2,(a6)
  
  * high
  lsr.l   #8,d0
  move.b  d0,d3
  * Mask off high bit (mode select)
  and.b   #$7F,d3
  move.w  d3,(a6)
  
  * trigger DMA
  move.l d5,(a6)
  
  *** This code is probably not necessary, but it's how the original game does
  *** it and I'll take cargo cult programming over obscure hardware bugs
  * get status register
  ctrWaitLoop:
    move.w  (a6),d5
    * loop until dma finished (if not a 68k copy)
    btst    #1,d5
    bne.s   ctrWaitLoop
  
  * restore original mode set register 2
  move.w  $FFFFFD02,(a6)
  
  *** This code is probably not necessary either
  * do dma again ... except it won't work because we just disabled it
*  move.w  $FFFFEE94,(a6)
*  move.w  $FFFFEE96,(a6)
  * re-copy the first word of the already-transferred pattern data (first 4
  * pixels) to the VDP data port after performing the dma
  * isn't this only a problem with the sega cd?
*  move.w  (a0),-4(a6)
  
  * restart z80
  move.w  #0,$A11100
  
  movem.l   (a7)+,d0-d7/a0-a6
  rts
  
  introVoiceSetup:
    
    **************************************
    * load subtitle graphics
    **************************************
    
    * decompress the subtitle graphics
    move.l voiceSubsGrpPtr,a0
    move.l #$FF0000,a1
    jsr kosinskiDecmp
    
    * DMA to VRAM
    move.l #$FF0000,a0
    move.l a1,d0            * get end address from decompression
    subi.l #$FF0000,d0    * subtract base address to get size
    move.l #voiceSubsGrpDmaCmd,d5
    jsr doDmaGeneric
    
    * decompress the subtitle map
    move.l voiceSubsMapPtr,a0
    move.l #$FF0000,a1
    jsr kosinskiDecmp
    
    * DMA to VRAM
    move.l #$FF0000,a0
    move.l #voiceSubsMapSize,d0
    move.l #voiceSubsMapDmaCmd,d5
    jsr doDmaGeneric
    
    **************************************
    * set up palette
    **************************************
  
    * the routine that uploads the palettes during this scene is
    * apparently bugged (writes longs instead of words), so palette
    * offsets have to be doubled
    
    * palette 0 color 1 = almost-black
*    move.w #$0222,$FFFFFB86
    
    * palette 0 color 15 = white
*    move.w #$0EEE,$FFFFFBBE
    
    * palette 0 color 1 = almost-black
    move.w #$0222,$FFFFFB82
    
    * palette 0 color 2 = white
*    move.w #$0EEE,$FFFFFB9E
    move.w #$0EEE,$FFFFFB84
    
    **************************************
    * finish up
    **************************************
    
    * fulfill call that we replaced to get here
    jmp $F9D4

  *********************************************************************
  * alter load address of game over/continue graphics
  * by changing DMA transfer command
  *********************************************************************
  
  * continue
  org $15FD8
  * orig: 70000082 = B000
  * new = A800
  dc.l $68000082
  
  * game over
  org $15FE2
  * orig: 76000082 = B600
  * new = B400
  dc.l $74000082
  

  *********************************************************************
  * extend length of credits
  *********************************************************************

  org $15874
*  cmpi.l #$00FF3000,($FFFFEEEA)
  cmpi.l #$00FF3D00,($FFFFEEEA)
  
  *********************************************************************
  * Completely irrelevant to anything else in this file:
  * Switch the options screen so "left" lowers difficulty and "right"
  * increases it (instead of vice versa)
  *********************************************************************

  org       $12A22
  beq       $1301A
  
  org       $12A54
  beq       $12FFC
  
  * similarly irrelevant: increase numbers of tiles loaded for credits
  org       $15904
  * original value is $2D00 = 2D0 tiles
  * increase to $3800 for $380 tiles
  move.w    #$3800,d7
  
  * move title screen sprite cursor 8px to the right to match new
  * centering
  org       $16C32
  dc.b      $00,$E8
  org       $16C3E
  dc.b      $00,$E8
  
  *********************************************************************
  * Call our buffer update code
  *********************************************************************
  
  ************
  * Intro
  ************
  
  * standard print
  org       $B88C
  jsr       bufUpdate
  
  * linebreak
  org       $B8CC
  jsr       initBuffers   * must clear existing content first
  jsr       bufUpdate
  * additionally, don't skip a character on the second line
  org       $B8B2
  move.l    #$7000000,$FFFFEE9C
  
  * box clear
  org       $B8FC
  jsr       injctClrBox
  nop
  
  * script initialization
  org       $B81E
  jsr       injctClrBox
  nop
  
  * end-of-script buffer clear
  org       $BA2E
  jsr       injctTr2
  
  ************
  * Gameplay
  ************
  
  * standard print
  org       $BC48
  jsr       bufUpdate
  
  * linebreak
  org       $BC88
  jsr       initBuffers   * must clear existing content first
  jsr       bufUpdate
  * additionally, don't skip a character on the second line
  org       $BC6E
  move.l    #$4000000,$FFFFEE9C
  
  * box clear
  org       $BCC0
  jsr       injctClrBox
  nop
  
  * script initialization
  org       $BBD0
  jsr       injctClrBox
  nop
  
  * end-of-script buffer clear
  org       $BDD8
  jmp       injctTr1
  nop

  *********************************************************************
  * Eliminate width updates in text script handlers
  *********************************************************************
  
  * Intro scripts
  org $BB14
  nop
  nop
  nop
  
  * Game scripts
  org $BE84
  nop
  nop
  nop

  *********************************************************************
  * Eliminate EE94 updates in text script handlers
  *********************************************************************
  
  * Game scripts
*  org $BE9A
*  nop
*  nop

  *********************************************************************
  * The game sometimes uses the "unused" portions of memory we've
  * co-opted for this hack as the source for DMA transfers, the
  * assumption being that this memory is zeroed and can therefore be
  * used to clear out VRAM.
  * We have to make sure we clean out this part of memory when that
  * happens.
  *********************************************************************

  org $FB82
  jsr dmaClrFix

  *********************************************************************
  * Disable region lockout
  *********************************************************************
  
  org $28DE
  nop
  nop

  *********************************************************************
  * VBlank injection point
  *********************************************************************

  org $C0FE
  
  jmp vwfHackStart
 
injectionRetPoint equ $C16C

*********************************************************************
* Hack code begins here
*********************************************************************

  org $18F800

  *********************************************************************
  * Injection code for "virtual DMA clear" fix
  *********************************************************************
  
dmaClrFix:
  
  * clear buffers
  jsr       initBuffers
  
  * make up work
  move.l   #$50000083,d5
  
  rts

  *********************************************************************
  * Injection code for text box clear routines
  *********************************************************************
  
injctClrBox:
  
  * make up work
  move.l    #0,$FFFFEE9C
  
  * clear buffers
  jsr       initBuffers
  
  rts

  *********************************************************************
  * Injection code for script terminator routines
  *********************************************************************
  
injctTr1:
  
  * clear buffers
  jsr       initBuffers
  
  * make up work
  subq.b    #1,$FFFFEEB6
  bpl       injcTrJ1
  jmp       $BDE0
  
injcTrJ1:
  jmp       $BE22
  
injctTr2:
  
  * clear buffers
  jsr       initBuffers
  
  * make up work
  move.w    #0,$FFFFEE86
  
  rts

  *********************************************************************
  * Interrupt handler modifications
  *********************************************************************
  
vwfHackStart:

  * Initialize the VRAM put command
  jsr     setVramCom
  
  * Starting at the index of the last active buffer, check each buffer for
  * dirtiness, and transfer it to VRAM if dirty.
  * As soon as we find a clean buffer, we're done.
  * At most three checks are necessary (for the three buffers).
  
  * Get last active buffer index
  move    (activBuf),d2
  move    (lastActivBuf),d1
  move    d1,d0

  *********************************************************************
  * Check 1
  *********************************************************************
  
  * Is this buffer dirty?
  bsr     isBufDirty
  
  * No: we're done
  cmp     #0,d0
  beq     vwfchkDone
  
  * Yes: mark as clean and flush it
  move    d1,d0
  bsr     cleanBuffer
  bsr     transCharBuf
  
*  bsr     clearCharBuf

  * If this buffer is still active, we'll write more data to it and will
  * need to flush it to this same position again.
  * Otherwise, move to the next VRAM position.
  cmp     d1,d2
  beq     vwfchk2
    bsr     incVramPos

  *********************************************************************
  * Check 2
  *********************************************************************
vwfchk2:

  * Get the index of the next buffer in the sequence
  move    d1,d0
  bsr     getNextBuf
  * Update register
  move    d0,d1
  
  * Is this buffer dirty?
  bsr     isBufDirty
  
  * No: we're done
  cmp     #0,d0
  beq     vwfchkDone
  
  * Yes: mark as clean and flush it
  move    d1,d0
  bsr     cleanBuffer
  bsr     transCharBuf
  
  * If this buffer is still active, we'll write more data to it and will
  * need to flush it to this same position again.
  * Otherwise, move to the next VRAM position.
  cmp     d1,d2
  beq     vwfchk3
    bsr     incVramPos

  *********************************************************************
  * Check 3
  *********************************************************************
vwfchk3:

  * Get the index of the next buffer in the sequence
  move    d1,d0
  bsr     getNextBuf
  * Update register
  move    d0,d1
  
  * Is this buffer dirty?
  bsr     isBufDirty
  
  * No: we're done
  cmp     #0,d0
  beq     vwfchkDone
  
  * Yes: mark as clean and flush it
  move    d1,d0
  bsr     cleanBuffer
  bsr     transCharBuf
  
  * If this buffer is still active, we'll write more data to it and will
  * need to flush it to this same position again.
  * Otherwise, move to the next VRAM position.
  * 
  * this will never happen (if we get to the third buffer at all, it must
  * be active)
*  cmp     d1,d2
*  beq     vwfchkDone
*    bsr     incVramPos

  *********************************************************************
  * Finish up
  *********************************************************************
vwfchkDone:
  
  * Update last active buffer position to current
  move    (activBuf),d1
  move    d1,(lastActivBuf)
  
  * "Acknowledge" transfer
  * (the original game does not do this; it will continue transferring
  * the same character to the same VRAM position every frame until the
  * script moves on to the next command)
  move    #0,$FFFFEE82
 
  * Done: return to normal logic
*  movem  (a7)+,d0-d7/a0-a6
  jmp injectionRetPoint

  *********************************************************************
  * bufUpdate
  * 
  * Writes new character data to the buffers and updates state variables
  * accordingly.
  * 
  * Arguments:
  *
  * Trashes:
  *   Everything
  *
  *********************************************************************

bufUpdate:

  movem.l a0,-(a7)
  
  * Get the offset of the target font character
  move.l  (printingCharacterOffset),d5
  move.l  d5,d0
  * Subtract off the base position
  move.l  #fontPatternData,d1
  sub.l   d1,d0
  move.l  d0,d6
  
  * Convert offset to width table index (divide by 64)
  lsr     #6,d6
  
  * Look up width entry address
  movea.l #fontWidthData,a0
  add.l   d6,a0
  
  * Get pixel width of character from width entry (byte 0)
  move.b  (a0),d6
  ext.w   d6
  
  * Initialize D3: copy position, in pixels, in src
  move.l  #0,d3
  
  * get buffer putpos
  move    (bufPos),d4
  
  * At this point:
  * 
  * a0 = offset of width info struct
  * 
  * d3 = copy position (initialized to zero)
  * d4 = pixel pos in dst char buffer
  * d5 = pointer to target src character pattern data
  * d6 = pixel width of target character
  
  
  *****************
  * Transfer 1
  * This copies up to the next pattern-column boundary in dst VRAM.
  *****************
  
  * Transfer width: Number of pixels needed to reach the next dst
  * column boundary, or all of them if fewer.
  * Formula: 8 - (dstBasePos % 8)
  
  * get pixel offset in dst
  move    d4,d1
  * take modulo 8
  andi    #$0007,d1
  * subtract from 8
  move    #8,d0
  sub     d1,d0
  * if the result is greater than the actual number of pixels remaining,
  * just copy what's there
  cmp     d0,d6
  bgt     budL1
  move    d6,d0
  
budL1:

  * save transfer width
  move    d0,d7
  
  * Update count of pixel-pairs remaining
  sub     d0,d6
  
  * Clear active buffer if "new" (not yet written to)
  bsr     clrNewBuf
  
  * Get pointer to active buffer
  bsr     getActBuffer
  
  * Do the transfer
  bsr     transferCharData
  
  * Update curPixelPos
  add     d7,d3
  
  * Update dstVramPos (buffer putpos)
  add     d7,d4
  * Write updated buffer putpos
  move    d4,(bufPos)
  
  * Update buffers
  bsr updBufPrint
  
  * Read updated buffer putpos
  move    (bufPos),d4
  
  * If all columns have been transferred, we're done
  cmp     #0,d6
  beq     budDone
  
  *****************
  * Transfer 2
  * Copies 8 pixel columns, or all if there are fewer than that
  *****************
  
  * Default width: 8
  move    #8,d7
  * If fewer pixels than that remain, lower appropriately
  cmp     d6,d7
  ble     budL2
  move    d6,d7
  
budL2:
  
  * Update count of pixels remaining
  sub     d7,d6
  
  * Clear active buffer if "new" (not yet written to)
  bsr     clrNewBuf
  
  * Get pointer to active buffer
  bsr     getActBuffer
  
  * Do the transfer
  bsr     transferCharData
  
  * Update curPixelPos
  add     d7,d3
  
  * Update dstVramPos
  add     d7,d4
  * Write updated buffer putpos
  move    d4,(bufPos)
  
  * Update buffers
  bsr updBufPrint
  
  * Read updated buffer putpos
  move    (bufPos),d4
  
  * If all columns have been transferred, we're done
  cmp     #0,d6
  beq     budDone
  
  *****************
  * Transfer 3
  * Copies 8 pixel columns, or all if there are fewer than that
  *****************
  
  * Default width: 8
  move    #8,d7
  * If fewer pixel-pairs than that remain, lower appropriately
  cmp     d6,d7
  ble     budL3
  move    d6,d7
  
budL3:
  
  * Update count of pixels remaining
  sub     d7,d6
  
  * Clear active buffer if "new" (not yet written to)
  bsr     clrNewBuf
  
  * Get pointer to active buffer
  bsr     getActBuffer
  
  * Do the transfer
  bsr     transferCharData
  
  * Update curPixelPos
  add     d7,d3
  
  * Update dstVramPos
  add     d7,d4
  * Write updated buffer putpos
  move    d4,(bufPos)
  
  * Update buffers
  bsr     updBufPrint
  
  * Read updated buffer putpos
*  move    (bufPos),d4
  
  *****************
  * Finish up
  *****************
budDone:
    
  movem.l (a7)+,a0
  rts

  *********************************************************************
  * initBuffers
  * 
  * Initializes buffers and related variables.
  * 
  * Arguments:
  *
  * Trashes:
  *
  *********************************************************************
  
initBuffers:
  
  movem.l d0-d1/a0,-(a7)
  
  move.l  #0,d0
  move.l  #$3F,d1
  lea     buf0Addr,a0
  
intbLoop:
  move.l  d0,(a0)+
  dbra    d1,intbLoop
  
  movem.l (a7)+,d0-d1/a0
  
  rts

  *********************************************************************
  * getBuffer
  * 
  * Returns the address of a buffer by index.
  * 
  * Arguments:
  *   D0: Buffer index. Must be 0, 1, or 2.
  *
  * Trashes:
  *   D0
  *
  *********************************************************************

getBuffer:
  cmp     #0,d0
  bne     gabNext1
  
  move.l  #buf0Addr,d0
  rts
  
gabNext1:
  
  cmp     #1,d0
  bne     gabNext2
  
  move.l  #buf1Addr,d0
  rts
  
gabNext2:
  
  move.l  #buf2Addr,d0
  rts

  *********************************************************************
  * cleanBuffer
  * 
  * Marks a buffer as clean.
  * 
  * Arguments:
  *   D0: Buffer index.
  *
  * Trashes:
  *   A0
  *
  *********************************************************************
  
cleanBuffer:
  
  lea     bufDirtyArray,a0
  move.b  #$00,(a0,d0.w)
  
  rts
  

  *********************************************************************
  * markActBuffer
  * 
  * Marks active buffer as dirty.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0
  *   A0
  *
  *********************************************************************

markActBuffer:
  
  move    (activBuf),d0
  lea     bufDirtyArray,a0
  move.b  #$FF,(a0,d0.w)
  
  rts

  *********************************************************************
  * getActBuffer
  * 
  * Returns the address of the active buffer.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0
  *
  *********************************************************************
  
getActBuffer:
  
  move    (activBuf),d0
  bsr     getBuffer
  
  rts

  *********************************************************************
  * advBuffer
  * 
  * Advances to the next buffer.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0
  *
  *********************************************************************

advBuffer:
  
  move    (activBuf),d0
  bsr     getNextBuf
  move    d0,(activBuf)
  
  * Reset buffer put position
  move    #0,(bufPos)
  
  rts

  *********************************************************************
  * clrNewBuf
  * 
  * If the active buffer is "new" (putpos is zero), clears its contents.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0, D1
  *   A0
  *
  *********************************************************************

clrNewBuf:
  
  * Branch if buffer position not zero
  move    (bufPos),d0
  cmp     #0,d0
  bne     clnbDone
    * Clear the active buffer
    bsr     getActBuffer
    bsr     clrBuffer
  
clnbDone:

  rts

  *********************************************************************
  * clrBuffer
  * 
  * Clears the given buffer.
  * 
  * Arguments:
  *   D0: Pointer to buffer.
  *
  * Trashes:
  *   D0, D1
  *   A0
  *
  *********************************************************************

clrBuffer:
  
  move.l  d0,a0
  move.l  #0,d0
  move    #$F,d1
  
clrBufLoop:
  move.l  d0,(a0)+
  dbra    d1,clrBufLoop
  
  rts

  *********************************************************************
  * updBufPrint
  * 
  * Updates buffer states following a print.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0
  *
  *********************************************************************

updBufPrint:

  * Mark active buffer as dirty
  bsr     markActBuffer

  * If active buffer is full, advance to next one
  cmp     #8,(bufPos)
  blt     updbL1
    bsr     advBuffer
    
updbL1:
  
  rts

  *********************************************************************
  * isBufDirty
  * 
  * Returns nonzero if the buffer with index D0 is dirty.
  * 
  * Arguments:
  *   D0: Buffer index (word).
  *
  * Trashes:
  *   D0
  *   A0
  *
  *********************************************************************

isBufDirty:

  lea     bufDirtyArray,a0
  move.b  (a0,d0.w),d0
  rts

  *********************************************************************
  * getNextBuf
  * 
  * Given a buffer index, returns the next buffer in sequence.
  * 0 -> 1 -> 2 -> 0 -> ...
  * 
  * Arguments:
  *   D0: Buffer index.
  *
  * Trashes:
  *   D0
  *
  *********************************************************************

getNextBuf:
  add     #1,d0
  cmp     #2,d0
  ble     gnb1
  
  move    #0,d0
  
gnb1:
  rts

  *********************************************************************
  * transCharBuf
  * 
  * Transfers the character buffer from RAM to VRAM.
  * Does not clear the buffer or alter the put positions.
  * 
  * Arguments:
  *   D0: Index of buffer to transfer.
  *
  * Trashes:
  *   
  *********************************************************************

transCharBuf:

  movem.l   d0-d7/a0-a6,-(a7)
  
  * Set up and execute a 0x40-byte (2-pattern) DMA transfer
  
  * get mode set register 2 command from RAM FFFD02
  move.w  ($FFFFFD02),d6
  * set bit 4 (dma enable)
  bset    #4,d6
  
  * prepare for VDP control port access
  lea     $C00004,a6
  
  * set up high byte of dma source commands
  move.w  #$9500,d1
  move.w  #$9600,d2
  move.w  #$9700,d3
  
  * get address of source data
  * TODO: REMOVE METODO: REMOVE METODO: REMOVE METODO: REMOVE METODO: REMOVE ME
  lea     charPutBuf,a0
  
  * stop z80 (in preparation for 68000 freeze during dma)
  move.w  #$100,$A11100
  * set auto data increment to 2
  move.w  #$8F02,(a6)
  * write mode set register 2
  move.w  d6,(a6)
  
  * set dma length to 0x40 (in halved DMA format)
  move.w  #$9320,(A6)
  move.w  #$9400,(A6)
  
  * get source data address
  bsr     getBuffer
  * move for re-copy
  move.l  d0,a0
*  move.l  #charPutBuf,d0
*  move.l  #$188af0,d0
  * halve to match DMA format
  lsr.l   #1,d0
  
  * copy each byte to corresponding command and execute it
  
  * low
  move.b  d0,d1
  move.w  d1,(a6)
  
  * middle
  lsr.l   #8,d0
  move.b  d0,d2
  move.w  d2,(a6)
  
  * high
  lsr.l   #8,d0
  move.b  d0,d3
  * Mask off high bit (mode select)
  and.b   #$7F,d3
  move.w  d3,(a6)
  
  * write FFEE94 and FFEE96 as-is to set dma destination and trigger it
  move.w  $FFFFEE94,(a6)
  move.w  $FFFFEE96,(a6)
  
  *** This code is probably not necessary, but it's how the original game does
  *** it and I'll take cargo cult programming over obscure hardware bugs
  * get status register
  ctrWLoop:
    move.w  (a6),d5
    * loop until dma finished (if not a 68k copy)
    btst    #1,d5
    bne.s   ctrWLoop
  
  * restore original mode set register 2
  move.w  $FFFFFD02,(a6)
  
  *** This code is probably not necessary either
  * do dma again ... except it won't work because we just disabled it
  move.w  $FFFFEE94,(a6)
  move.w  $FFFFEE96,(a6)
  * re-copy the first word of the already-transferred pattern data (first 4
  * pixels) to the VDP data port after performing the dma
  * isn't this only a problem with the sega cd?
  move.w  (a0),-4(a6)
  
  * restart z80
  move.w  #0,$A11100
  
  movem.l   (a7)+,d0-d7/a0-a6
  rts

  *********************************************************************
  * clearCharBuf
  * 
  * Clears the character buffer and resets the local put position.
  * The VRAM put position is not affected.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0, D1
  *   A0
  *
  *********************************************************************

clearCharBuf:
  
  * Zero the character buffer
*  lea     charPutBuf,a0
*  move    #cpClrSize,d1
*  move.l  #0,d0
*  ccbLoop:
*    move.l  d0,(a0)+
*    dbra    d1,ccbLoop
    
  * reset put position
*  move    d0,charPutPos
  
*  rts

  *********************************************************************
  * setVramCom
  * 
  * Resets the VRAM put command (EE94) from current values.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0, D1
  *
  *********************************************************************

setVramCom:

  * get current vram pos
  move.l  (textPrintVramPos),d0
  
  * get base command position
  * (treat as 32-bit, but since it isn't 32-bit aligned, we have to
  * read the halves individually)
*  move.l  (baseTextPrintVramPos),d1
  move.w  (hiBaseTextPrintVramPos),d1
  swap    d1
  move.w  (loBaseTextPrintVramPos),d1
  
  * add updated position
*  swap    d0
*  move.w  #0,d0
  add.l   d0,d1
  
  * save
  move.l  d1,(textDmaCommand)
  
  rts

  *********************************************************************
  * incVramPos
  * 
  * Advances the VRAM put position by a pattern column.
  * 
  * Arguments:
  *
  * Trashes:
  *   D0
  *
  *********************************************************************

incVramPos:

  movem   d1,-(a7)

  * get current vram pos
  * (word access intentional -- lower half isn't important for us)
  move.w  (textPrintVramPos),d0
  
  * add column width
  add.w   #cpSize,d0
  
  * save
  move.w  d0,textPrintVramPos
  
  * get base command position
  move.l  (baseTextPrintVramPos),d1
  
  * add updated position
  swap    d0
  move.w  #0,d0
  add.l   d0,d1
  
  * save
  move.l  d1,textDmaCommand
  
  movem   (a7)+,d1
  rts

  *********************************************************************
  * transferCharData
  * 
  * Transfers a specified number of pixel columns to a buffer.
  * 
  * Arguments:
  *   D0: Address of destination buffer.
  *   D3: srcPixPos -- Starting offset, in pixels, in source pattern
  *       data.
  *   D5: Pointer to base position of source character's pattern data.
  *   D7: Width, in pixels, of transfer.
  *
  * Trashes:
  *   D0, D1, D2
  *
  *********************************************************************
  
transferCharData:

  movem.l d4/d6,-(a7)
  
  * Save destination address
  move.l  d0,d6
  
  * Get pixel putpos in dst
  move    (bufPos),d4
  ext.l   d4
  * Convert pixel to absolute offset
  move.l  d4,d0
  bsr     pixToVramPos
  * Add buffer base offset
  add.l   d6,d0
  * Save to register
  move.l  d0,a1
  
  * The transfer is split into two parts in order to handle cases where
  * the full transfer straddles a pattern-column boundary in the source
  * data.
  
  * First transfer width: 8 - (srcPixelPos % 8)
  move    d3,d1
  and     #$0007,d1
  move    #8,d2
  sub     d1,d2
  * if calculated size exceeds remaining columns, only use those left
  cmp     d7,d2
  ble     tcBra1
  move    d7,d2
tcBra1:
  
  ext.l   d2
  
  * Derive pointer to the target source character data
  * convert srcPixPos to VRAM format
  move.l  d3,d0
  bsr     pixToVramPos
  * get base position in source and add offset
  move.l  d5,d1
  add.l   d0,d1
  
  * a1 = dst pointer
  * d1 = source pointer
  * d2 = firstTransferWidth
  * d3 = srcPixPos
  * d4 = dstPixPos
  
  bsr colTransferCharData
  
  * Advance src pos (may cross pattern-column boundary!!)
  * Corrections are applied below.
  add.l   d2,d1
  
  * Advance dst pos (should never cross boundary)
  add.l   d2,d4
  
  * Second transfer width: (transferWidth - firstTransferWidth)
  move    d7,d0
  sub     d2,d0
  move    d0,d2
  
  ext.l   d2
  
  * if no data remains to be transferred, we're done
  cmp     #0,d2
  beq     tcdDone
  
  * if a second transfer is necessary, we need to advance to the next
  * source pattern column
  add.l   #60,d1
  
  bsr colTransferCharData
  
tcdDone:
  movem.l (a7)+,d4/d6
  rts

  *********************************************************************
  * colTransferCharData
  * 
  * Transfers a specified number of pixel columns to target.
  * 
  * Arguments:
  *   A1: Destination pointer.
  *   D1: Source pointer.
  *   D2: Transfer width in pixels.
  *   D3: Starting offset, in pixels, in source data.
  *   D4: Target offset, in pixels, in destination.
  *
  * Trashes:
  *   D0
  *   A0, A1
  *
  *********************************************************************
  
colTransferCharData:

  movem.l a2-a4/d1-d7,-(a7)
  
  * prep source pointer
  move.l  d1,a0
  
  * prep destination pointer copy
  move.l  a1,a2
  
  * copy all rows
  move    #15,d0
colTrLoop:
  
  * save initial src/dst pixel positions
*  movem.l d3-d4,-(a7)
  move.l  d3,a3
  move.l  d4,a4
  
*  move.l  d4,a1
  move.l  a2,a1
  
  * copy all pixels
  move    d2,d5
  sub     #1,d5
  colTrILoop:
    
    colTrIDoT:
      
    * get source byte
*    move.b    (a0),d6
    
    * fetch a nybble based on parity of source pixel pos
    * even (first) = high nybble
    * odd (second) = low nybble
    btst    #0,d3
    beq     srcParEven
    
    srcParOdd:
      * get source byte (and advance position)
      move.b    (a0)+,d6
      * low nybble only
      and.b     #$0F,d6
      
      bra srcParDone
      
    srcParEven:
      * get source byte (and do not advance position; next nybble to
      * follow)
      move.b    (a0),d6
      * high nybble only
      and.b     #$F0,d6
      * shift to low position
      lsr.b     #4,d6
    
    srcParDone:
    
    * get destination byte
    move.b      (a1),d7
    
    * check destination parity
    btst    #0,d4
    beq     dstParEven
      
    * target low nybble of dst
    dstParOdd:
      
      * clear low nybble of dst
      * (not needed -- cleared before transfer)
*      and.b   #$F0,d7
      * OR with src
      or.b    d6,d7
      
      * move to destination (and advance position)
      move.b    d7,(a1)+
      
      bra dstParDone
    
    * target high nybble of dst
    dstParEven:
      
      * clear high nybble of dst
      * (not needed -- cleared before transfer)
*        and.b   #$0F,d7
      * shift src to high nybble
      lsl.b    #4,d6
      * OR with src
      or.b    d6,d7
      
      * move to destination (and do not advance position; low nybble to
      * follow)
      move.b    d7,(a1)
      
    dstParDone:
    
    * increment pixel positions
    add       #1,d3
    add       #1,d4
      
    * get byte
*    move.b  (a0)+,d0
    * copy to destination
*    move.b  d0,(a1)+
    
    * iterate over pixels in row
    dbra    d5,colTrILoop
  
  * move to next source row
  add.l   #4,d1
  move.l  d1,a0
  * move to next dst row
  add.l   #4,a2
    
  * reset pixel positions
*  movem.l (a7)+,d3-d4
  move.l  a3,d3
  move.l  a4,d4
  
  * iterate over rows
  dbra    d0,colTrLoop
  
  movem.l (a7)+,a2-a4/d1-d7
  rts

  *********************************************************************
  * pixToVramPos
  * 
  * Converts a pixel column value to tile-address format.
  * 
  * Arguments:
  *   D0: A pixel column value
  *
  * Trashes:
  *   D0, D1
  *
  *********************************************************************
  
pixToVramPos:
  
  * save original value
  move.l d0,d1
  
  * result = ((x / 8) * 8 * 16) + ((x mod 8) / 2)
  lsr.l  #3,d0
  lsl.l  #7,d0
  andi.l #7,d1
  lsr.l  #1,d1
  add.l  d1,d0
  rts

  *********************************************************************
  * vramToPixPos
  * 
  * Converts a tile-address value to pixel column format.
  * 
  * Arguments:
  *   D0: A tile-address value
  *
  * Trashes:
  *   D0, D1
  *
  *********************************************************************
  
*vramToPixPos:
  
  * save original value
*  move.l d0,d1
  
  * result = (x / 8) + ((x mod 4) * 2)
*  lsr.l  #4,d0
*  andi.l #3,d1
*  lsl.l  #1,d1
*  add.l  d1,d0
*  rts
  
  

KOSCMP="kens-1.5a1/build/src/tools/koscmp"

#KOSCOUNTER="0"

dokoscmp() {
  $KOSCMP -v -x $1 bssm.md $2$1.bin
#  $KOSCMP -x $1 bssm.md $2/$KOSCOUNTER.bin
#  KOSCOUNTER=$(($KOSCOUNTER+1))
}

# Intro part 1 (moon)
dokoscmp 0x1b0000 gfx/
dokoscmp 0x1b0ec0 gfx/
dokoscmp 0x1b1020 gfx/
dokoscmp 0x1b1c60 gfx/
dokoscmp 0x1b32a0 gfx/
dokoscmp 0x1b4c00 gfx/
dokoscmp 0x1b6e90 gfx/
dokoscmp 0x1b6ab0 gfx/
dokoscmp 0x1bdef0 gfx/
dokoscmp 0x1b5f50 gfx/
dokoscmp 0x1bdef0 gfx/

# Intro part 2 ("silhouette" 1)
dokoscmp 0x1b6150 gfx/
dokoscmp 0x1b61e0 gfx/

# Intro part 3 (castle 1)
dokoscmp 0x1b5f50 gfx/

# Intro part 4 ("silhouette" 2)
#dokoscmp 0x1b6150 gfx/
dokoscmp 0x1b63b0 gfx/

# Intro part 5 (castle 2)
dokoscmp 0x1b6ab0 gfx/

# Intro part 6 (eyes open)
#dokoscmp 0x1b63b0 gfx/

# Title
#dokoscmp 0x1b6ab0 gfx/

# Options
dokoscmp 0x19dd10 gfx/
#dokoscmp 0x1b32a0 gfx/
dokoscmp 0x19e4c0 gfx/
#dokoscmp 0x1b6ab0 gfx/

# Player select
dokoscmp 0x1b74c0 gfx/
dokoscmp 0x1bb2b0 gfx/
dokoscmp 0x1ba510 gfx/
dokoscmp 0x1bb430 gfx/

# henshin sequences would go here if i cared
# ...

# eyecatches (did these as venus, stage 1 -- others?)
dokoscmp 0x19e680 gfx/
dokoscmp 0x1a99c0 gfx/
dokoscmp 0x1aae10 gfx/
dokoscmp 0x1ac6d0 gfx/
# tilemap
dokoscmp 0x1ad100 gfx/
dokoscmp 0x18ec94 gfx/
# tilemap
dokoscmp 0x1bf0f0 gfx/
#dokoscmp 0x1ad100 gfx/

# stage 1-1
dokoscmp 0x1f0180 gfx/
dokoscmp 0x1f0eb0 gfx/
dokoscmp 0x1d0000 gfx/
dokoscmp 0x1c0000 gfx/
dokoscmp 0x1c0040 gfx/
dokoscmp 0x1c01f0 gfx/
dokoscmp 0x1c0da0 gfx/
dokoscmp 0x1c0e10 gfx/
dokoscmp 0x1f93e0 gfx/
dokoscmp 0x1c0cc0 gfx/

# ...

# results screen
dokoscmp 0x1a8d60 gfx/
dokoscmp 0x1a9720 gfx/
dokoscmp 0x1a9890 gfx/

# credits
#dokoscmp 0x1f0180 gfx/
#dokoscmp 0x1f0eb0 gfx/
dokoscmp 0x1883a0 gfx/

# region check screen
# size: 0x2ed
dokoscmp 0x11d540 gfx/
# size: 0x128
dokoscmp 0x11d410 gfx/

# continue screen
# "continue" text
dokoscmp 0x1be630 gfx/
#dokoscmp 0x1bb2b0 gfx/
# digits
dokoscmp 0x1be880 gfx/
# "game over" text
dokoscmp 0x19f1e0 gfx/



#for file in gfx/*; do
#  ./grpdmp $file gfx_dmp/$(basename $file).png
#done
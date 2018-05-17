###############################################################################
# 
# Bishoujo Senshi Sailor Moon (Mega Drive) translation build script
#
# Before running this script, you'll need to compile the included version of
# KENS in the kens-1.5a1 directory. This version is slightly modified to accept
# hexadecimal offsets, so don't substitute an existing installation.
# 
# You'll also need gcc in order to compile 68kasm (unless you compile it
# yourself by some other means).
#
###############################################################################

set -o errexit

BASE_PWD=$PWD

###############################################################################
# If no ROM parameters given, print usage info and quit
###############################################################################

if [ "$1" == "" -o "$2" == "" ]; then
  echo "Bishoujo Senshi Sailor Moon (Mega Drive) translation build script"
  echo "Usage: $0 <srcrom> <dstrom>"
  exit 0
fi

###############################################################################
# Make sure required tools are available
###############################################################################

###############################################################################
# Build project tools
###############################################################################

echo "************************************************************************"
echo "Building project tools..."
echo "************************************************************************"

make libmd
make

mkdir -p out/precmp
mkdir -p out/cmp
mkdir -p out/nocmp
mkdir -p out/scripts

# Location of the KENS tool koscmp, needed to compress files
KOSCMP="./kens-1.5a1/build/src/tools/koscmp"

if [ ! -f "$KOSCMP" ]; then
  echo "Error: KENS has not been compiled. Please build the bundled version of KENS located in the kens-1.5a1 directory."
  exit 1
fi

# Location of 68kasm, needed to build VWF hacks
M68KASM="68kasm/68kasm"

if [ ! -f $M68KASM ]; then
  echo "************************************************************************"
  echo "Building 68kasm..."
  echo "************************************************************************"
  
  cd 68kasm
    gcc -std=c99 *.c -o 68kasm
    
#    if [ ! $? -eq 0 ]; then
#      echo "Error compiling 68kasm"
#      exit
#    fi
  cd "$BASE_PWD"
fi

###############################################################################
# Set up variables
###############################################################################

INROM=$1
OUTROM=$2

cp "$INROM" "$OUTROM"

###############################################################################
# Patch in VWF modifications
###############################################################################

cd 68kasm

  echo "************************************************************************"
  echo "Assembling hacks..."
  echo "************************************************************************"

  # Assemble code
  ./68kasm -l bssm_vwf.asm

cd "$BASE_PWD"
  
echo "************************************************************************"
echo "Patching assembled hacks to ROM..."
echo "************************************************************************"

# "Link" output
./srecpatch "$OUTROM" "$OUTROM" 0x0 < 68kasm/bssm_vwf.h68

###############################################################################
# Insert new font
###############################################################################

echo "************************************************************************"
echo "Inserting font..."
echo "************************************************************************"

./smfontinsr "$OUTROM" "$OUTROM" font/ 0x188970 0x18FC00 pal/bssm_text_pal_line.bin

###############################################################################
# Assemble tilemaps
###############################################################################

echo "************************************************************************"
echo "Running tilemap generator scripts..."
echo "************************************************************************"

#./tilemapper tilemappers/title.txt

for file in tilemappers/*; do
  ./tilemapper "$file"
done

###############################################################################
# Render eyecatch logo
###############################################################################

echo "************************************************************************"
echo "Rendering eyecatch logo..."
echo "************************************************************************"

./logounrender rsrc/eyecatch/trans/logo.png pal/bssm_eyecatch_pal_mod_v2_line1.bin out/nocmp/eyecatch_logo.bin
./datpatch "rsrc/eyecatch/orig/other_grp.bin" "out/precmp/eyecatch_other_grp.bin" out/nocmp/eyecatch_logo.bin 0x2500

###############################################################################
# Update stage graphics/tiles
###############################################################################

echo "************************************************************************"
echo "Generating new stage graphics..."
echo "************************************************************************"

cp rsrc/stage1-1/trans/metatiles.bin out/precmp/stage1-1_mtiles.bin
cp rsrc/stage1-1/trans/metametatiles.bin out/precmp/stage1-1_mmtiles.bin
./grpundmp rsrc/stage1-1/trans/grp.png out/precmp/stage1-1_grp.bin -p pal/bssm_stage1-1_pal_line1.bin
# Re-inserting the HUD doesn't really go here, but it's used to insert a few extra tiles
# needed to make a neat translation of the background signs in stage 1-1
./grpundmp rsrc/misc/trans/hud.png out/precmp/hud.bin -p pal/bssm_stage1-1_pal_line1.bin

###############################################################################
# Compile game scripts
###############################################################################

echo "************************************************************************"
echo "Compiling game scripts..."
echo "************************************************************************"

./smscriptinsr scripts/bssm_script_en.csv out/scripts/ bssm_thingy_en.txt font/ > out/scripts_inject.txt

###############################################################################
# Compress data
###############################################################################

echo "************************************************************************"
echo "Compressing data..."
echo "************************************************************************"

#$KOSCMP out/grp.bin out/grp_cmp.bin
#$KOSCMP out/title_map.bin out/title_map_cmp.bin

for file in out/precmp/*; do
  $KOSCMP -v -c 8192 256 "$file" out/cmp/$(basename "$file")
done

###############################################################################
# Inject data into ROM
###############################################################################

echo "************************************************************************"
echo "Injecting data into ROM..."
echo "************************************************************************"

# Generate full injection script
cat bssm_inject_script.txt out/scripts_inject.txt > out/injection_script_full.txt
./datinjct bssm_freespace.txt out/injection_script_full.txt "$OUTROM" "$OUTROM"

###############################################################################
# Patch uncompressed tilemaps
###############################################################################

echo "************************************************************************"
echo "Patching uncompressed tilemaps..."
echo "************************************************************************"

./datpatch "$OUTROM" "$OUTROM" out/nocmp/options_hard.bin 0x13086
./datpatch "$OUTROM" "$OUTROM" out/nocmp/options_normal.bin 0x130AE
./datpatch "$OUTROM" "$OUTROM" out/nocmp/options_easy.bin 0x130D6
./datpatch "$OUTROM" "$OUTROM" out/nocmp/options_digits.bin 0x12C3C
./datpatch "$OUTROM" "$OUTROM" out/nocmp/playersel_portrait1.bin 0x18FD6
./datpatch "$OUTROM" "$OUTROM" out/nocmp/playersel_portrait2.bin 0x19128
./datpatch "$OUTROM" "$OUTROM" out/nocmp/playersel_portrait3.bin 0x1927A
./datpatch "$OUTROM" "$OUTROM" out/nocmp/playersel_portrait4.bin 0x193CC
./datpatch "$OUTROM" "$OUTROM" out/nocmp/playersel_portrait5.bin 0x1951E
./datpatch "$OUTROM" "$OUTROM" out/nocmp/results_ranka.bin 0x11168
./datpatch "$OUTROM" "$OUTROM" out/nocmp/results_rankb.bin 0x11170
./datpatch "$OUTROM" "$OUTROM" out/nocmp/results_rankc.bin 0x11178
./datpatch "$OUTROM" "$OUTROM" out/nocmp/results_rankd.bin 0x11180
./datpatch "$OUTROM" "$OUTROM" out/nocmp/results_ranke.bin 0x11188
#./datpatch "$OUTROM" "$OUTROM" out/nocmp/gameover_map.bin 0x11c7c
#./datpatch "$OUTROM" "$OUTROM" out/nocmp/continue_map.bin 0x11bec

###############################################################################
# Do final ROM prep
###############################################################################

echo "************************************************************************"
echo "Prepping ROM..."
echo "************************************************************************"

./romprep "$OUTROM" "$OUTROM"

###############################################################################
# We're done!
###############################################################################

echo "************************************************************************"
echo "Build complete!"
echo "************************************************************************"

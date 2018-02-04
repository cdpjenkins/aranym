#!/bin/sh

datefile=version_date.h
versfile=src/include/version.h
log=build-cygwin.log

if ! test -f $datefile; then
	echo "$0: $datefile missing" >&2
	exit 1
fi
if ! test -f $versfile; then
	echo "$0: $versfile missing" >&2
	exit 1
fi

#
# configure args for all configurations
#
CONFIGURE_ARGS="--prefix=/usr --enable-usbhost"

major=`sed -n -e 's/#define[ \t]*VER_MAJOR[ \t]*\([0-9]*\)/\1/p' $versfile`
minor=`sed -n -e 's/#define[ \t]*VER_MINOR[ \t]*\([0-9]*\)/\1/p' $versfile`
micro=`sed -n -e 's/#define[ \t]*VER_MICRO[ \t]*\([0-9]*\)/\1/p' $versfile`

PROJECT_LOWER=aranym
VERSION=$major.$minor.$micro
archive_tag=-cygwin-i386
ATAG=${VERSION}${archive_tag}

#
# recent version of cygwin have the mingw libraries installed
# in a sys-rooted cross-compiler environment
#
if test -d /usr/i686-w64-mingw32/sys-root/mingw; then
	MINGW_ROOT=/usr/i686-w64-mingw32/sys-root/mingw
	CONFIGURE_ARGS="$CONFIGURE_ARGS --with-sdl-prefix=$MINGW_ROOT --with-sdl2-prefix=$MINGW_ROOT"
fi
if test `uname -m` = x86_64; then
	CONFIGURE_ARGS="$CONFIGURE_ARGS --host=i686-pc-cygwin"
	# that currently does not work:
	# we build a 32-bit binary, but the installation process
	# below would copy cygwin dlls from the 64-bit environment
	echo "building 32-bit distribution in 64-bit environment currently not supported" >&2
	echo "install cygwin using setup-x86.exe and try again" >&2
	exit 1
fi

DATE=`sed -n -e 's/#define[ \t]*VERSION_DATE[ \t]*"\([^"]*\)"/\1/p' $datefile`

echo VERSION=$VERSION
echo DATE=$DATE
rm -f $log

trap 'rm -f $log' 1 2 3 15

function build() {
	local sdloption

	case $SDL in 
	1) sdloption=--disable-sdl2 ;;
	2) sdloption=--enable-sdl2 ;;
	*) exit 1 ;;
	esac
	
	echo "configuring aranym-mmu..."
	./configure $CONFIGURE_ARGS $sdloption --enable-fullmmu --enable-lilo >> $log 2>&1 || {
		echo "configuring aranym-mmu failed; see $log for details" >&2
		exit 1
	}
	echo "building aranym-mmu..."
	make clean
	make >> $log 2>&1 || {
		echo "building aranym-mmu failed; see $log for details" >&2
		exit 1
	}
	strip aranym.exe || exit 1
	mv aranym.exe aranym-mmu.exe
	
	
	echo "configuring aranym-jit..."
	./configure $CONFIGURE_ARGS $sdloption --enable-jit-compiler --enable-jit-fpu >> $log 2>&1 || {
		echo "configuring aranym-mmu failed; see $log for details" >&2
		exit 1
	}
	echo "building aranym-jit..."
	make clean
	make >> $log 2>&1 || {
		echo "building aranym-jit failed; see $log for details" >&2
		exit 1
	}
	strip aranym.exe || exit 1
	mv aranym.exe aranym-jit.exe
	
	
	echo "configuring aranym..."
	./configure $CONFIGURE_ARGS $sdloption >> $log 2>&1 || {
		echo "configuring aranym failed; see $log for details" >&2
		exit 1
	}
	echo "building aranym..."
	make clean
	make >> $log 2>&1 || {
		echo "building aranym failed; see $log for details" >&2
		exit 1
	}
	strip aranym.exe || exit 1
	mv aranym.exe aranym-dflt.exe
	
	make clean
	
	mv aranym-dflt.exe aranym.exe
}


function mkdist() {
	local sdlname
	local sdldef
	
	case $SDL in 
	1) sdlname=-sdl1
	   ;;
	2) sdlname=-sdl2
	   ;;
	*) exit 1 ;;
	esac
	sdldef=-DSDL=${sdlname}
	
	tmpdir="${TMPDIR:-/tmp}"
	distdir="$tmpdir/${PROJECT_LOWER}-$VERSION${sdlname}"
	
	mkdir -p "$distdir/doc" "$distdir/aranym" || exit 1
	
	cp -a COPYING "$distdir/COPYING.txt" || exit 1
	sed -e "s|@VERSION@|$VERSION|g" -e "s|@DATE@|$DATE|g" README-cygwin.in > "$distdir/README.txt" || exit 1
	for f in AUTHORS BUGS README ChangeLog FAQ INSTALL NEWS TODO; do
		cp -a $f "$distdir/doc/$f.txt" || exit 1
	done
	for f in config; do
		cp -a doc/$f "$distdir/doc/$f.txt" || exit 1
	done
	for f in logo.bmp wm_icon.bmp; do
		cp -a data/$f "$distdir/aranym/$f" || exit 1
	done
	
	(cd "$distdir"
	  for f in doc/* *.txt; do
	     cat $f | tr -d '\r' | sed -e 's/$/\r/' > $$.tmp
	     touch -r $f $$.tmp
	     mv $$.tmp $f
	  done
	)
	
	unset dlls
	declare -a dlls
	save_IFS=$IFS
	IFS='
'
	for f in aranym.exe aranym-jit.exe aranym-mmu.exe; do
		cp -a "$f" "$distdir" || exit 1
		dlls+=(`./ldd.exe --path $f`)
	done
	IFS=$save_IFS
	unset copydlls
	declare -a copydlls
	for dll in "${dlls[@]}"; do
		lower="`echo $dll | tr '[A-Z]' '[a-z]'`"
		case $lower in
		*/system32/* | */syswow64/* )
			continue ;;
		notfound:sdl.dll)
			dll=$MINGW_ROOT/bin/SDL.dll
			;;
		notfound:sdl_image.dll)
			dll=$MINGW_ROOT/bin/SDL_image.dll
			;;
		notfound:sdl2.dll)
			dll=$MINGW_ROOT/bin/SDL2.dll
			;;
		notfound:sdl2_image.dll)
			dll=$MINGW_ROOT/bin/SDL2_image.dll
			;;
		notfound:libgcc_s_sjlj-1.dll)
			dll=$MINGW_ROOT/bin/libgcc_s_sjlj-1.dll
			;;
		notfound:libwinpthread-1.dll)
			dll=$MINGW_ROOT/bin/libwinpthread-1.dll
			;;
		notfound:iconv.dll)
			dll=$MINGW_ROOT/bin/iconv.dll
			;;
		*)
			;;
		esac
		copydlls+=("$dll")
	done

	for f in "${copydlls[@]}"; do
		cp -a "$f" "$distdir" || exit 1
	done
	
	ARCHIVE="${PROJECT_LOWER}-${ATAG}${sdlname}.zip"
	SETUP_EXE="${PROJECT_LOWER}-${ATAG}${sdlname}-setup.exe"
	( cd "$tmpdir"
	  zip -r "$ARCHIVE" ${PROJECT_LOWER}-$VERSION${sdlname}
	) || exit 1
	
	echo "$tmpdir/$ARCHIVE ready for release"
	
	nsis=`cygpath "$PROGRAMFILES"`/NSIS/makensis.exe
	if test -x "$nsis"; then
	    cwd=`pwd`
	    cd "$distdir"
	    echo "creating Windows installer"
	    find . -type d | sed -e 's|^./\(.*\)$|${CreateDirectory} "$INSTDIR\\\1"|' -e '/^\.$/d' | tr '/' '\\' > ../aranym.files
	    find . -type f | sed -e 's|^./\(.*\)$|${File} "\1"|' | tr '/' '\\' >> ../aranym.files
	    "$nsis" -V2 -NOCD -DVER_MAJOR=$major -DVER_MINOR=$minor -DVER_MICRO=$micro $sdldef -DOUTFILE=../${SETUP_EXE} `cygpath -m $cwd/tools/aranym.nsi` || exit 1
	    rm -f ../aranym.files
	    cd $cwd
	    echo "$tmpdir/${SETUP_EXE} ready for release"
	fi
	
}


gcc -O2 -mwin32 tools/ldd.c -o ldd.exe || exit 1

SDL=1
build
mkdist

SDL=2
build
mkdist


rm -f $log ldd.exe

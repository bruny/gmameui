#!/bin/sh
ACLOCAL_FLAGS="-I m4"

SETUP_GETTEXT=./setup-gettext

($SETUP_GETTEXT --gettext-tool) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have gettext installed to compile GMAMEUI.";
	echo;
	exit;
}

(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
    echo;
    echo "You must have intltool installed to compile GMAMEUI.";
    echo;
    exit;
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have automake installed to compile GMAMEUI";
	echo;
	exit;
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have autoconf installed to compile GMAMEUI";
	echo;
	exit;
}

echo "Generating configuration files for GMAMEUI, please wait...."
echo;

# Backup the po/ChangeLog. This should prevent the annoying
# gettext ChangeLog modifications.

cp -p po/ChangeLog po/ChangeLog.save

echo "Running gettextize, please ignore non-fatal messages...."
$SETUP_GETTEXT
intltoolize --force --copy --automake

# Restore the po/ChangeLog file.
mv po/ChangeLog.save po/ChangeLog

# Add other directories to this list if people continue to experience
# brokennesses ...  Obviously the real answer is for them to fix it
# themselves, but for Luke's sake we have this.
for dir in "/usr/local/share/aclocal"
do
	if test -d $dir ; then
		ACLOCAL_FLAGS="$ACLOCAL_FLAGS -I $dir"
	fi
done

echo "Running aclocal $ACLOCAL_FLAGS ..."
aclocal $ACLOCAL_FLAGS || exit;
echo "Running autoheader..."
autoheader || exit;
echo "Running automake --add-missing --copy ..."
automake --add-missing --copy;
echo "Running autoconf ..."
autoconf || exit;
echo "Running automake ..."
automake || exit;

echo "Now you can run ./configure"

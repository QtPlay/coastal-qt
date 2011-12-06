#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Coastal Manpager
GenericName=Manual Viewer
Comment=Manual page viwer
TryExec=$1/bin/coastal-manpager
Exec=$1/bin/coastal-manpager &
Categories=Application;Utility;
Icon=$1/share/icons/coastal-manpager.png
EOF

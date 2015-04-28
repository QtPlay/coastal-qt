# Copyright (c) 2012-2015 David Sugar, Tycho Softworks.
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# different distros may have different lib pkg naming conventions...

%define libname libcoastal0
Name:           coastal-qt
Summary:        Coastal Qt default applications
Version:        0.10.2
Release:        1
License:        GPL-3.0+
URL:            http://www.gnutelephony.org
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
%if 0%{?suse_version} >= 1330
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-linguist-devel
BuildRequires:  libQt5PlatformSupport-private-headers-devel
%else
BuildRequires:  libqt4-devel >= 4.8.0
%endif
BuildRequires:  cmake >= 2.8.0
BuildRequires:  gcc-c++
BuildRequires:  update-desktop-files
Requires:       %{libname} = %{version}-%{release}
Group:          System/X11/Utilities
Summary:        Stand-alone applications and utilities
Obsoletes:      coastal-qt-apps

%package -n %libname
Group: System/Libraries
Summary: Runtime library support for coastal applications

%description -n %libname
Runtime library required by coastal applications.  This offers extensions
for the Qt framework that we use.

%package devel
Requires: %{libname} = %{version}-%{release}
Requires: libqt4-devel >= 4.8.0
Requires: gcc-c++ pkgconfig
Group: Development/Libraries/C and C++
Summary: Headers for building coastal applications

%description
Stand-alone applications built with coastal-qt and some helper
applications. 

%description devel
Header files for building applications with coastal library.  Used in
conjunction with Qt.

%prep
%setup -q

%build
%cmake	-DCMAKE_INSTALL_SYSCONFDIR=%{_sysconfdir} \
		-DCMAKE_INSTALL_LOCALSTATEDIR=%{_localstatedir}

%{__make} %{?_smp_mflags}

%install
%cmake_install
%{__rm} -rf %{buildroot}%{_libdir}/*.la
%suse_update_desktop_file -r coastal-manpager Qt Utility DesktopUtility Office Viewer 
%suse_update_desktop_file -r coastal-search Qt Utility DesktopUtility Filesystem

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/coastal-manpager
%{_datadir}/applications/coastal-manpager.desktop
%{_datadir}/pixmaps/coastal-manpager.png
%{_bindir}/coastal-search
%{_datadir}/applications/coastal-search.desktop
%{_datadir}/pixmaps/coastal-search.png
%{_bindir}/coastal-icons
%{_bindir}/coastal-dialog

%files -n %libname
%doc README COPYING TODO ChangeLog
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/coastal.h
%{_libdir}/pkgconfig/*.pc

%dir %{_datadir}/coastal-qt
%dir %{_datadir}/coastal-qt/cmake
%{_datadir}/coastal-qt/cmake/*.cmake

%post -n %libname -p /sbin/ldconfig

%postun -n %libname -p /sbin/ldconfig

%changelog
* Sun Mar 23 2014 David Sugar <dyfet@gnutelephony.org> - 0.9.0-0
- Initial packaging for osc


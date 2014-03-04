# Copyright (c) 2012 David Sugar, Tycho Softworks.
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

Name: coastal-qt
Group: Development/Libraries
Summary: Runtime library support for coastal applications
Version: 0.8.0
Release: 0%{?dist}
License: GPLv3+
URL: http://www.gnutelephony.org
Source: http://dev.gnutelephony.org/dist/tarballs/coastal-qt-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: qt4-devel >= 4.8.0
BuildRequires: cmake >= 2.6.0

%package devel
Requires: %{name} = %{version}-%{release}
Requires: qt4-devel >= 4.8.0
Group: Development/Libraries
Summary: Headers for building coastal applications

%package apps
Group: Applications/System
Summary: Stand-alone applications and utilities
Requires: %{name} = %{version}-%{release}

%description manpager
Stand-alone applications and extra utilities that are built with the coastal-qt library.

%description devel
Header files for building applications with coastal library.  Used in
conjunction with Qt.

%description
Runtime library required by coastal applications.  This offers extensions
for the Qt framework that we use.

%prep
%setup -q

%build
%cmake	-DINSTALL_MANDIR=%{_mandir} \
		-DINSTALL_DATADIR=%{_datadir} \
		-DINSTALL_LIBDIR=%{_libdir} \
		-DINSTALL_BINDIR=%{_bindir} \
		-DINSTALL_INCLUDEDIR=%{_includedir} .

%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR=%{buildroot}
%{__rm} -rf %{buildroot}%{_libdir}/*.la

%clean
%{__rm} -rf %{buildroot}

%files apps
%defattr(-,root,root,-)
%{_bindir}/coastal-manpager
%{_datadir}/applications/coastal-manpager.desktop
%{_datadir}/pixmaps/coastal-manpager.png
%{_bindir}/coastal-search
%{_datadir}/applications/coastal-search.desktop
%{_datadir}/pixmaps/coastal-search.png
%{_bindir}/coastal-notify
%{_datadir}/pixmaps/coastal-notify.png
%{_bindir}/coastal-icons

%files
%doc README COPYING TODO ChangeLog
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/coastal.h
%{_libdir}/pkgconfig/*.pc

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%changelog


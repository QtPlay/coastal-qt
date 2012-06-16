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
Summary: QT Desktop Applications
Version: 0.2.0
Release: 0%{?dist}
License: GPLv3+
URL: http://www.gnutelephony.org
Group: Applications/System
Source: http://www.gnutelephony.org/dist/tarballs/coastal-qt-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: qt4-devel cmake
Requires: %{name}-runtime = %{version}-%{release}

%package runtime
Group: Development/Libraries
Summary: Runtime library support for coastal applications

%package devel
Requires: %{name}-runtime = %{version}-%{release}
Requires: qt4-devel
Group: Development/Libraries
Summary: Headers for building coastal applications

%description
Generic QT desktop system applications.  This includes coastal-manpager
and coastal-searchl.

%description devel
Header files for building applications with coastal library.  Used in
conjunction with Qt.

%description runtime
Runtime library required by coastal applications.  This offers extensions
for the Qt framework that we use.

%prep
%setup -q

%build
cmake -DCMAKE_INSTALL_PREFIX=%{_prefix} \
      -DSYSCONFDIR=%{_sysconfdir} \
      -DMANDIR=%{_mandir} \
      -DDATADIR=%{_datadir} \
      -DCMAKE_VERBOSE_MAKEFILE=TRUE \
      -DCMAKE_C_FLAGS_RELEASE:STRING="$RPM_OPT_FLAGS" \
      -DCMAKE_CXX_FLAGS_RELEASE:STRING="$RPM_OPT_FLAGS" \

%{__make} %{?_smp_mflags}

%install
cd build
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR=%{buildroot}
%{__rm} -rf %{buildroot}%{_libdir}/*.la

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc README COPYING TODO NOTES ChangeLog
%{_bindir}/coastal-manpager
%{_bindir}/coastal-search
%{_datadir}/applications/coastal-manpager.desktop
%{_datadir}/applications/coastal-search.desktop
%{_datadir}/icons/coastal-manpager.png
%{_datadir}/icons/coastal-search.png

%files runtime
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_includedir}/coastal.h
%{_libdir}/pkgconfig/*.pc

%post runtime
/sbin/ldconfig

%postun runtime
/sbin/ldconfig

%changelog


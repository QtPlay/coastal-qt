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
Version: @VERSION@
Release: 0%{?dist}
License: GPLv3+
URL: http://www.gnutelephony.org
Source: http://www.gnutelephony.org/dist/tarballs/coastal-qt-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: qt4-devel >= 4.8.0
BuildRequires: cmake >= 2.6.0

%package devel
Requires: %{name}-runtime = %{version}-%{release}
Requires: qt4-devel >= 4.8.0
Group: Development/Libraries
Summary: Headers for building coastal applications

# this may later be separated into separate per app packages like we do
# in debian
%package apps
Group: Applications/System
Summary: QT Desktop Applications
Requires: %{name} = %{version}-%{release}

%package dialogs
Group: Applications/System
Summary: QT Dialog Applications
Requires: %{name} = %{version}-%{release}

%description apps
Generic QT desktop system applications.  This includes coastal-manpager
and coastal-search.

%description devel
Header files for building applications with coastal library.  Used in
conjunction with Qt.

%description
Runtime library required by coastal applications.  This offers extensions
for the Qt framework that we use.

%description dialogs
Small coastal qt dialog applications.

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
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR=%{buildroot}
%{__rm} -rf %{buildroot}%{_libdir}/*.la

%clean
%{__rm} -rf %{buildroot}

%files apps
%defattr(-,root,root,-)
%{_bindir}/coastal-manpager
%{_bindir}/coastal-search
%{_bindir}/coastal-notify
%{_datadir}/applications/coastal-manpager.desktop
%{_datadir}/applications/coastal-search.desktop
%{_datadir}/icons/coastal-manpager.png
%{_datadir}/icons/coastal-search.png
%{_datadir}/icons/coastal-notify.png

%files
%doc README COPYING TODO NOTES ChangeLog
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files dialogs
%defattr(-,root,root,-)
%{_bindir}/coastal-icons

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


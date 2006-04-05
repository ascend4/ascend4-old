
Name:		ascend
Summary:	ASCEND Modelling Environment
Version:	0.9.6rc0

# Use release "0" so that distro-released versions will override ours.
Release:	0.jdpipe

License:	GPL
Group:		Applications/Graphics
Source:		%{name}-%{version}.tar.bz2
URL:		http://inkscape.sourceforge.net/

Prefix:		%{_prefix}
Packager:	Automatic
Vendor:		The Inkscape Project
BuildRoot:	%{_tmppath}/%{name}-%{version}-root

BuildRequires: python >= 2.4, python-devel
BuildRequires: scons >= 0.96
BuildRequires: bison, flex
BuildRequires: swig >= 1.3.24
BuildRequires: desktop-file-utils

Requires(post):   desktop-file-utils
Requires(postun): desktop-file-utils

Requires: python >= 2.4
Requires: pygtk2 >= 2.6, pygtk2-libglade
Requires: python-matplotlib, python-numeric
Requires: gtksourceview
Requires: make

%description
ASCEND IV is both a large-scale object-oriented mathematical
modeling environment and a strongly typed mathematical modeling
language. Although ASCEND has primarily been developed by Chemical
Engineers, great care has been exercised to assure that it is
domain independent. ASCEND can support modeling activities in
fields from Architecture to (computational) Zoology.

%prep
%setup

%build
scons %{?_smp_mflags}

%install
rm -rf %{buildroot}
scons INSTALL_PREFIX=%{buildroot}%{_prefix} INSTALL_DATA=%{buildroot}%{_datadir} INSTALL_BIN=%{buildroot}%{_bindir} install
install -m 644 -o root pygtk/interface/ascend.desktop %{buildroot}%{_datadir}/applications/
install -m 644 -o root pygtk/interface/ascend.lang %{buildroot}%{_datadir}/gtksourceview-1.0/language-specs/

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%doc INSTALL tcltk98/license.txt
%{_bindir}/ascend
%{_datadir}/applications/ascend.desktop
%{_datadir}/ascend/*

%changelog
* Thu Apr 04 2006 John Pye <john.pye@student.unsw.edu.au>
- First RPM package for new SCons build

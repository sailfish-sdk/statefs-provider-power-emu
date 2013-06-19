Summary: Statefs provider to emulate power subsystem properties
Name: statefs-provider-power-emu
Version: x.x.x
Release: 1
License: LGPLv2
Group: System Environment/Libraries
URL: http://github.com/nemomobile/statefs-provider-power-emu
Source0: %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(statefs-cpp) >= 0.2.17
Requires: statefs

%description
%{summary}

%prep
%setup -q

%build
%cmake
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_libdir}/statefs/libprovider-power-emu.so

%post
statefs register %{_libdir}/statefs/libprovider-power-emu.so || :

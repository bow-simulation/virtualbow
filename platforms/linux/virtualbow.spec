# https://stackoverflow.com/questions/880227/what-is-the-minimum-i-have-to-do-to-create-an-rpm-file

%define __spec_install_post %{nil}
%define   debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress

Summary: @APPLICATION_DESCRIPTION_SHORT@
Name: @APPLICATION_NAME@
Version: @APPLICATION_VERSION@
Release: 1
License: GPL
SOURCE0 : %{name}-%{version}.tar.gz
URL: @APPLICATION_WEBSITE@
Requires: qt5-qtbase

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
%{summary}

%prep
%setup -q

%build
# Empty section.

%install
rm -rf %{buildroot}
mkdir -p  %{buildroot}
cp -a * %{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(755,root,root)
/usr/bin/virtualbow-gui
/usr/bin/virtualbow-slv
/usr/bin/virtualbow-post
%defattr(644,root,root)
/usr/share/applications/virtualbow-gui.desktop
/usr/share/applications/virtualbow-post.desktop
/usr/share/icons/hicolor/scalable/apps/virtualbow.svg
/usr/share/virtualbow/manual.pdf

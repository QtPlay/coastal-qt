# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.

# Maintainer: David Sugar <dyfet@gnutelephony.org>
pkgname=coastal-qt
pkgver=@VERSION@
pkgrel=0
epoch=9
pkgdesc="QT extension library and some simple desktop applications"
arch=('x86_64' 'i686')
url="http://www.gnutelephony.org"
license=('GPL3')
groups=()
depends=('qt4>=4.8.0' 'libxss' 'file')
makedepends=('cmake>=2.8.0')
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=($pkgname-$pkgver.tar.gz)
noextract=()
md5sums=('SKIP')

build() {
	cd "$srcdir/$pkgname-$pkgver"
	cmake ./ -DCMAKE_INSTALL_PREFIX=/usr -DQT_QMAKE_EXECUTABLE=qmake-qt4 -DBUILD_QT4ONLY=true
	make
}

package() {
	cd "$srcdir/$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install
	install -Dm644 COPYING "$pkgdir/usr/share/licenses/$pkgname/COPYING"
}

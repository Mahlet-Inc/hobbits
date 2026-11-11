#include <QCoreApplication>
#include <QSslSocket>
#include <QTextStream>

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    if (argc == 2) {
        // Allow install verification to restrict backend discovery to a relocated
        // package instead of accidentally finding the build-tree TLS plugin.
        QCoreApplication::setLibraryPaths({QString::fromLocal8Bit(argv[1])});
    }
    QTextStream output(stdout);
#if QT_CONFIG(ssl)
    output << "TLS backends: " << QSslSocket::availableBackends().join(", ") << '\n';
    if (!QSslSocket::supportsSsl()) {
        output << "Qt has no working TLS backend; HTTPS imports would fail.\n";
        return 1;
    }
    output << "TLS runtime: " << QSslSocket::sslLibraryVersionString() << '\n';
    return 0;
#else
    output << "Qt was built without TLS support; HTTPS imports would fail.\n";
    return 1;
#endif
}

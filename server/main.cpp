#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    WhiteboardServer server;
    server.startServer(1234, 49152);
    return app.exec();
}

#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    WhiteboardServer server;
    server.startServer(1234, 5678);
    return app.exec();
}

#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(AnomadeApp);
    QApplication app(argc, argv);
    app.setApplicationName("Anomade");
    app.setOrganizationName("Metatri");
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
    return app.exec();
}

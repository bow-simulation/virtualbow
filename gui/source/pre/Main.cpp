#include "MainWindow.hpp"
#include "KeyEventFilter.hpp"
#include "config.hpp"
#include <iostream>

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName(Config::ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(Config::APPLICATION_NAME_GUI);
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    QApplication::setApplicationVersion(Config::APPLICATION_VERSION);
    QLocale::setDefault(QLocale::C);

    QApplication application(argc, argv);
    application.installEventFilter(new KeyEventFilter());

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Model file (.bow)");
    parser.process(application);

    QStringList args = parser.positionalArguments();
    if(args.size() > 1) {
        std::cerr << "Only one argument is accepted." << std::endl;
        return 1;
    }

    MainWindow window;
    window.show();
    if(args.size() == 1) {
        window.loadFromFile(args[0]);
    }

    return application.exec();
}

/*
#include "models/MainModel.hpp"
#include <iostream>

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QBindable>
#include <qmetaobject.h>

void bind(QObject* target, const char* targetProperty, const QObject* source, const char* sourceProperty) {
    const QMetaObject* metaTarget = target->metaObject();
    const QMetaObject* metaSource = source->metaObject();

    QMetaProperty metaPropTarget = metaTarget->property(metaTarget->indexOfProperty(targetProperty));
    QMetaProperty metaPropSource = metaSource->property(metaSource->indexOfProperty(sourceProperty));

}

int main(int argc, char* argv[]) {
    MainModel viewModel;

    QApplication application(argc, argv);
    QMainWindow window;

    QPushButton* button = new QPushButton("Modify");
    window.setCentralWidget(button);

    //QObject::connect(button, &QPushButton::clicked, &viewModel, &MainModel::modify);

    //window.setWindowFilePath(viewModel.currentFile());
    //QObject::connect(&viewModel, &MainModel::currentFileChanged, &window, &QMainWindow::setWindowFilePath);

    bind(&window, "windowFilePath", &viewModel, "currentFile");

    window.show();
    return application.exec();
}
*/

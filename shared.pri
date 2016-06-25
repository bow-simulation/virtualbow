INCLUDEPATH += /home/s/Libraries/Eigen-3.2.8 \
               /home/s/Libraries/Catch-1.5.6 \

SOURCES += src/gui/MainWindow.cpp \
           src/fem/System.cpp \
           src/fem/elements/Element.cpp

HEADERS += src/gui/MainWindow.hpp \
           src/fem/System.hpp \
           src/fem/elements/Element.hpp \
           src/fem/elements/BarElement.hpp \
           src/fem/elements/MassElement.hpp \
           src/fem/elements/BeamElement.hpp \
           src/fem/Node.hpp \
           src/fem/View.hpp

TEMPLATE = subdirs

SUBDIRS += \
    polygon \
    line \
    circle\
    figure \
    tnest

polygon.file = polygon/polygon.pro
line.file = line/line.pro
circle.file = circle/circle.pro
figure.file = figure/figure.pro

CONFIG += ordered

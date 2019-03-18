TEMPLATE = subdirs

SUBDIRS += \
        talgorithmslib \
        examples

talgorithmslib.file = talgorithmslib/talgorithmslib.pro
examples.file = examples/examples.pro

CONFIG += ordered

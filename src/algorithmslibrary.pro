TEMPLATE = subdirs

SUBDIRS += \
        talgorithmslib \
	examples \
	test

talgorithmslib.file = talgorithmslib/talgorithmslib.pro
examples.file = examples/examples.pro
test.file = test/test.pro

CONFIG += ordered

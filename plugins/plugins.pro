TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

SUBDIRS += SecPrFFT
SUBDIRS += Player

TARGET = rip3p

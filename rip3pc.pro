TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

SUBDIRS = plugins \
          app

TARGET = rip3p

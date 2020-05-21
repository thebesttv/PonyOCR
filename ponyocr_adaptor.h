/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.thebesttv.PonyOCR.xml -a ponyocr_adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2019 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef PONYOCR_ADAPTOR_H
#define PONYOCR_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.thebesttv.PonyOCR
 */
class PonyOCRAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.thebesttv.PonyOCR")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.thebesttv.PonyOCR\">\n"
"    <method name=\"requestOCR\"/>\n"
"  </interface>\n"
        "")
public:
    PonyOCRAdaptor(QObject *parent);
    virtual ~PonyOCRAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void requestOCR();
Q_SIGNALS: // SIGNALS
};

#endif
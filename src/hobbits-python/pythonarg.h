#ifndef PYTHONARG_H
#define PYTHONARG_H

#include <QString>
#include "pluginactionprogress.h"
#include "bitcontainer.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonArg
{
public:
    enum Type {
        HobbitsWrapper = 0x1,
        String = 0x2,
        Integer = 0x3,
        Double = 0x4
    };

    static PythonArg* bitArray(QSharedPointer<BitArray> bitArray);
    static PythonArg* constBitArray(QSharedPointer<const BitArray> bitArray);
    static PythonArg* actionProgress(QSharedPointer<PluginActionProgress> progress);
    static PythonArg* bitContainer(QSharedPointer<BitContainer> container);
    static PythonArg* constBitContainer(QSharedPointer<const BitContainer> container);
    static PythonArg* bitInfo(QSharedPointer<BitInfo> info);
    static PythonArg* constBitInfo(QSharedPointer<const BitInfo> info);
    static PythonArg* qString(QString s);
    static PythonArg* integer(int n);
    static PythonArg* number(double n);
    static PythonArg* boolean(bool n);

    Type type() const;
    QString argSymbol() const;
    QString wrapType() const;
    void *pointer() const;
    QString stringData() const;
    int integerData() const;
    double doubleData() const;

private:
    PythonArg();

    Type m_type;
    QString m_argSymbol;
    QString m_wrapType;
    void* m_pointer;
    QString m_stringData;
    int m_integerData;
    double m_doubleData;
};

#endif // PYTHONARG_H

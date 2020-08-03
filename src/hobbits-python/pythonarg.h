#ifndef PYTHONARG_H
#define PYTHONARG_H

#include <QString>
#include "bitarray.h"
#include "actionprogress.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonArg
{
public:
    static PythonArg* bitArray(QSharedPointer<BitArray> bitArray);
    static PythonArg* constBitArray(QSharedPointer<const BitArray> bitArray);
    static PythonArg* actionProgress(QSharedPointer<ActionProgress> progress);

    friend class PythonInterpreter;

private:
    PythonArg();

    enum Type {
        HobbitsWrapper = 0x1
    };

    Type m_type;
    QString m_argSymbol;
    QString m_wrapType;
    void* m_pointer;
};

#endif // PYTHONARG_H

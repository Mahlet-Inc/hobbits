#include "pythonarg.h"
#include <QSharedPointer>

PythonArg::PythonArg()
{

}

QString PythonArg::stringData() const
{
    return m_stringData;
}

void *PythonArg::pointer() const
{
    return m_pointer;
}

QString PythonArg::wrapType() const
{
    return m_wrapType;
}

QString PythonArg::argSymbol() const
{
    return m_argSymbol;
}

PythonArg::Type PythonArg::type() const
{
    return m_type;
}

PythonArg* PythonArg::bitArray(QSharedPointer<BitArray> bitArray)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "BitArray";
    arg->m_argSymbol = "O";
    arg->m_pointer = bitArray.data();
    return arg;
}

PythonArg* PythonArg::constBitArray(QSharedPointer<const BitArray> bitArray)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "ImmutableBitArray";
    arg->m_argSymbol = "O";
    arg->m_pointer = const_cast<BitArray*>(bitArray.data());
    return arg;
}

PythonArg *PythonArg::actionProgress(QSharedPointer<ActionProgress> progress)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "ActionProgress";
    arg->m_argSymbol = "O";
    arg->m_pointer = progress.data();
    return arg;
}

PythonArg *PythonArg::qString(QString s)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::String;
    arg->m_argSymbol = "s";
    arg->m_stringData = s;
    return arg;
}

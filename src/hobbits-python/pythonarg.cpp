#include "pythonarg.h"
#include <QSharedPointer>

PythonArg::PythonArg()
{
    m_pointer = nullptr;
    m_stringData = QString();
    m_integerData = 0;
    m_doubleData = 0;
    m_boolData = false;
}

bool PythonArg::boolData() const
{
    return m_boolData;
}

QSize PythonArg::sizeData() const
{
    return m_sizeData;
}

int PythonArg::integerData() const
{
    return m_integerData;
}

double PythonArg::doubleData() const
{
    return m_doubleData;
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

PythonArg *PythonArg::actionProgress(QSharedPointer<PluginActionProgress> progress)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "PluginActionProgress";
    arg->m_argSymbol = "O";
    arg->m_pointer = progress.data();
    return arg;
}

PythonArg *PythonArg::bitContainer(QSharedPointer<BitContainer> container)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "BitContainer";
    arg->m_argSymbol = "O";
    arg->m_pointer = container.data();
    return arg;
}

PythonArg *PythonArg::constBitContainer(QSharedPointer<const BitContainer> container)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "ImmutableBitContainer";
    arg->m_argSymbol = "O";
    arg->m_pointer = const_cast<BitContainer*>(container.data());
    return arg;
}

PythonArg *PythonArg::bitInfo(QSharedPointer<BitInfo> info)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "BitInfo";
    arg->m_argSymbol = "O";
    arg->m_pointer = info.data();
    return arg;
}

PythonArg *PythonArg::constBitInfo(QSharedPointer<const BitInfo> info)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "ImmutableBitInfo";
    arg->m_argSymbol = "O";
    arg->m_pointer = const_cast<BitInfo*>(info.data());
    return arg;
}

PythonArg *PythonArg::displayHandle(QSharedPointer<DisplayHandle> handle)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::HobbitsWrapper;
    arg->m_wrapType = "DisplayHandle";
    arg->m_argSymbol = "O";
    arg->m_pointer = handle.data();
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

PythonArg *PythonArg::integer(int n)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::Integer;
    arg->m_argSymbol = "i";
    arg->m_integerData = n;
    return arg;
}

PythonArg *PythonArg::number(double n)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::Double;
    arg->m_argSymbol = "d";
    arg->m_doubleData = n;
    return arg;
}

PythonArg *PythonArg::boolean(bool n)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::Boolean;
    arg->m_argSymbol = "O";
    arg->m_boolData = n;
    return arg;
}

PythonArg *PythonArg::byteBuffer(char *bytes, int size)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::ByteBuffer;
    arg->m_argSymbol = "";
    arg->m_integerData = size;
    arg->m_pointer = bytes;
    return arg;
}

PythonArg *PythonArg::imageBuffer(char *bytes, QSize size)
{
    PythonArg *arg = new PythonArg();
    arg->m_type = Type::ImageBuffer;
    arg->m_argSymbol = "";
    arg->m_sizeData = size;
    arg->m_pointer = bytes;
    return arg;
}

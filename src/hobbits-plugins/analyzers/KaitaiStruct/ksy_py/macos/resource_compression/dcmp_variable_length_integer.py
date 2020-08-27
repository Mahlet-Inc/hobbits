# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class DcmpVariableLengthInteger(KaitaiStruct):
    """A variable-length integer,
    in the format used by the 0xfe chunks in the `'dcmp' (0)` and `'dcmp' (1)` resource compression formats.
    See the dcmp_0 and dcmp_1 specs for more information about these compression formats.
    
    This variable-length integer format can store an integer `x` in any of the following ways:
    
    * In a single byte,
      if `0 <= x <= 0x7f`
      (7-bit unsigned integer)
    * In 2 bytes,
      if `-0x4000 <= x <= 0x3eff`
      (15-bit signed integer with the highest `0x100` values unavailable)
    * In 5 bytes, if `-0x80000000 <= x <= 0x7fffffff`
      (32-bit signed integer)
    
    In practice,
    values are always stored in the smallest possible format,
    but technically any of the larger formats could be used as well.
    
    .. seealso::
       Source - https://github.com/dgelessus/python-rsrcfork/tree/master/rsrcfork/compress/common.py
    """
    SEQ_FIELDS = ["first", "more"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['first']['start'] = self._io.pos()
        self.first = self._io.read_u1()
        self._debug['first']['end'] = self._io.pos()
        if self.first >= 128:
            self._debug['more']['start'] = self._io.pos()
            _on = self.first
            if _on == 255:
                self.more = self._io.read_s4be()
            else:
                self.more = self._io.read_u1()
            self._debug['more']['end'] = self._io.pos()


    @property
    def value(self):
        """The decoded value of the variable-length integer.
        """
        if hasattr(self, '_m_value'):
            return self._m_value if hasattr(self, '_m_value') else None

        self._m_value = (self.more if self.first == 255 else ((((self.first << 8) | self.more) - 49152) if self.first >= 128 else self.first))
        return self._m_value if hasattr(self, '_m_value') else None



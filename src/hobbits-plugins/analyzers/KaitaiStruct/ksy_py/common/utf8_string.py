# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Utf8String(KaitaiStruct):
    """UTF-8 is a popular character encoding scheme that allows to
    represent strings as sequence of code points defined in Unicode
    standard. Its features are:
    
    * variable width (i.e. one code point might be represented by 1 to 4
      bytes)
    * backward compatiblity with ASCII
    * basic validity checking (and thus distinguishing from other legacy
      8-bit encodings)
    * maintaining sort order of codepoints if sorted as a byte array
    
    WARNING: For the vast majority of practical purposes of format
    definitions in Kaitai Struct, you'd likely NOT want to use this and
    rather just use `type: str` with `encoding: utf8`. That will use
    native string implementations, which are most likely more efficient
    and will give you native language strings, rather than an array of
    individual codepoints.  This format definition is provided mostly
    for educational / research purposes.
    """
    SEQ_FIELDS = ["codepoints"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['codepoints']['start'] = self._io.pos()
        self.codepoints = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['codepoints']:
                self._debug['codepoints']['arr'] = []
            self._debug['codepoints']['arr'].append({'start': self._io.pos()})
            _t_codepoints = Utf8String.Utf8Codepoint(self._io, self, self._root)
            _t_codepoints._read()
            self.codepoints.append(_t_codepoints)
            self._debug['codepoints']['arr'][len(self.codepoints) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['codepoints']['end'] = self._io.pos()

    class Utf8Codepoint(KaitaiStruct):
        SEQ_FIELDS = ["byte1", "byte2", "byte3", "byte4"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['byte1']['start'] = self._io.pos()
            self.byte1 = self._io.read_u1()
            self._debug['byte1']['end'] = self._io.pos()
            if self.len >= 2:
                self._debug['byte2']['start'] = self._io.pos()
                self.byte2 = self._io.read_u1()
                self._debug['byte2']['end'] = self._io.pos()

            if self.len >= 3:
                self._debug['byte3']['start'] = self._io.pos()
                self.byte3 = self._io.read_u1()
                self._debug['byte3']['end'] = self._io.pos()

            if self.len >= 4:
                self._debug['byte4']['start'] = self._io.pos()
                self.byte4 = self._io.read_u1()
                self._debug['byte4']['end'] = self._io.pos()


        @property
        def raw1(self):
            if hasattr(self, '_m_raw1'):
                return self._m_raw1 if hasattr(self, '_m_raw1') else None

            self._m_raw1 = (self.byte1 & (127 if self.len == 1 else (31 if self.len == 2 else (15 if self.len == 3 else (7 if self.len == 4 else 0)))))
            return self._m_raw1 if hasattr(self, '_m_raw1') else None

        @property
        def raw4(self):
            if hasattr(self, '_m_raw4'):
                return self._m_raw4 if hasattr(self, '_m_raw4') else None

            if self.len >= 4:
                self._m_raw4 = (self.byte4 & 63)

            return self._m_raw4 if hasattr(self, '_m_raw4') else None

        @property
        def raw3(self):
            if hasattr(self, '_m_raw3'):
                return self._m_raw3 if hasattr(self, '_m_raw3') else None

            if self.len >= 3:
                self._m_raw3 = (self.byte3 & 63)

            return self._m_raw3 if hasattr(self, '_m_raw3') else None

        @property
        def value_as_int(self):
            if hasattr(self, '_m_value_as_int'):
                return self._m_value_as_int if hasattr(self, '_m_value_as_int') else None

            self._m_value_as_int = (self.raw1 if self.len == 1 else (((self.raw1 << 6) | self.raw2) if self.len == 2 else ((((self.raw1 << 12) | (self.raw2 << 6)) | self.raw3) if self.len == 3 else (((((self.raw1 << 18) | (self.raw2 << 12)) | (self.raw3 << 6)) | self.raw4) if self.len == 4 else -1))))
            return self._m_value_as_int if hasattr(self, '_m_value_as_int') else None

        @property
        def raw2(self):
            if hasattr(self, '_m_raw2'):
                return self._m_raw2 if hasattr(self, '_m_raw2') else None

            if self.len >= 2:
                self._m_raw2 = (self.byte2 & 63)

            return self._m_raw2 if hasattr(self, '_m_raw2') else None

        @property
        def len(self):
            if hasattr(self, '_m_len'):
                return self._m_len if hasattr(self, '_m_len') else None

            self._m_len = (1 if (self.byte1 & 128) == 0 else (2 if (self.byte1 & 224) == 192 else (3 if (self.byte1 & 240) == 224 else (4 if (self.byte1 & 248) == 240 else -1))))
            return self._m_len if hasattr(self, '_m_len') else None




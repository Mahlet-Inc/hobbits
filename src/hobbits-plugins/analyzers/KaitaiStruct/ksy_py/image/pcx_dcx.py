# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import pcx
class PcxDcx(KaitaiStruct):
    """DCX is a simple extension of PCX image format allowing to bundle
    many PCX images (typically, pages of a document) in one file. It saw
    some limited use in DOS-era fax software, but was largely
    superseeded with multi-page TIFFs and PDFs since then.
    """
    SEQ_FIELDS = ["magic", "files"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['magic']['start'] = self._io.pos()
        self.magic = self._io.read_bytes(4)
        self._debug['magic']['end'] = self._io.pos()
        if not self.magic == b"\xB1\x68\xDE\x3A":
            raise kaitaistruct.ValidationNotEqualError(b"\xB1\x68\xDE\x3A", self.magic, self._io, u"/seq/0")
        self._debug['files']['start'] = self._io.pos()
        self.files = []
        i = 0
        while True:
            if not 'arr' in self._debug['files']:
                self._debug['files']['arr'] = []
            self._debug['files']['arr'].append({'start': self._io.pos()})
            _t_files = PcxDcx.PcxOffset(self._io, self, self._root)
            _t_files._read()
            _ = _t_files
            self.files.append(_)
            self._debug['files']['arr'][len(self.files) - 1]['end'] = self._io.pos()
            if _.ofs_body == 0:
                break
            i += 1
        self._debug['files']['end'] = self._io.pos()

    class PcxOffset(KaitaiStruct):
        SEQ_FIELDS = ["ofs_body"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['ofs_body']['start'] = self._io.pos()
            self.ofs_body = self._io.read_u4le()
            self._debug['ofs_body']['end'] = self._io.pos()

        @property
        def body(self):
            if hasattr(self, '_m_body'):
                return self._m_body if hasattr(self, '_m_body') else None

            if self.ofs_body != 0:
                _pos = self._io.pos()
                self._io.seek(self.ofs_body)
                self._debug['_m_body']['start'] = self._io.pos()
                self._m_body = pcx.Pcx(self._io)
                self._m_body._read()
                self._debug['_m_body']['end'] = self._io.pos()
                self._io.seek(_pos)

            return self._m_body if hasattr(self, '_m_body') else None




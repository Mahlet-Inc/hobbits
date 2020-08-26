# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class HeroesOfMightAndMagicBmp(KaitaiStruct):
    SEQ_FIELDS = ["magic", "width", "height", "data"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['magic']['start'] = self._io.pos()
        self.magic = self._io.read_u2le()
        self._debug['magic']['end'] = self._io.pos()
        self._debug['width']['start'] = self._io.pos()
        self.width = self._io.read_u2le()
        self._debug['width']['end'] = self._io.pos()
        self._debug['height']['start'] = self._io.pos()
        self.height = self._io.read_u2le()
        self._debug['height']['end'] = self._io.pos()
        self._debug['data']['start'] = self._io.pos()
        self.data = self._io.read_bytes((self.width * self.height))
        self._debug['data']['end'] = self._io.pos()



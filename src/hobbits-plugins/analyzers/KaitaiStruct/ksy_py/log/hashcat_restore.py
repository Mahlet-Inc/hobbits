# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class HashcatRestore(KaitaiStruct):
    """
    .. seealso::
       Source - https://hashcat.net/wiki/doku.php?id=restore
    """
    SEQ_FIELDS = ["version", "cwd", "dicts_pos", "masks_pos", "padding", "current_restore_point", "argc", "padding2", "argv"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['version']['start'] = self._io.pos()
        self.version = self._io.read_u4le()
        self._debug['version']['end'] = self._io.pos()
        self._debug['cwd']['start'] = self._io.pos()
        self.cwd = (KaitaiStream.bytes_terminate(self._io.read_bytes(256), 0, False)).decode(u"UTF-8")
        self._debug['cwd']['end'] = self._io.pos()
        self._debug['dicts_pos']['start'] = self._io.pos()
        self.dicts_pos = self._io.read_u4le()
        self._debug['dicts_pos']['end'] = self._io.pos()
        self._debug['masks_pos']['start'] = self._io.pos()
        self.masks_pos = self._io.read_u4le()
        self._debug['masks_pos']['end'] = self._io.pos()
        self._debug['padding']['start'] = self._io.pos()
        self.padding = self._io.read_bytes(4)
        self._debug['padding']['end'] = self._io.pos()
        self._debug['current_restore_point']['start'] = self._io.pos()
        self.current_restore_point = self._io.read_u8le()
        self._debug['current_restore_point']['end'] = self._io.pos()
        self._debug['argc']['start'] = self._io.pos()
        self.argc = self._io.read_u4le()
        self._debug['argc']['end'] = self._io.pos()
        self._debug['padding2']['start'] = self._io.pos()
        self.padding2 = self._io.read_bytes(12)
        self._debug['padding2']['end'] = self._io.pos()
        self._debug['argv']['start'] = self._io.pos()
        self.argv = [None] * (self.argc)
        for i in range(self.argc):
            if not 'arr' in self._debug['argv']:
                self._debug['argv']['arr'] = []
            self._debug['argv']['arr'].append({'start': self._io.pos()})
            self.argv[i] = (self._io.read_bytes_term(10, False, True, True)).decode(u"UTF-8")
            self._debug['argv']['arr'][i]['end'] = self._io.pos()

        self._debug['argv']['end'] = self._io.pos()



# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections
import zlib


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Fallout2Dat(KaitaiStruct):

    class Compression(Enum):
        none = 0
        zlib = 1
    SEQ_FIELDS = []
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        pass

    class Pstr(KaitaiStruct):
        SEQ_FIELDS = ["size", "str"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['size']['start'] = self._io.pos()
            self.size = self._io.read_u4le()
            self._debug['size']['end'] = self._io.pos()
            self._debug['str']['start'] = self._io.pos()
            self.str = (self._io.read_bytes(self.size)).decode(u"ASCII")
            self._debug['str']['end'] = self._io.pos()


    class Footer(KaitaiStruct):
        SEQ_FIELDS = ["index_size", "file_size"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['index_size']['start'] = self._io.pos()
            self.index_size = self._io.read_u4le()
            self._debug['index_size']['end'] = self._io.pos()
            self._debug['file_size']['start'] = self._io.pos()
            self.file_size = self._io.read_u4le()
            self._debug['file_size']['end'] = self._io.pos()


    class Index(KaitaiStruct):
        SEQ_FIELDS = ["file_count", "files"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_count']['start'] = self._io.pos()
            self.file_count = self._io.read_u4le()
            self._debug['file_count']['end'] = self._io.pos()
            self._debug['files']['start'] = self._io.pos()
            self.files = [None] * (self.file_count)
            for i in range(self.file_count):
                if not 'arr' in self._debug['files']:
                    self._debug['files']['arr'] = []
                self._debug['files']['arr'].append({'start': self._io.pos()})
                _t_files = Fallout2Dat.File(self._io, self, self._root)
                _t_files._read()
                self.files[i] = _t_files
                self._debug['files']['arr'][i]['end'] = self._io.pos()

            self._debug['files']['end'] = self._io.pos()


    class File(KaitaiStruct):
        SEQ_FIELDS = ["name", "flags", "size_unpacked", "size_packed", "offset"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['name']['start'] = self._io.pos()
            self.name = Fallout2Dat.Pstr(self._io, self, self._root)
            self.name._read()
            self._debug['name']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = KaitaiStream.resolve_enum(Fallout2Dat.Compression, self._io.read_u1())
            self._debug['flags']['end'] = self._io.pos()
            self._debug['size_unpacked']['start'] = self._io.pos()
            self.size_unpacked = self._io.read_u4le()
            self._debug['size_unpacked']['end'] = self._io.pos()
            self._debug['size_packed']['start'] = self._io.pos()
            self.size_packed = self._io.read_u4le()
            self._debug['size_packed']['end'] = self._io.pos()
            self._debug['offset']['start'] = self._io.pos()
            self.offset = self._io.read_u4le()
            self._debug['offset']['end'] = self._io.pos()

        @property
        def contents_raw(self):
            if hasattr(self, '_m_contents_raw'):
                return self._m_contents_raw if hasattr(self, '_m_contents_raw') else None

            if self.flags == Fallout2Dat.Compression.none:
                io = self._root._io
                _pos = io.pos()
                io.seek(self.offset)
                self._debug['_m_contents_raw']['start'] = io.pos()
                self._m_contents_raw = io.read_bytes(self.size_unpacked)
                self._debug['_m_contents_raw']['end'] = io.pos()
                io.seek(_pos)

            return self._m_contents_raw if hasattr(self, '_m_contents_raw') else None

        @property
        def contents_zlib(self):
            if hasattr(self, '_m_contents_zlib'):
                return self._m_contents_zlib if hasattr(self, '_m_contents_zlib') else None

            if self.flags == Fallout2Dat.Compression.zlib:
                io = self._root._io
                _pos = io.pos()
                io.seek(self.offset)
                self._debug['_m_contents_zlib']['start'] = io.pos()
                self._raw__m_contents_zlib = io.read_bytes(self.size_packed)
                self._m_contents_zlib = zlib.decompress(self._raw__m_contents_zlib)
                self._debug['_m_contents_zlib']['end'] = io.pos()
                io.seek(_pos)

            return self._m_contents_zlib if hasattr(self, '_m_contents_zlib') else None

        @property
        def contents(self):
            if hasattr(self, '_m_contents'):
                return self._m_contents if hasattr(self, '_m_contents') else None

            if  ((self.flags == Fallout2Dat.Compression.zlib) or (self.flags == Fallout2Dat.Compression.none)) :
                self._m_contents = (self.contents_zlib if self.flags == Fallout2Dat.Compression.zlib else self.contents_raw)

            return self._m_contents if hasattr(self, '_m_contents') else None


    @property
    def footer(self):
        if hasattr(self, '_m_footer'):
            return self._m_footer if hasattr(self, '_m_footer') else None

        _pos = self._io.pos()
        self._io.seek((self._io.size() - 8))
        self._debug['_m_footer']['start'] = self._io.pos()
        self._m_footer = Fallout2Dat.Footer(self._io, self, self._root)
        self._m_footer._read()
        self._debug['_m_footer']['end'] = self._io.pos()
        self._io.seek(_pos)
        return self._m_footer if hasattr(self, '_m_footer') else None

    @property
    def index(self):
        if hasattr(self, '_m_index'):
            return self._m_index if hasattr(self, '_m_index') else None

        _pos = self._io.pos()
        self._io.seek(((self._io.size() - 8) - self.footer.index_size))
        self._debug['_m_index']['start'] = self._io.pos()
        self._m_index = Fallout2Dat.Index(self._io, self, self._root)
        self._m_index._read()
        self._debug['_m_index']['end'] = self._io.pos()
        self._io.seek(_pos)
        return self._m_index if hasattr(self, '_m_index') else None



# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class AndroidOpenglShadersCache(KaitaiStruct):
    """Android apps using directly or indirectly OpenGL cache compiled shaders
    into com.android.opengl.shaders_cache file.
    
    .. seealso::
       Source - https://android.googlesource.com/platform/frameworks/native/+/master/opengl/libs/EGL/FileBlobCache.cpp
    """
    SEQ_FIELDS = ["magic", "crc32", "contents"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['magic']['start'] = self._io.pos()
        self.magic = self._io.read_bytes(4)
        self._debug['magic']['end'] = self._io.pos()
        if not self.magic == b"\x45\x47\x4C\x24":
            raise kaitaistruct.ValidationNotEqualError(b"\x45\x47\x4C\x24", self.magic, self._io, u"/seq/0")
        self._debug['crc32']['start'] = self._io.pos()
        self.crc32 = self._io.read_u4le()
        self._debug['crc32']['end'] = self._io.pos()
        self._debug['contents']['start'] = self._io.pos()
        self._raw_contents = self._io.read_bytes_full()
        _io__raw_contents = KaitaiStream(BytesIO(self._raw_contents))
        self.contents = AndroidOpenglShadersCache.Cache(_io__raw_contents, self, self._root)
        self.contents._read()
        self._debug['contents']['end'] = self._io.pos()

    class Alignment(KaitaiStruct):
        SEQ_FIELDS = ["alignment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['alignment']['start'] = self._io.pos()
            self.alignment = self._io.read_bytes(((self._io.pos() + 3) & (~3 - self._io.pos())))
            self._debug['alignment']['end'] = self._io.pos()


    class PrefixedString(KaitaiStruct):
        SEQ_FIELDS = ["len_str", "str", "alignment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['len_str']['start'] = self._io.pos()
            self.len_str = self._io.read_u4le()
            self._debug['len_str']['end'] = self._io.pos()
            self._debug['str']['start'] = self._io.pos()
            self.str = (KaitaiStream.bytes_terminate(self._io.read_bytes(self.len_str), 0, False)).decode(u"ascii")
            self._debug['str']['end'] = self._io.pos()
            self._debug['alignment']['start'] = self._io.pos()
            self.alignment = AndroidOpenglShadersCache.Alignment(self._io, self, self._root)
            self.alignment._read()
            self._debug['alignment']['end'] = self._io.pos()


    class Cache(KaitaiStruct):
        """
        .. seealso::
           Source - https://android.googlesource.com/platform/frameworks/native/+/master/opengl/libs/EGL/BlobCache.cpp
        """
        SEQ_FIELDS = ["magic", "version", "device_version", "num_entries", "build_id", "entries"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic']['start'] = self._io.pos()
            self.magic = self._io.read_bytes(4)
            self._debug['magic']['end'] = self._io.pos()
            if not self.magic == b"\x24\x62\x42\x5F":
                raise kaitaistruct.ValidationNotEqualError(b"\x24\x62\x42\x5F", self.magic, self._io, u"/types/cache/seq/0")
            self._debug['version']['start'] = self._io.pos()
            self.version = self._io.read_u4le()
            self._debug['version']['end'] = self._io.pos()
            self._debug['device_version']['start'] = self._io.pos()
            self.device_version = self._io.read_u4le()
            self._debug['device_version']['end'] = self._io.pos()
            self._debug['num_entries']['start'] = self._io.pos()
            self.num_entries = self._io.read_u4le()
            self._debug['num_entries']['end'] = self._io.pos()
            if self.version >= 3:
                self._debug['build_id']['start'] = self._io.pos()
                self.build_id = AndroidOpenglShadersCache.PrefixedString(self._io, self, self._root)
                self.build_id._read()
                self._debug['build_id']['end'] = self._io.pos()

            self._debug['entries']['start'] = self._io.pos()
            self.entries = [None] * (self.num_entries)
            for i in range(self.num_entries):
                if not 'arr' in self._debug['entries']:
                    self._debug['entries']['arr'] = []
                self._debug['entries']['arr'].append({'start': self._io.pos()})
                _t_entries = AndroidOpenglShadersCache.Cache.Entry(self._io, self, self._root)
                _t_entries._read()
                self.entries[i] = _t_entries
                self._debug['entries']['arr'][i]['end'] = self._io.pos()

            self._debug['entries']['end'] = self._io.pos()

        class Entry(KaitaiStruct):
            SEQ_FIELDS = ["len_key", "len_value", "key", "value", "alignment"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['len_key']['start'] = self._io.pos()
                self.len_key = self._io.read_u4le()
                self._debug['len_key']['end'] = self._io.pos()
                self._debug['len_value']['start'] = self._io.pos()
                self.len_value = self._io.read_u4le()
                self._debug['len_value']['end'] = self._io.pos()
                self._debug['key']['start'] = self._io.pos()
                self.key = self._io.read_bytes(self.len_key)
                self._debug['key']['end'] = self._io.pos()
                self._debug['value']['start'] = self._io.pos()
                self.value = self._io.read_bytes(self.len_value)
                self._debug['value']['end'] = self._io.pos()
                self._debug['alignment']['start'] = self._io.pos()
                self.alignment = AndroidOpenglShadersCache.Alignment(self._io, self, self._root)
                self.alignment._read()
                self._debug['alignment']['end'] = self._io.pos()





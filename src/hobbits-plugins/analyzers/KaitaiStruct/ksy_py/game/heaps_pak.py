# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class HeapsPak(KaitaiStruct):
    """
    .. seealso::
       Source - https://github.com/HeapsIO/heaps/blob/2bbc2b386952dfd8856c04a854bb706a52cb4b58/hxd/fmt/pak/Reader.hx
    """
    SEQ_FIELDS = ["header"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['header']['start'] = self._io.pos()
        self.header = HeapsPak.Header(self._io, self, self._root)
        self.header._read()
        self._debug['header']['end'] = self._io.pos()

    class Header(KaitaiStruct):
        SEQ_FIELDS = ["magic1", "version", "len_header", "len_data", "root_entry", "magic2"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic1']['start'] = self._io.pos()
            self.magic1 = self._io.read_bytes(3)
            self._debug['magic1']['end'] = self._io.pos()
            if not self.magic1 == b"\x50\x41\x4B":
                raise kaitaistruct.ValidationNotEqualError(b"\x50\x41\x4B", self.magic1, self._io, u"/types/header/seq/0")
            self._debug['version']['start'] = self._io.pos()
            self.version = self._io.read_u1()
            self._debug['version']['end'] = self._io.pos()
            self._debug['len_header']['start'] = self._io.pos()
            self.len_header = self._io.read_u4le()
            self._debug['len_header']['end'] = self._io.pos()
            self._debug['len_data']['start'] = self._io.pos()
            self.len_data = self._io.read_u4le()
            self._debug['len_data']['end'] = self._io.pos()
            self._debug['root_entry']['start'] = self._io.pos()
            self._raw_root_entry = self._io.read_bytes((self.len_header - 16))
            _io__raw_root_entry = KaitaiStream(BytesIO(self._raw_root_entry))
            self.root_entry = HeapsPak.Header.Entry(_io__raw_root_entry, self, self._root)
            self.root_entry._read()
            self._debug['root_entry']['end'] = self._io.pos()
            self._debug['magic2']['start'] = self._io.pos()
            self.magic2 = self._io.read_bytes(4)
            self._debug['magic2']['end'] = self._io.pos()
            if not self.magic2 == b"\x44\x41\x54\x41":
                raise kaitaistruct.ValidationNotEqualError(b"\x44\x41\x54\x41", self.magic2, self._io, u"/types/header/seq/5")

        class Entry(KaitaiStruct):
            """
            .. seealso::
               Source - https://github.com/HeapsIO/heaps/blob/2bbc2b386952dfd8856c04a854bb706a52cb4b58/hxd/fmt/pak/Data.hx
            """
            SEQ_FIELDS = ["len_name", "name", "flags", "body"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['len_name']['start'] = self._io.pos()
                self.len_name = self._io.read_u1()
                self._debug['len_name']['end'] = self._io.pos()
                self._debug['name']['start'] = self._io.pos()
                self.name = (self._io.read_bytes(self.len_name)).decode(u"UTF-8")
                self._debug['name']['end'] = self._io.pos()
                self._debug['flags']['start'] = self._io.pos()
                self.flags = HeapsPak.Header.Entry.Flags(self._io, self, self._root)
                self.flags._read()
                self._debug['flags']['end'] = self._io.pos()
                self._debug['body']['start'] = self._io.pos()
                _on = self.flags.is_dir
                if _on == True:
                    self.body = HeapsPak.Header.Dir(self._io, self, self._root)
                    self.body._read()
                elif _on == False:
                    self.body = HeapsPak.Header.File(self._io, self, self._root)
                    self.body._read()
                self._debug['body']['end'] = self._io.pos()

            class Flags(KaitaiStruct):
                SEQ_FIELDS = ["unused", "is_dir"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['unused']['start'] = self._io.pos()
                    self.unused = self._io.read_bits_int_be(7)
                    self._debug['unused']['end'] = self._io.pos()
                    self._debug['is_dir']['start'] = self._io.pos()
                    self.is_dir = self._io.read_bits_int_be(1) != 0
                    self._debug['is_dir']['end'] = self._io.pos()



        class File(KaitaiStruct):
            SEQ_FIELDS = ["ofs_data", "len_data", "checksum"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['ofs_data']['start'] = self._io.pos()
                self.ofs_data = self._io.read_u4le()
                self._debug['ofs_data']['end'] = self._io.pos()
                self._debug['len_data']['start'] = self._io.pos()
                self.len_data = self._io.read_u4le()
                self._debug['len_data']['end'] = self._io.pos()
                self._debug['checksum']['start'] = self._io.pos()
                self.checksum = self._io.read_bytes(4)
                self._debug['checksum']['end'] = self._io.pos()

            @property
            def data(self):
                if hasattr(self, '_m_data'):
                    return self._m_data if hasattr(self, '_m_data') else None

                io = self._root._io
                _pos = io.pos()
                io.seek((self._root.header.len_header + self.ofs_data))
                self._debug['_m_data']['start'] = io.pos()
                self._m_data = io.read_bytes(self.len_data)
                self._debug['_m_data']['end'] = io.pos()
                io.seek(_pos)
                return self._m_data if hasattr(self, '_m_data') else None


        class Dir(KaitaiStruct):
            SEQ_FIELDS = ["num_entries", "entries"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['num_entries']['start'] = self._io.pos()
                self.num_entries = self._io.read_u4le()
                self._debug['num_entries']['end'] = self._io.pos()
                self._debug['entries']['start'] = self._io.pos()
                self.entries = [None] * (self.num_entries)
                for i in range(self.num_entries):
                    if not 'arr' in self._debug['entries']:
                        self._debug['entries']['arr'] = []
                    self._debug['entries']['arr'].append({'start': self._io.pos()})
                    _t_entries = HeapsPak.Header.Entry(self._io, self, self._root)
                    _t_entries._read()
                    self.entries[i] = _t_entries
                    self._debug['entries']['arr'][i]['end'] = self._io.pos()

                self._debug['entries']['end'] = self._io.pos()





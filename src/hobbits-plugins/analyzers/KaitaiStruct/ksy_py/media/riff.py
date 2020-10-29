# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Riff(KaitaiStruct):
    """The Resource Interchange File Format (RIFF) is a generic file container format
    for storing data in tagged chunks. It is primarily used to store multimedia
    such as sound and video, though it may also be used to store any arbitrary data.
    
    The Microsoft implementation is mostly known through container formats
    like AVI, ANI and WAV, which use RIFF as their basis.
    
    .. seealso::
       Source - https://www.johnloomis.org/cpe102/asgn/asgn1/riff.html
    """

    class Fourcc(Enum):
        riff = 1179011410
        info = 1330007625
        list = 1414744396
    SEQ_FIELDS = ["chunk"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['chunk']['start'] = self._io.pos()
        self.chunk = Riff.Chunk(self._io, self, self._root)
        self.chunk._read()
        self._debug['chunk']['end'] = self._io.pos()

    class ListChunkData(KaitaiStruct):
        SEQ_FIELDS = ["save_parent_chunk_data_ofs", "parent_chunk_data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            if self.parent_chunk_data_ofs < 0:
                self._debug['save_parent_chunk_data_ofs']['start'] = self._io.pos()
                self.save_parent_chunk_data_ofs = self._io.read_bytes(0)
                self._debug['save_parent_chunk_data_ofs']['end'] = self._io.pos()

            self._debug['parent_chunk_data']['start'] = self._io.pos()
            self.parent_chunk_data = Riff.ParentChunkData(self._io, self, self._root)
            self.parent_chunk_data._read()
            self._debug['parent_chunk_data']['end'] = self._io.pos()

        @property
        def parent_chunk_data_ofs(self):
            if hasattr(self, '_m_parent_chunk_data_ofs'):
                return self._m_parent_chunk_data_ofs if hasattr(self, '_m_parent_chunk_data_ofs') else None

            self._m_parent_chunk_data_ofs = self._io.pos()
            return self._m_parent_chunk_data_ofs if hasattr(self, '_m_parent_chunk_data_ofs') else None

        @property
        def form_type(self):
            if hasattr(self, '_m_form_type'):
                return self._m_form_type if hasattr(self, '_m_form_type') else None

            self._m_form_type = KaitaiStream.resolve_enum(Riff.Fourcc, self.parent_chunk_data.form_type)
            return self._m_form_type if hasattr(self, '_m_form_type') else None

        @property
        def form_type_readable(self):
            if hasattr(self, '_m_form_type_readable'):
                return self._m_form_type_readable if hasattr(self, '_m_form_type_readable') else None

            _pos = self._io.pos()
            self._io.seek(self.parent_chunk_data_ofs)
            self._debug['_m_form_type_readable']['start'] = self._io.pos()
            self._m_form_type_readable = (self._io.read_bytes(4)).decode(u"ASCII")
            self._debug['_m_form_type_readable']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_form_type_readable if hasattr(self, '_m_form_type_readable') else None

        @property
        def subchunks(self):
            if hasattr(self, '_m_subchunks'):
                return self._m_subchunks if hasattr(self, '_m_subchunks') else None

            io = self.parent_chunk_data.subchunks_slot._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_subchunks']['start'] = io.pos()
            self._m_subchunks = []
            i = 0
            while not io.is_eof():
                if not 'arr' in self._debug['_m_subchunks']:
                    self._debug['_m_subchunks']['arr'] = []
                self._debug['_m_subchunks']['arr'].append({'start': io.pos()})
                _on = self.form_type
                if _on == Riff.Fourcc.info:
                    if not 'arr' in self._debug['_m_subchunks']:
                        self._debug['_m_subchunks']['arr'] = []
                    self._debug['_m_subchunks']['arr'].append({'start': io.pos()})
                    _t__m_subchunks = Riff.InfoSubchunk(io, self, self._root)
                    _t__m_subchunks._read()
                    self._m_subchunks.append(_t__m_subchunks)
                    self._debug['_m_subchunks']['arr'][len(self._m_subchunks) - 1]['end'] = io.pos()
                else:
                    if not 'arr' in self._debug['_m_subchunks']:
                        self._debug['_m_subchunks']['arr'] = []
                    self._debug['_m_subchunks']['arr'].append({'start': io.pos()})
                    _t__m_subchunks = Riff.ChunkType(io, self, self._root)
                    _t__m_subchunks._read()
                    self._m_subchunks.append(_t__m_subchunks)
                    self._debug['_m_subchunks']['arr'][len(self._m_subchunks) - 1]['end'] = io.pos()
                self._debug['_m_subchunks']['arr'][len(self._m_subchunks) - 1]['end'] = io.pos()
                i += 1

            self._debug['_m_subchunks']['end'] = io.pos()
            io.seek(_pos)
            return self._m_subchunks if hasattr(self, '_m_subchunks') else None


    class Chunk(KaitaiStruct):
        SEQ_FIELDS = ["id", "len", "data_slot", "pad_byte"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['id']['start'] = self._io.pos()
            self.id = self._io.read_u4le()
            self._debug['id']['end'] = self._io.pos()
            self._debug['len']['start'] = self._io.pos()
            self.len = self._io.read_u4le()
            self._debug['len']['end'] = self._io.pos()
            self._debug['data_slot']['start'] = self._io.pos()
            self._raw_data_slot = self._io.read_bytes(self.len)
            _io__raw_data_slot = KaitaiStream(BytesIO(self._raw_data_slot))
            self.data_slot = Riff.Chunk.Slot(_io__raw_data_slot, self, self._root)
            self.data_slot._read()
            self._debug['data_slot']['end'] = self._io.pos()
            self._debug['pad_byte']['start'] = self._io.pos()
            self.pad_byte = self._io.read_bytes((self.len % 2))
            self._debug['pad_byte']['end'] = self._io.pos()

        class Slot(KaitaiStruct):
            SEQ_FIELDS = []
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                pass



    class ParentChunkData(KaitaiStruct):
        SEQ_FIELDS = ["form_type", "subchunks_slot"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['form_type']['start'] = self._io.pos()
            self.form_type = self._io.read_u4le()
            self._debug['form_type']['end'] = self._io.pos()
            self._debug['subchunks_slot']['start'] = self._io.pos()
            self._raw_subchunks_slot = self._io.read_bytes_full()
            _io__raw_subchunks_slot = KaitaiStream(BytesIO(self._raw_subchunks_slot))
            self.subchunks_slot = Riff.ParentChunkData.Slot(_io__raw_subchunks_slot, self, self._root)
            self.subchunks_slot._read()
            self._debug['subchunks_slot']['end'] = self._io.pos()

        class Slot(KaitaiStruct):
            SEQ_FIELDS = []
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                pass



    class InfoSubchunk(KaitaiStruct):
        """All registered subchunks in the INFO chunk are NULL-terminated strings,
        but the unregistered might not be. By convention, the registered
        chunk IDs are in uppercase and the unregistered IDs are in lowercase.
        
        If the chunk ID of an INFO subchunk contains a lowercase
        letter, this chunk is considered as unregistered and thus we can make
        no assumptions about the type of data.
        """
        SEQ_FIELDS = ["save_chunk_ofs", "chunk"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            if self.chunk_ofs < 0:
                self._debug['save_chunk_ofs']['start'] = self._io.pos()
                self.save_chunk_ofs = self._io.read_bytes(0)
                self._debug['save_chunk_ofs']['end'] = self._io.pos()

            self._debug['chunk']['start'] = self._io.pos()
            self.chunk = Riff.Chunk(self._io, self, self._root)
            self.chunk._read()
            self._debug['chunk']['end'] = self._io.pos()

        @property
        def chunk_data(self):
            if hasattr(self, '_m_chunk_data'):
                return self._m_chunk_data if hasattr(self, '_m_chunk_data') else None

            io = self.chunk.data_slot._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_chunk_data']['start'] = io.pos()
            _on = self.is_unregistered_tag
            if _on == False:
                self._m_chunk_data = (io.read_bytes_term(0, False, True, True)).decode(u"UTF-8")
            self._debug['_m_chunk_data']['end'] = io.pos()
            io.seek(_pos)
            return self._m_chunk_data if hasattr(self, '_m_chunk_data') else None

        @property
        def is_unregistered_tag(self):
            """Check if chunk_id contains lowercase characters ([a-z], ASCII 97 = a, ASCII 122 = z).
            """
            if hasattr(self, '_m_is_unregistered_tag'):
                return self._m_is_unregistered_tag if hasattr(self, '_m_is_unregistered_tag') else None

            self._m_is_unregistered_tag =  (( ((KaitaiStream.byte_array_index(self.id_chars, 0) >= 97) and (KaitaiStream.byte_array_index(self.id_chars, 0) <= 122)) ) or ( ((KaitaiStream.byte_array_index(self.id_chars, 1) >= 97) and (KaitaiStream.byte_array_index(self.id_chars, 1) <= 122)) ) or ( ((KaitaiStream.byte_array_index(self.id_chars, 2) >= 97) and (KaitaiStream.byte_array_index(self.id_chars, 2) <= 122)) ) or ( ((KaitaiStream.byte_array_index(self.id_chars, 3) >= 97) and (KaitaiStream.byte_array_index(self.id_chars, 3) <= 122)) )) 
            return self._m_is_unregistered_tag if hasattr(self, '_m_is_unregistered_tag') else None

        @property
        def id_chars(self):
            if hasattr(self, '_m_id_chars'):
                return self._m_id_chars if hasattr(self, '_m_id_chars') else None

            _pos = self._io.pos()
            self._io.seek(self.chunk_ofs)
            self._debug['_m_id_chars']['start'] = self._io.pos()
            self._m_id_chars = self._io.read_bytes(4)
            self._debug['_m_id_chars']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_id_chars if hasattr(self, '_m_id_chars') else None

        @property
        def chunk_id_readable(self):
            if hasattr(self, '_m_chunk_id_readable'):
                return self._m_chunk_id_readable if hasattr(self, '_m_chunk_id_readable') else None

            self._m_chunk_id_readable = (self.id_chars).decode(u"ASCII")
            return self._m_chunk_id_readable if hasattr(self, '_m_chunk_id_readable') else None

        @property
        def chunk_ofs(self):
            if hasattr(self, '_m_chunk_ofs'):
                return self._m_chunk_ofs if hasattr(self, '_m_chunk_ofs') else None

            self._m_chunk_ofs = self._io.pos()
            return self._m_chunk_ofs if hasattr(self, '_m_chunk_ofs') else None


    class ChunkType(KaitaiStruct):
        SEQ_FIELDS = ["save_chunk_ofs", "chunk"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            if self.chunk_ofs < 0:
                self._debug['save_chunk_ofs']['start'] = self._io.pos()
                self.save_chunk_ofs = self._io.read_bytes(0)
                self._debug['save_chunk_ofs']['end'] = self._io.pos()

            self._debug['chunk']['start'] = self._io.pos()
            self.chunk = Riff.Chunk(self._io, self, self._root)
            self.chunk._read()
            self._debug['chunk']['end'] = self._io.pos()

        @property
        def chunk_ofs(self):
            if hasattr(self, '_m_chunk_ofs'):
                return self._m_chunk_ofs if hasattr(self, '_m_chunk_ofs') else None

            self._m_chunk_ofs = self._io.pos()
            return self._m_chunk_ofs if hasattr(self, '_m_chunk_ofs') else None

        @property
        def chunk_id(self):
            if hasattr(self, '_m_chunk_id'):
                return self._m_chunk_id if hasattr(self, '_m_chunk_id') else None

            self._m_chunk_id = KaitaiStream.resolve_enum(Riff.Fourcc, self.chunk.id)
            return self._m_chunk_id if hasattr(self, '_m_chunk_id') else None

        @property
        def chunk_id_readable(self):
            if hasattr(self, '_m_chunk_id_readable'):
                return self._m_chunk_id_readable if hasattr(self, '_m_chunk_id_readable') else None

            _pos = self._io.pos()
            self._io.seek(self.chunk_ofs)
            self._debug['_m_chunk_id_readable']['start'] = self._io.pos()
            self._m_chunk_id_readable = (self._io.read_bytes(4)).decode(u"ASCII")
            self._debug['_m_chunk_id_readable']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_chunk_id_readable if hasattr(self, '_m_chunk_id_readable') else None

        @property
        def chunk_data(self):
            if hasattr(self, '_m_chunk_data'):
                return self._m_chunk_data if hasattr(self, '_m_chunk_data') else None

            io = self.chunk.data_slot._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_chunk_data']['start'] = io.pos()
            _on = self.chunk_id
            if _on == Riff.Fourcc.list:
                self._m_chunk_data = Riff.ListChunkData(io, self, self._root)
                self._m_chunk_data._read()
            self._debug['_m_chunk_data']['end'] = io.pos()
            io.seek(_pos)
            return self._m_chunk_data if hasattr(self, '_m_chunk_data') else None


    @property
    def chunk_id(self):
        if hasattr(self, '_m_chunk_id'):
            return self._m_chunk_id if hasattr(self, '_m_chunk_id') else None

        self._m_chunk_id = KaitaiStream.resolve_enum(Riff.Fourcc, self.chunk.id)
        return self._m_chunk_id if hasattr(self, '_m_chunk_id') else None

    @property
    def is_riff_chunk(self):
        if hasattr(self, '_m_is_riff_chunk'):
            return self._m_is_riff_chunk if hasattr(self, '_m_is_riff_chunk') else None

        self._m_is_riff_chunk = self.chunk_id == Riff.Fourcc.riff
        return self._m_is_riff_chunk if hasattr(self, '_m_is_riff_chunk') else None

    @property
    def parent_chunk_data(self):
        if hasattr(self, '_m_parent_chunk_data'):
            return self._m_parent_chunk_data if hasattr(self, '_m_parent_chunk_data') else None

        if self.is_riff_chunk:
            io = self.chunk.data_slot._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_parent_chunk_data']['start'] = io.pos()
            self._m_parent_chunk_data = Riff.ParentChunkData(io, self, self._root)
            self._m_parent_chunk_data._read()
            self._debug['_m_parent_chunk_data']['end'] = io.pos()
            io.seek(_pos)

        return self._m_parent_chunk_data if hasattr(self, '_m_parent_chunk_data') else None

    @property
    def subchunks(self):
        if hasattr(self, '_m_subchunks'):
            return self._m_subchunks if hasattr(self, '_m_subchunks') else None

        if self.is_riff_chunk:
            io = self.parent_chunk_data.subchunks_slot._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_subchunks']['start'] = io.pos()
            self._m_subchunks = []
            i = 0
            while not io.is_eof():
                if not 'arr' in self._debug['_m_subchunks']:
                    self._debug['_m_subchunks']['arr'] = []
                self._debug['_m_subchunks']['arr'].append({'start': io.pos()})
                _t__m_subchunks = Riff.ChunkType(io, self, self._root)
                _t__m_subchunks._read()
                self._m_subchunks.append(_t__m_subchunks)
                self._debug['_m_subchunks']['arr'][len(self._m_subchunks) - 1]['end'] = io.pos()
                i += 1

            self._debug['_m_subchunks']['end'] = io.pos()
            io.seek(_pos)

        return self._m_subchunks if hasattr(self, '_m_subchunks') else None



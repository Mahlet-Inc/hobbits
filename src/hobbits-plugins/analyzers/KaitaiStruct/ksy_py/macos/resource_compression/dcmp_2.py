# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Dcmp2(KaitaiStruct):
    """Compressed resource data in `'dcmp' (2)` format,
    as stored in compressed resources with header type `9` and decompressor ID `2`.
    
    The `'dcmp' (2)` decompressor resource is included in the System file of System 7.0 and later.
    This compression format is used for a few compressed resources in System 7.0's files
    (such as the System file).
    This decompressor is also included with and used by some other Apple applications,
    such as ResEdit.
    (Note: ResEdit includes the `'dcmp' (2)` resource,
    but none of its resources actually use this decompressor.)
    
    This compression format is based on simple dictionary coding,
    where each byte in the compressed data expands to two bytes,
    based on a lookup table
    (either included in the compressed data or provided by the decompressor).
    An alternative "tagged" compression format is also supported,
    which allows using two-byte literals in addition to single-byte table references,
    at the cost of requiring an extra "tag" byte every 16 output bytes,
    to differentiate literals and table references.
    
    .. seealso::
       Source - https://github.com/dgelessus/python-rsrcfork/tree/master/rsrcfork/compress/dcmp2.py
    """
    SEQ_FIELDS = ["custom_lookup_table", "data", "last_byte"]
    def __init__(self, len_decompressed, header_parameters_with_io, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self.len_decompressed = len_decompressed
        self.header_parameters_with_io = header_parameters_with_io
        self._debug = collections.defaultdict(dict)

    def _read(self):
        if self.header_parameters.flags.has_custom_lookup_table:
            self._debug['custom_lookup_table']['start'] = self._io.pos()
            self.custom_lookup_table = [None] * (self.header_parameters.num_custom_lookup_table_entries)
            for i in range(self.header_parameters.num_custom_lookup_table_entries):
                if not 'arr' in self._debug['custom_lookup_table']:
                    self._debug['custom_lookup_table']['arr'] = []
                self._debug['custom_lookup_table']['arr'].append({'start': self._io.pos()})
                self.custom_lookup_table[i] = self._io.read_bytes(2)
                self._debug['custom_lookup_table']['arr'][i]['end'] = self._io.pos()

            self._debug['custom_lookup_table']['end'] = self._io.pos()

        self._debug['data']['start'] = self._io.pos()
        _on = self.header_parameters.flags.tagged
        if _on == True:
            self._raw_data = self._io.read_bytes(((self._io.size() - self._io.pos()) - (1 if self.is_len_decompressed_odd else 0)))
            _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
            self.data = Dcmp2.TaggedData(_io__raw_data, self, self._root)
            self.data._read()
        else:
            self._raw_data = self._io.read_bytes(((self._io.size() - self._io.pos()) - (1 if self.is_len_decompressed_odd else 0)))
            _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
            self.data = Dcmp2.UntaggedData(_io__raw_data, self, self._root)
            self.data._read()
        self._debug['data']['end'] = self._io.pos()
        if self.is_len_decompressed_odd:
            self._debug['last_byte']['start'] = self._io.pos()
            self.last_byte = self._io.read_bytes(1)
            self._debug['last_byte']['end'] = self._io.pos()


    class HeaderParameters(KaitaiStruct):
        """Decompressor-specific parameters for this compression format,
        as stored in the compressed resource header.
        """
        SEQ_FIELDS = ["unknown", "num_custom_lookup_table_entries_m1", "flags"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['unknown']['start'] = self._io.pos()
            self.unknown = self._io.read_u2be()
            self._debug['unknown']['end'] = self._io.pos()
            self._debug['num_custom_lookup_table_entries_m1']['start'] = self._io.pos()
            self.num_custom_lookup_table_entries_m1 = self._io.read_u1()
            self._debug['num_custom_lookup_table_entries_m1']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = Dcmp2.HeaderParameters.Flags(self._io, self, self._root)
            self.flags._read()
            self._debug['flags']['end'] = self._io.pos()

        class Flags(KaitaiStruct):
            """Flags for the decompressor,
            as stored in the decompressor-specific parameters.
            """
            SEQ_FIELDS = ["reserved", "tagged", "has_custom_lookup_table"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_bits_int_be(6)
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['tagged']['start'] = self._io.pos()
                self.tagged = self._io.read_bits_int_be(1) != 0
                self._debug['tagged']['end'] = self._io.pos()
                self._debug['has_custom_lookup_table']['start'] = self._io.pos()
                self.has_custom_lookup_table = self._io.read_bits_int_be(1) != 0
                self._debug['has_custom_lookup_table']['end'] = self._io.pos()

            @property
            def as_int(self):
                """The flags as a packed integer,
                as they are stored in the data.
                """
                if hasattr(self, '_m_as_int'):
                    return self._m_as_int if hasattr(self, '_m_as_int') else None

                _pos = self._io.pos()
                self._io.seek(0)
                self._debug['_m_as_int']['start'] = self._io.pos()
                self._m_as_int = self._io.read_u1()
                self._debug['_m_as_int']['end'] = self._io.pos()
                self._io.seek(_pos)
                return self._m_as_int if hasattr(self, '_m_as_int') else None


        @property
        def num_custom_lookup_table_entries(self):
            """The number of entries in the custom lookup table.
            Only used if a custom lookup table is present.
            """
            if hasattr(self, '_m_num_custom_lookup_table_entries'):
                return self._m_num_custom_lookup_table_entries if hasattr(self, '_m_num_custom_lookup_table_entries') else None

            if self.flags.has_custom_lookup_table:
                self._m_num_custom_lookup_table_entries = (self.num_custom_lookup_table_entries_m1 + 1)

            return self._m_num_custom_lookup_table_entries if hasattr(self, '_m_num_custom_lookup_table_entries') else None


    class UntaggedData(KaitaiStruct):
        """Compressed data in the "untagged" variant of the format.
        """
        SEQ_FIELDS = ["table_references"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['table_references']['start'] = self._io.pos()
            self.table_references = []
            i = 0
            while not self._io.is_eof():
                if not 'arr' in self._debug['table_references']:
                    self._debug['table_references']['arr'] = []
                self._debug['table_references']['arr'].append({'start': self._io.pos()})
                self.table_references.append(self._io.read_u1())
                self._debug['table_references']['arr'][len(self.table_references) - 1]['end'] = self._io.pos()
                i += 1

            self._debug['table_references']['end'] = self._io.pos()


    class TaggedData(KaitaiStruct):
        """Compressed data in the "tagged" variant of the format.
        """
        SEQ_FIELDS = ["chunks"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['chunks']['start'] = self._io.pos()
            self.chunks = []
            i = 0
            while not self._io.is_eof():
                if not 'arr' in self._debug['chunks']:
                    self._debug['chunks']['arr'] = []
                self._debug['chunks']['arr'].append({'start': self._io.pos()})
                _t_chunks = Dcmp2.TaggedData.Chunk(self._io, self, self._root)
                _t_chunks._read()
                self.chunks.append(_t_chunks)
                self._debug['chunks']['arr'][len(self.chunks) - 1]['end'] = self._io.pos()
                i += 1

            self._debug['chunks']['end'] = self._io.pos()

        class Chunk(KaitaiStruct):
            """A single tagged chunk of compressed data.
            
            Each chunk expands to 16 bytes of decompressed data.
            In compressed form,
            the chunks have a variable length
            (between 9 and 17 bytes)
            depending on the value of the tag byte.
            """
            SEQ_FIELDS = ["tag", "units"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['tag']['start'] = self._io.pos()
                self.tag = [None] * (8)
                for i in range(8):
                    if not 'arr' in self._debug['tag']:
                        self._debug['tag']['arr'] = []
                    self._debug['tag']['arr'].append({'start': self._io.pos()})
                    self.tag[i] = self._io.read_bits_int_be(1) != 0
                    self._debug['tag']['arr'][i]['end'] = self._io.pos()

                self._debug['tag']['end'] = self._io.pos()
                self._io.align_to_byte()
                self._debug['units']['start'] = self._io.pos()
                self._raw_units = []
                self.units = []
                i = 0
                while True:
                    if not 'arr' in self._debug['units']:
                        self._debug['units']['arr'] = []
                    self._debug['units']['arr'].append({'start': self._io.pos()})
                    _on = self.tag[i]
                    if _on == True:
                        if not 'arr' in self._debug['units']:
                            self._debug['units']['arr'] = []
                        self._debug['units']['arr'].append({'start': self._io.pos()})
                        _ = self._io.read_u1()
                        self.units.append(_)
                        self._debug['units']['arr'][len(self.units) - 1]['end'] = self._io.pos()
                    else:
                        if not 'arr' in self._debug['units']:
                            self._debug['units']['arr'] = []
                        self._debug['units']['arr'].append({'start': self._io.pos()})
                        _ = self._io.read_bytes((1 if self.tag[i] else 2))
                        self.units.append(_)
                        self._debug['units']['arr'][len(self.units) - 1]['end'] = self._io.pos()
                    self._debug['units']['arr'][len(self.units) - 1]['end'] = self._io.pos()
                    if  ((i >= 7) or (self._io.is_eof())) :
                        break
                    i += 1
                self._debug['units']['end'] = self._io.pos()



    @property
    def header_parameters(self):
        """The parsed decompressor-specific parameters from the compressed resource header.
        """
        if hasattr(self, '_m_header_parameters'):
            return self._m_header_parameters if hasattr(self, '_m_header_parameters') else None

        io = self.header_parameters_with_io._io
        _pos = io.pos()
        io.seek(0)
        self._debug['_m_header_parameters']['start'] = io.pos()
        self._m_header_parameters = Dcmp2.HeaderParameters(io, self, self._root)
        self._m_header_parameters._read()
        self._debug['_m_header_parameters']['end'] = io.pos()
        io.seek(_pos)
        return self._m_header_parameters if hasattr(self, '_m_header_parameters') else None

    @property
    def is_len_decompressed_odd(self):
        """Whether the length of the decompressed data is odd.
        This affects the meaning of the last byte of the compressed data.
        """
        if hasattr(self, '_m_is_len_decompressed_odd'):
            return self._m_is_len_decompressed_odd if hasattr(self, '_m_is_len_decompressed_odd') else None

        self._m_is_len_decompressed_odd = (self.len_decompressed % 2) != 0
        return self._m_is_len_decompressed_odd if hasattr(self, '_m_is_len_decompressed_odd') else None

    @property
    def default_lookup_table(self):
        """The default lookup table,
        which is used if no custom lookup table is included with the compressed data.
        """
        if hasattr(self, '_m_default_lookup_table'):
            return self._m_default_lookup_table if hasattr(self, '_m_default_lookup_table') else None

        self._m_default_lookup_table = [b"\x00\x00", b"\x00\x08", b"\x4E\xBA", b"\x20\x6E", b"\x4E\x75", b"\x00\x0C", b"\x00\x04", b"\x70\x00", b"\x00\x10", b"\x00\x02", b"\x48\x6E", b"\xFF\xFC", b"\x60\x00", b"\x00\x01", b"\x48\xE7", b"\x2F\x2E", b"\x4E\x56", b"\x00\x06", b"\x4E\x5E", b"\x2F\x00", b"\x61\x00", b"\xFF\xF8", b"\x2F\x0B", b"\xFF\xFF", b"\x00\x14", b"\x00\x0A", b"\x00\x18", b"\x20\x5F", b"\x00\x0E", b"\x20\x50", b"\x3F\x3C", b"\xFF\xF4", b"\x4C\xEE", b"\x30\x2E", b"\x67\x00", b"\x4C\xDF", b"\x26\x6E", b"\x00\x12", b"\x00\x1C", b"\x42\x67", b"\xFF\xF0", b"\x30\x3C", b"\x2F\x0C", b"\x00\x03", b"\x4E\xD0", b"\x00\x20", b"\x70\x01", b"\x00\x16", b"\x2D\x40", b"\x48\xC0", b"\x20\x78", b"\x72\x00", b"\x58\x8F", b"\x66\x00", b"\x4F\xEF", b"\x42\xA7", b"\x67\x06", b"\xFF\xFA", b"\x55\x8F", b"\x28\x6E", b"\x3F\x00", b"\xFF\xFE", b"\x2F\x3C", b"\x67\x04", b"\x59\x8F", b"\x20\x6B", b"\x00\x24", b"\x20\x1F", b"\x41\xFA", b"\x81\xE1", b"\x66\x04", b"\x67\x08", b"\x00\x1A", b"\x4E\xB9", b"\x50\x8F", b"\x20\x2E", b"\x00\x07", b"\x4E\xB0", b"\xFF\xF2", b"\x3D\x40", b"\x00\x1E", b"\x20\x68", b"\x66\x06", b"\xFF\xF6", b"\x4E\xF9", b"\x08\x00", b"\x0C\x40", b"\x3D\x7C", b"\xFF\xEC", b"\x00\x05", b"\x20\x3C", b"\xFF\xE8", b"\xDE\xFC", b"\x4A\x2E", b"\x00\x30", b"\x00\x28", b"\x2F\x08", b"\x20\x0B", b"\x60\x02", b"\x42\x6E", b"\x2D\x48", b"\x20\x53", b"\x20\x40", b"\x18\x00", b"\x60\x04", b"\x41\xEE", b"\x2F\x28", b"\x2F\x01", b"\x67\x0A", b"\x48\x40", b"\x20\x07", b"\x66\x08", b"\x01\x18", b"\x2F\x07", b"\x30\x28", b"\x3F\x2E", b"\x30\x2B", b"\x22\x6E", b"\x2F\x2B", b"\x00\x2C", b"\x67\x0C", b"\x22\x5F", b"\x60\x06", b"\x00\xFF", b"\x30\x07", b"\xFF\xEE", b"\x53\x40", b"\x00\x40", b"\xFF\xE4", b"\x4A\x40", b"\x66\x0A", b"\x00\x0F", b"\x4E\xAD", b"\x70\xFF", b"\x22\xD8", b"\x48\x6B", b"\x00\x22", b"\x20\x4B", b"\x67\x0E", b"\x4A\xAE", b"\x4E\x90", b"\xFF\xE0", b"\xFF\xC0", b"\x00\x2A", b"\x27\x40", b"\x67\x02", b"\x51\xC8", b"\x02\xB6", b"\x48\x7A", b"\x22\x78", b"\xB0\x6E", b"\xFF\xE6", b"\x00\x09", b"\x32\x2E", b"\x3E\x00", b"\x48\x41", b"\xFF\xEA", b"\x43\xEE", b"\x4E\x71", b"\x74\x00", b"\x2F\x2C", b"\x20\x6C", b"\x00\x3C", b"\x00\x26", b"\x00\x50", b"\x18\x80", b"\x30\x1F", b"\x22\x00", b"\x66\x0C", b"\xFF\xDA", b"\x00\x38", b"\x66\x02", b"\x30\x2C", b"\x20\x0C", b"\x2D\x6E", b"\x42\x40", b"\xFF\xE2", b"\xA9\xF0", b"\xFF\x00", b"\x37\x7C", b"\xE5\x80", b"\xFF\xDC", b"\x48\x68", b"\x59\x4F", b"\x00\x34", b"\x3E\x1F", b"\x60\x08", b"\x2F\x06", b"\xFF\xDE", b"\x60\x0A", b"\x70\x02", b"\x00\x32", b"\xFF\xCC", b"\x00\x80", b"\x22\x51", b"\x10\x1F", b"\x31\x7C", b"\xA0\x29", b"\xFF\xD8", b"\x52\x40", b"\x01\x00", b"\x67\x10", b"\xA0\x23", b"\xFF\xCE", b"\xFF\xD4", b"\x20\x06", b"\x48\x78", b"\x00\x2E", b"\x50\x4F", b"\x43\xFA", b"\x67\x12", b"\x76\x00", b"\x41\xE8", b"\x4A\x6E", b"\x20\xD9", b"\x00\x5A", b"\x7F\xFF", b"\x51\xCA", b"\x00\x5C", b"\x2E\x00", b"\x02\x40", b"\x48\xC7", b"\x67\x14", b"\x0C\x80", b"\x2E\x9F", b"\xFF\xD6", b"\x80\x00", b"\x10\x00", b"\x48\x42", b"\x4A\x6B", b"\xFF\xD2", b"\x00\x48", b"\x4A\x47", b"\x4E\xD1", b"\x20\x6F", b"\x00\x41", b"\x60\x0C", b"\x2A\x78", b"\x42\x2E", b"\x32\x00", b"\x65\x74", b"\x67\x16", b"\x00\x44", b"\x48\x6D", b"\x20\x08", b"\x48\x6C", b"\x0B\x7C", b"\x26\x40", b"\x04\x00", b"\x00\x68", b"\x20\x6D", b"\x00\x0D", b"\x2A\x40", b"\x00\x0B", b"\x00\x3E", b"\x02\x20"]
        return self._m_default_lookup_table if hasattr(self, '_m_default_lookup_table') else None

    @property
    def lookup_table(self):
        """The lookup table to be used for this compressed data.
        """
        if hasattr(self, '_m_lookup_table'):
            return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None

        self._m_lookup_table = (self.custom_lookup_table if self.header_parameters.flags.has_custom_lookup_table else self.default_lookup_table)
        return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None



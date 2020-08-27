# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import dcmp_variable_length_integer
class Dcmp1(KaitaiStruct):
    """Compressed resource data in `'dcmp' (1)` format,
    as stored in compressed resources with header type `8` and decompressor ID `1`.
    
    The `'dcmp' (1)` decompressor resource is included in the System file of System 7.0 and later.
    This compression format is used for a few compressed resources in System 7.0's files
    (such as the Finder Help file).
    This decompressor is also included with and used by some other Apple applications,
    such as ResEdit.
    (Note: ResEdit includes the `'dcmp' (1)` resource,
    but none of its resources actually use this decompressor.)
    
    This compression format supports some basic general-purpose compression schemes,
    including backreferences to previous data and run-length encoding.
    It also includes some types of compression tailored specifically to Mac OS resources,
    including a set of single-byte codes that correspond to entries in a hard-coded lookup table.
    
    The `'dcmp' (0)` compression format (see dcmp_0.ksy) is very similar to this format,
    with the main difference that it operates mostly on units of 2 or 4 bytes.
    This makes the ``dcmp' (0)` format more suitable for word-aligned data,
    such as executable code, bitmaps, sounds, etc.
    The `'dcmp' (0)` format also appears to be generally preferred over `'dcmp' (1)`,
    with the latter only being used in resource files that contain mostly unaligned data,
    such as text.
    
    .. seealso::
       Source - https://github.com/dgelessus/python-rsrcfork/tree/master/rsrcfork/compress/dcmp1.py
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
        while True:
            if not 'arr' in self._debug['chunks']:
                self._debug['chunks']['arr'] = []
            self._debug['chunks']['arr'].append({'start': self._io.pos()})
            _t_chunks = self._root.Chunk(self._io, self, self._root)
            _t_chunks._read()
            _ = _t_chunks
            self.chunks.append(_)
            self._debug['chunks']['arr'][len(self.chunks) - 1]['end'] = self._io.pos()
            if _.tag == 255:
                break
            i += 1
        self._debug['chunks']['end'] = self._io.pos()

    class Chunk(KaitaiStruct):
        """A single chunk of compressed data.
        Each chunk in the compressed data expands to a sequence of bytes in the uncompressed data,
        except when `tag == 0xff`,
        which marks the end of the data and does not correspond to any bytes in the uncompressed data.
        
        Most chunks are stateless and always expand to the same data,
        regardless of where the chunk appears in the sequence.
        However,
        some chunks affect the behavior of future chunks,
        or expand to different data depending on which chunks came before them.
        """

        class TagKind(Enum):
            invalid = -1
            literal = 0
            backreference = 1
            table_lookup = 2
            extended = 3
            end = 4
        SEQ_FIELDS = ["tag", "body"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['tag']['start'] = self._io.pos()
            self.tag = self._io.read_u1()
            self._debug['tag']['end'] = self._io.pos()
            self._debug['body']['start'] = self._io.pos()
            _on = (self._root.Chunk.TagKind.literal if  ((self.tag >= 0) and (self.tag <= 31))  else (self._root.Chunk.TagKind.backreference if  ((self.tag >= 32) and (self.tag <= 207))  else (self._root.Chunk.TagKind.literal if  ((self.tag >= 208) and (self.tag <= 209))  else (self._root.Chunk.TagKind.backreference if self.tag == 210 else (self._root.Chunk.TagKind.table_lookup if  ((self.tag >= 213) and (self.tag <= 253))  else (self._root.Chunk.TagKind.extended if self.tag == 254 else (self._root.Chunk.TagKind.end if self.tag == 255 else self._root.Chunk.TagKind.invalid)))))))
            if _on == self._root.Chunk.TagKind.extended:
                self.body = self._root.Chunk.ExtendedBody(self._io, self, self._root)
                self.body._read()
            elif _on == self._root.Chunk.TagKind.literal:
                self.body = self._root.Chunk.LiteralBody(self.tag, self._io, self, self._root)
                self.body._read()
            elif _on == self._root.Chunk.TagKind.end:
                self.body = self._root.Chunk.EndBody(self._io, self, self._root)
                self.body._read()
            elif _on == self._root.Chunk.TagKind.table_lookup:
                self.body = self._root.Chunk.TableLookupBody(self.tag, self._io, self, self._root)
                self.body._read()
            elif _on == self._root.Chunk.TagKind.backreference:
                self.body = self._root.Chunk.BackreferenceBody(self.tag, self._io, self, self._root)
                self.body._read()
            self._debug['body']['end'] = self._io.pos()

        class LiteralBody(KaitaiStruct):
            """The body of a literal data chunk.
            
            The data that this chunk expands to is stored literally in the body (`literal`).
            Optionally,
            the literal data may also be stored for use by future backreference chunks (`do_store`).
            """
            SEQ_FIELDS = ["len_literal_separate", "literal"]
            def __init__(self, tag, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self.tag = tag
                self._debug = collections.defaultdict(dict)

            def _read(self):
                if self.is_len_literal_separate:
                    self._debug['len_literal_separate']['start'] = self._io.pos()
                    self.len_literal_separate = self._io.read_u1()
                    self._debug['len_literal_separate']['end'] = self._io.pos()

                self._debug['literal']['start'] = self._io.pos()
                self.literal = self._io.read_bytes(self.len_literal)
                self._debug['literal']['end'] = self._io.pos()

            @property
            def do_store(self):
                """Whether this literal should be stored for use by future backreference chunks.
                
                See the documentation of the `backreference_body` type for details about backreference chunks.
                """
                if hasattr(self, '_m_do_store'):
                    return self._m_do_store if hasattr(self, '_m_do_store') else None

                self._m_do_store = (self.tag == 209 if self.is_len_literal_separate else (self.tag & 16) != 0)
                return self._m_do_store if hasattr(self, '_m_do_store') else None

            @property
            def len_literal_m1_in_tag(self):
                """The part of the tag byte that indicates the length of the literal data,
                in bytes,
                minus one.
                
                If the tag byte is 0xd0 or 0xd1,
                the length is stored in a separate byte after the tag byte and before the literal data.
                """
                if hasattr(self, '_m_len_literal_m1_in_tag'):
                    return self._m_len_literal_m1_in_tag if hasattr(self, '_m_len_literal_m1_in_tag') else None

                if not (self.is_len_literal_separate):
                    self._m_len_literal_m1_in_tag = (self.tag & 15)

                return self._m_len_literal_m1_in_tag if hasattr(self, '_m_len_literal_m1_in_tag') else None

            @property
            def is_len_literal_separate(self):
                """Whether the length of the literal is stored separately from the tag.
                """
                if hasattr(self, '_m_is_len_literal_separate'):
                    return self._m_is_len_literal_separate if hasattr(self, '_m_is_len_literal_separate') else None

                self._m_is_len_literal_separate = self.tag >= 208
                return self._m_is_len_literal_separate if hasattr(self, '_m_is_len_literal_separate') else None

            @property
            def len_literal(self):
                """The length of the literal data,
                in bytes.
                
                In practice,
                this value is always greater than zero,
                as there is no use in storing a zero-length literal.
                """
                if hasattr(self, '_m_len_literal'):
                    return self._m_len_literal if hasattr(self, '_m_len_literal') else None

                self._m_len_literal = (self.len_literal_separate if self.is_len_literal_separate else (self.len_literal_m1_in_tag + 1))
                return self._m_len_literal if hasattr(self, '_m_len_literal') else None


        class BackreferenceBody(KaitaiStruct):
            """The body of a backreference chunk.
            
            This chunk expands to the data stored in a preceding literal chunk,
            indicated by an index number (`index`).
            """
            SEQ_FIELDS = ["index_separate_minus"]
            def __init__(self, tag, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self.tag = tag
                self._debug = collections.defaultdict(dict)

            def _read(self):
                if self.is_index_separate:
                    self._debug['index_separate_minus']['start'] = self._io.pos()
                    self.index_separate_minus = self._io.read_u1()
                    self._debug['index_separate_minus']['end'] = self._io.pos()


            @property
            def is_index_separate(self):
                """Whether the index is stored separately from the tag.
                """
                if hasattr(self, '_m_is_index_separate'):
                    return self._m_is_index_separate if hasattr(self, '_m_is_index_separate') else None

                self._m_is_index_separate = self.tag == 210
                return self._m_is_index_separate if hasattr(self, '_m_is_index_separate') else None

            @property
            def index_in_tag(self):
                """The index of the referenced literal chunk,
                as stored in the tag byte.
                """
                if hasattr(self, '_m_index_in_tag'):
                    return self._m_index_in_tag if hasattr(self, '_m_index_in_tag') else None

                self._m_index_in_tag = (self.tag - 32)
                return self._m_index_in_tag if hasattr(self, '_m_index_in_tag') else None

            @property
            def index_separate(self):
                """The index of the referenced literal chunk,
                as stored separately from the tag byte,
                with the implicit offset corrected for.
                """
                if hasattr(self, '_m_index_separate'):
                    return self._m_index_separate if hasattr(self, '_m_index_separate') else None

                if self.is_index_separate:
                    self._m_index_separate = (self.index_separate_minus + 176)

                return self._m_index_separate if hasattr(self, '_m_index_separate') else None

            @property
            def index(self):
                """The index of the referenced literal chunk.
                
                Stored literals are assigned index numbers in the order in which they appear in the compressed data,
                starting at 0.
                Non-stored literals are not counted in the numbering and cannot be referenced using backreferences.
                Once an index is assigned to a stored literal,
                it is never changed or unassigned for the entire length of the compressed data.
                
                As the name indicates,
                a backreference can only reference stored literal chunks found *before* the backreference,
                not ones that come after it.
                """
                if hasattr(self, '_m_index'):
                    return self._m_index if hasattr(self, '_m_index') else None

                self._m_index = (self.index_separate if self.is_index_separate else self.index_in_tag)
                return self._m_index if hasattr(self, '_m_index') else None


        class TableLookupBody(KaitaiStruct):
            """The body of a table lookup chunk.
            This body is always empty.
            
            This chunk always expands to two bytes (`value`),
            determined from the tag byte using a fixed lookup table (`lookup_table`).
            This lookup table is hardcoded in the decompressor and always the same for all compressed data.
            """
            SEQ_FIELDS = []
            def __init__(self, tag, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self.tag = tag
                self._debug = collections.defaultdict(dict)

            def _read(self):
                pass

            @property
            def lookup_table(self):
                """Fixed lookup table that maps tag byte numbers to two bytes each.
                
                The entries in the lookup table are offset -
                index 0 stands for tag 0xd5, 1 for 0xd6, etc.
                """
                if hasattr(self, '_m_lookup_table'):
                    return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None

                self._m_lookup_table = [b"\x00\x00", b"\x00\x01", b"\x00\x02", b"\x00\x03", b"\x2E\x01", b"\x3E\x01", b"\x01\x01", b"\x1E\x01", b"\xFF\xFF", b"\x0E\x01", b"\x31\x00", b"\x11\x12", b"\x01\x07", b"\x33\x32", b"\x12\x39", b"\xED\x10", b"\x01\x27", b"\x23\x22", b"\x01\x37", b"\x07\x06", b"\x01\x17", b"\x01\x23", b"\x00\xFF", b"\x00\x2F", b"\x07\x0E", b"\xFD\x3C", b"\x01\x35", b"\x01\x15", b"\x01\x02", b"\x00\x07", b"\x00\x3E", b"\x05\xD5", b"\x02\x01", b"\x06\x07", b"\x07\x08", b"\x30\x01", b"\x01\x33", b"\x00\x10", b"\x17\x16", b"\x37\x3E", b"\x36\x37"]
                return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None

            @property
            def value(self):
                """The two bytes that the tag byte expands to,
                based on the fixed lookup table.
                """
                if hasattr(self, '_m_value'):
                    return self._m_value if hasattr(self, '_m_value') else None

                self._m_value = self.lookup_table[(self.tag - 213)]
                return self._m_value if hasattr(self, '_m_value') else None


        class EndBody(KaitaiStruct):
            """The body of an end chunk.
            This body is always empty.
            
            The last chunk in the compressed data must always be an end chunk.
            An end chunk cannot appear elsewhere in the compressed data.
            """
            SEQ_FIELDS = []
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                pass


        class ExtendedBody(KaitaiStruct):
            """The body of an extended chunk.
            The meaning of this chunk depends on the extended tag byte stored in the chunk data.
            """
            SEQ_FIELDS = ["tag", "body"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['tag']['start'] = self._io.pos()
                self.tag = self._io.read_u1()
                self._debug['tag']['end'] = self._io.pos()
                self._debug['body']['start'] = self._io.pos()
                _on = self.tag
                if _on == 2:
                    self.body = self._root.Chunk.ExtendedBody.RepeatBody(self._io, self, self._root)
                    self.body._read()
                self._debug['body']['end'] = self._io.pos()

            class RepeatBody(KaitaiStruct):
                """The body of a repeat chunk.
                
                This chunk expands to the same byte repeated a number of times,
                i. e. it implements a form of run-length encoding.
                """
                SEQ_FIELDS = ["to_repeat_raw", "repeat_count_m1_raw"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['to_repeat_raw']['start'] = self._io.pos()
                    self.to_repeat_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.to_repeat_raw._read()
                    self._debug['to_repeat_raw']['end'] = self._io.pos()
                    self._debug['repeat_count_m1_raw']['start'] = self._io.pos()
                    self.repeat_count_m1_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.repeat_count_m1_raw._read()
                    self._debug['repeat_count_m1_raw']['end'] = self._io.pos()

                @property
                def to_repeat(self):
                    """The value to repeat.
                    
                    Although it is stored as a variable-length integer,
                    this value must fit into an unsigned 8-bit integer.
                    """
                    if hasattr(self, '_m_to_repeat'):
                        return self._m_to_repeat if hasattr(self, '_m_to_repeat') else None

                    self._m_to_repeat = self.to_repeat_raw.value
                    return self._m_to_repeat if hasattr(self, '_m_to_repeat') else None

                @property
                def repeat_count_m1(self):
                    """The number of times to repeat the value,
                    minus one.
                    
                    This value must not be negative.
                    """
                    if hasattr(self, '_m_repeat_count_m1'):
                        return self._m_repeat_count_m1 if hasattr(self, '_m_repeat_count_m1') else None

                    self._m_repeat_count_m1 = self.repeat_count_m1_raw.value
                    return self._m_repeat_count_m1 if hasattr(self, '_m_repeat_count_m1') else None

                @property
                def repeat_count(self):
                    """The number of times to repeat the value.
                    
                    This value must be positive.
                    """
                    if hasattr(self, '_m_repeat_count'):
                        return self._m_repeat_count if hasattr(self, '_m_repeat_count') else None

                    self._m_repeat_count = (self.repeat_count_m1 + 1)
                    return self._m_repeat_count if hasattr(self, '_m_repeat_count') else None






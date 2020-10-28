# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import dcmp_variable_length_integer
class Dcmp0(KaitaiStruct):
    """Compressed resource data in `'dcmp' (0)` format,
    as stored in compressed resources with header type `8` and decompressor ID `0`.
    
    The `'dcmp' (0)` decompressor resource is included in the System file of System 7.0 and later.
    This compression format is used for most compressed resources in System 7.0's files.
    This decompressor is also included with and used by some other Apple applications,
    such as ResEdit.
    
    This compression format supports some basic general-purpose compression schemes,
    including backreferences to previous data,
    run-length encoding,
    and delta encoding.
    It also includes some types of compression tailored specifically to Mac OS resources,
    including a set of single-byte codes that correspond to entries in a hard-coded lookup table,
    and a specialized kind of delta encoding for segment loader jump tables.
    
    Almost all parts of this compression format operate on units of 2 or 4 bytes.
    As a result,
    it is nearly impossible to store data with an odd length in this format.
    To work around this limitation,
    odd-length resources are padded with an extra byte before compressing them with this format.
    This extra byte is ignored after decompression,
    as the real (odd) length of the resource is stored in the compressed resource header.
    
    The `'dcmp' (1)` compression format (see dcmp_1.ksy) is very similar to this format,
    with the main difference that it operates mostly on single bytes rather than two-byte units.
    
    .. seealso::
       Source - https://github.com/dgelessus/python-rsrcfork/tree/master/rsrcfork/compress/dcmp0.py
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
            _t_chunks = Dcmp0.Chunk(self._io, self, self._root)
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
            _on = (Dcmp0.Chunk.TagKind.literal if  ((self.tag >= 0) and (self.tag <= 31))  else (Dcmp0.Chunk.TagKind.backreference if  ((self.tag >= 32) and (self.tag <= 74))  else (Dcmp0.Chunk.TagKind.table_lookup if  ((self.tag >= 75) and (self.tag <= 253))  else (Dcmp0.Chunk.TagKind.extended if self.tag == 254 else (Dcmp0.Chunk.TagKind.end if self.tag == 255 else Dcmp0.Chunk.TagKind.invalid)))))
            if _on == Dcmp0.Chunk.TagKind.extended:
                self.body = Dcmp0.Chunk.ExtendedBody(self._io, self, self._root)
                self.body._read()
            elif _on == Dcmp0.Chunk.TagKind.literal:
                self.body = Dcmp0.Chunk.LiteralBody(self.tag, self._io, self, self._root)
                self.body._read()
            elif _on == Dcmp0.Chunk.TagKind.end:
                self.body = Dcmp0.Chunk.EndBody(self._io, self, self._root)
                self.body._read()
            elif _on == Dcmp0.Chunk.TagKind.table_lookup:
                self.body = Dcmp0.Chunk.TableLookupBody(self.tag, self._io, self, self._root)
                self.body._read()
            elif _on == Dcmp0.Chunk.TagKind.backreference:
                self.body = Dcmp0.Chunk.BackreferenceBody(self.tag, self._io, self, self._root)
                self.body._read()
            self._debug['body']['end'] = self._io.pos()

        class LiteralBody(KaitaiStruct):
            """The body of a literal data chunk.
            
            The data that this chunk expands to is stored literally in the body (`literal`).
            Optionally,
            the literal data may also be stored for use by future backreference chunks (`do_store`).
            
            The length of the literal data is stored as a number of two-byte units.
            This means that the literal data always has an even length in bytes.
            """
            SEQ_FIELDS = ["len_literal_div2_separate", "literal"]
            def __init__(self, tag, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self.tag = tag
                self._debug = collections.defaultdict(dict)

            def _read(self):
                if self.is_len_literal_div2_separate:
                    self._debug['len_literal_div2_separate']['start'] = self._io.pos()
                    self.len_literal_div2_separate = self._io.read_u1()
                    self._debug['len_literal_div2_separate']['end'] = self._io.pos()

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

                self._m_do_store = (self.tag & 16) != 0
                return self._m_do_store if hasattr(self, '_m_do_store') else None

            @property
            def len_literal_div2(self):
                """The length of the literal data,
                in two-byte units.
                
                In practice,
                this value is always greater than zero,
                as there is no use in storing a zero-length literal.
                """
                if hasattr(self, '_m_len_literal_div2'):
                    return self._m_len_literal_div2 if hasattr(self, '_m_len_literal_div2') else None

                self._m_len_literal_div2 = (self.len_literal_div2_separate if self.is_len_literal_div2_separate else self.len_literal_div2_in_tag)
                return self._m_len_literal_div2 if hasattr(self, '_m_len_literal_div2') else None

            @property
            def len_literal(self):
                """The length of the literal data,
                in bytes.
                """
                if hasattr(self, '_m_len_literal'):
                    return self._m_len_literal if hasattr(self, '_m_len_literal') else None

                self._m_len_literal = (self.len_literal_div2 * 2)
                return self._m_len_literal if hasattr(self, '_m_len_literal') else None

            @property
            def len_literal_div2_in_tag(self):
                """The part of the tag byte that indicates the length of the literal data,
                in two-byte units.
                If this value is 0,
                the length is stored in a separate byte after the tag byte and before the literal data.
                """
                if hasattr(self, '_m_len_literal_div2_in_tag'):
                    return self._m_len_literal_div2_in_tag if hasattr(self, '_m_len_literal_div2_in_tag') else None

                self._m_len_literal_div2_in_tag = (self.tag & 15)
                return self._m_len_literal_div2_in_tag if hasattr(self, '_m_len_literal_div2_in_tag') else None

            @property
            def is_len_literal_div2_separate(self):
                """Whether the length of the literal is stored separately from the tag.
                """
                if hasattr(self, '_m_is_len_literal_div2_separate'):
                    return self._m_is_len_literal_div2_separate if hasattr(self, '_m_is_len_literal_div2_separate') else None

                self._m_is_len_literal_div2_separate = self.len_literal_div2_in_tag == 0
                return self._m_is_len_literal_div2_separate if hasattr(self, '_m_is_len_literal_div2_separate') else None


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
                    _on = self.tag
                    if _on == 32:
                        self.index_separate_minus = self._io.read_u1()
                    elif _on == 33:
                        self.index_separate_minus = self._io.read_u1()
                    elif _on == 34:
                        self.index_separate_minus = self._io.read_u2be()
                    self._debug['index_separate_minus']['end'] = self._io.pos()


            @property
            def is_index_separate(self):
                """Whether the index is stored separately from the tag.
                """
                if hasattr(self, '_m_is_index_separate'):
                    return self._m_is_index_separate if hasattr(self, '_m_is_index_separate') else None

                self._m_is_index_separate =  ((self.tag >= 32) and (self.tag <= 34)) 
                return self._m_is_index_separate if hasattr(self, '_m_is_index_separate') else None

            @property
            def index_in_tag(self):
                """The index of the referenced literal chunk,
                as stored in the tag byte.
                """
                if hasattr(self, '_m_index_in_tag'):
                    return self._m_index_in_tag if hasattr(self, '_m_index_in_tag') else None

                self._m_index_in_tag = (self.tag - 35)
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
                    self._m_index_separate = ((self.index_separate_minus + 40) + (256 if self.tag == 33 else 0))

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
                index 0 stands for tag 0x4b, 1 for 0x4c, etc.
                """
                if hasattr(self, '_m_lookup_table'):
                    return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None

                self._m_lookup_table = [b"\x00\x00", b"\x4E\xBA", b"\x00\x08", b"\x4E\x75", b"\x00\x0C", b"\x4E\xAD", b"\x20\x53", b"\x2F\x0B", b"\x61\x00", b"\x00\x10", b"\x70\x00", b"\x2F\x00", b"\x48\x6E", b"\x20\x50", b"\x20\x6E", b"\x2F\x2E", b"\xFF\xFC", b"\x48\xE7", b"\x3F\x3C", b"\x00\x04", b"\xFF\xF8", b"\x2F\x0C", b"\x20\x06", b"\x4E\xED", b"\x4E\x56", b"\x20\x68", b"\x4E\x5E", b"\x00\x01", b"\x58\x8F", b"\x4F\xEF", b"\x00\x02", b"\x00\x18", b"\x60\x00", b"\xFF\xFF", b"\x50\x8F", b"\x4E\x90", b"\x00\x06", b"\x26\x6E", b"\x00\x14", b"\xFF\xF4", b"\x4C\xEE", b"\x00\x0A", b"\x00\x0E", b"\x41\xEE", b"\x4C\xDF", b"\x48\xC0", b"\xFF\xF0", b"\x2D\x40", b"\x00\x12", b"\x30\x2E", b"\x70\x01", b"\x2F\x28", b"\x20\x54", b"\x67\x00", b"\x00\x20", b"\x00\x1C", b"\x20\x5F", b"\x18\x00", b"\x26\x6F", b"\x48\x78", b"\x00\x16", b"\x41\xFA", b"\x30\x3C", b"\x28\x40", b"\x72\x00", b"\x28\x6E", b"\x20\x0C", b"\x66\x00", b"\x20\x6B", b"\x2F\x07", b"\x55\x8F", b"\x00\x28", b"\xFF\xFE", b"\xFF\xEC", b"\x22\xD8", b"\x20\x0B", b"\x00\x0F", b"\x59\x8F", b"\x2F\x3C", b"\xFF\x00", b"\x01\x18", b"\x81\xE1", b"\x4A\x00", b"\x4E\xB0", b"\xFF\xE8", b"\x48\xC7", b"\x00\x03", b"\x00\x22", b"\x00\x07", b"\x00\x1A", b"\x67\x06", b"\x67\x08", b"\x4E\xF9", b"\x00\x24", b"\x20\x78", b"\x08\x00", b"\x66\x04", b"\x00\x2A", b"\x4E\xD0", b"\x30\x28", b"\x26\x5F", b"\x67\x04", b"\x00\x30", b"\x43\xEE", b"\x3F\x00", b"\x20\x1F", b"\x00\x1E", b"\xFF\xF6", b"\x20\x2E", b"\x42\xA7", b"\x20\x07", b"\xFF\xFA", b"\x60\x02", b"\x3D\x40", b"\x0C\x40", b"\x66\x06", b"\x00\x26", b"\x2D\x48", b"\x2F\x01", b"\x70\xFF", b"\x60\x04", b"\x18\x80", b"\x4A\x40", b"\x00\x40", b"\x00\x2C", b"\x2F\x08", b"\x00\x11", b"\xFF\xE4", b"\x21\x40", b"\x26\x40", b"\xFF\xF2", b"\x42\x6E", b"\x4E\xB9", b"\x3D\x7C", b"\x00\x38", b"\x00\x0D", b"\x60\x06", b"\x42\x2E", b"\x20\x3C", b"\x67\x0C", b"\x2D\x68", b"\x66\x08", b"\x4A\x2E", b"\x4A\xAE", b"\x00\x2E", b"\x48\x40", b"\x22\x5F", b"\x22\x00", b"\x67\x0A", b"\x30\x07", b"\x42\x67", b"\x00\x32", b"\x20\x28", b"\x00\x09", b"\x48\x7A", b"\x02\x00", b"\x2F\x2B", b"\x00\x05", b"\x22\x6E", b"\x66\x02", b"\xE5\x80", b"\x67\x0E", b"\x66\x0A", b"\x00\x50", b"\x3E\x00", b"\x66\x0C", b"\x2E\x00", b"\xFF\xEE", b"\x20\x6D", b"\x20\x40", b"\xFF\xE0", b"\x53\x40", b"\x60\x08", b"\x04\x80", b"\x00\x68", b"\x0B\x7C", b"\x44\x00", b"\x41\xE8", b"\x48\x41"]
                return self._m_lookup_table if hasattr(self, '_m_lookup_table') else None

            @property
            def value(self):
                """The two bytes that the tag byte expands to,
                based on the fixed lookup table.
                """
                if hasattr(self, '_m_value'):
                    return self._m_value if hasattr(self, '_m_value') else None

                self._m_value = self.lookup_table[(self.tag - 75)]
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
                if _on == 0:
                    self.body = Dcmp0.Chunk.ExtendedBody.JumpTableBody(self._io, self, self._root)
                    self.body._read()
                elif _on == 4:
                    self.body = Dcmp0.Chunk.ExtendedBody.DeltaEncoding16BitBody(self._io, self, self._root)
                    self.body._read()
                elif _on == 6:
                    self.body = Dcmp0.Chunk.ExtendedBody.DeltaEncoding32BitBody(self._io, self, self._root)
                    self.body._read()
                elif _on == 3:
                    self.body = Dcmp0.Chunk.ExtendedBody.RepeatBody(self.tag, self._io, self, self._root)
                    self.body._read()
                elif _on == 2:
                    self.body = Dcmp0.Chunk.ExtendedBody.RepeatBody(self.tag, self._io, self, self._root)
                    self.body._read()
                self._debug['body']['end'] = self._io.pos()

            class JumpTableBody(KaitaiStruct):
                """The body of a jump table chunk.
                
                This chunk generates parts of a segment loader jump table,
                in the format found in `'CODE' (0)` resources.
                It expands to the following data,
                with all non-constant numbers encoded as unsigned 16-bit big-endian integers:
                
                * `0x3f 0x3c` (push following segment number onto stack)
                * The segment number
                * `0xa9 0xf0` (`_LoadSeg` trap)
                * For each address:
                  * The address
                  * `0x3f 0x3c` (push following segment number onto stack)
                  * The segment number
                  * `0xa9 0xf0` (`_LoadSeg` trap)
                
                Note that this generates one jump table entry without an address before it,
                meaning that this address needs to be generated by the preceding chunk.
                All following jump table entries are generated with the addresses encoded in this chunk.
                """
                SEQ_FIELDS = ["segment_number_raw", "num_addresses_raw", "addresses_raw"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['segment_number_raw']['start'] = self._io.pos()
                    self.segment_number_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.segment_number_raw._read()
                    self._debug['segment_number_raw']['end'] = self._io.pos()
                    self._debug['num_addresses_raw']['start'] = self._io.pos()
                    self.num_addresses_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.num_addresses_raw._read()
                    self._debug['num_addresses_raw']['end'] = self._io.pos()
                    self._debug['addresses_raw']['start'] = self._io.pos()
                    self.addresses_raw = [None] * (self.num_addresses)
                    for i in range(self.num_addresses):
                        if not 'arr' in self._debug['addresses_raw']:
                            self._debug['addresses_raw']['arr'] = []
                        self._debug['addresses_raw']['arr'].append({'start': self._io.pos()})
                        _t_addresses_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                        _t_addresses_raw._read()
                        self.addresses_raw[i] = _t_addresses_raw
                        self._debug['addresses_raw']['arr'][i]['end'] = self._io.pos()

                    self._debug['addresses_raw']['end'] = self._io.pos()

                @property
                def segment_number(self):
                    """The segment number for all of the generated jump table entries.
                    
                    Although it is stored as a variable-length integer,
                    the segment number must be in the range `0x0 <= x <= 0xffff`,
                    i. e. an unsigned 16-bit integer.
                    """
                    if hasattr(self, '_m_segment_number'):
                        return self._m_segment_number if hasattr(self, '_m_segment_number') else None

                    self._m_segment_number = self.segment_number_raw.value
                    return self._m_segment_number if hasattr(self, '_m_segment_number') else None

                @property
                def num_addresses(self):
                    """The number of addresses stored in this chunk.
                    
                    This number must be greater than 0.
                    """
                    if hasattr(self, '_m_num_addresses'):
                        return self._m_num_addresses if hasattr(self, '_m_num_addresses') else None

                    self._m_num_addresses = self.num_addresses_raw.value
                    return self._m_num_addresses if hasattr(self, '_m_num_addresses') else None


            class RepeatBody(KaitaiStruct):
                """The body of a repeat chunk.
                
                This chunk expands to a 1-byte or 2-byte value repeated a number of times,
                i. e. it implements a form of run-length encoding.
                """
                SEQ_FIELDS = ["to_repeat_raw", "repeat_count_m1_raw"]
                def __init__(self, tag, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self.tag = tag
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
                def byte_count(self):
                    """The length in bytes of the value to be repeated.
                    Regardless of the byte count,
                    the value to be repeated is stored as a variable-length integer.
                    """
                    if hasattr(self, '_m_byte_count'):
                        return self._m_byte_count if hasattr(self, '_m_byte_count') else None

                    self._m_byte_count = (1 if self.tag == 2 else (2 if self.tag == 3 else -1))
                    return self._m_byte_count if hasattr(self, '_m_byte_count') else None

                @property
                def to_repeat(self):
                    """The value to repeat.
                    
                    Although it is stored as a variable-length integer,
                    this value must fit into an unsigned big-endian integer that is as long as `byte_count`,
                    i. e. either 8 or 16 bits.
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


            class DeltaEncoding16BitBody(KaitaiStruct):
                """The body of a 16-bit delta encoding chunk.
                
                This chunk expands to a sequence of 16-bit big-endian integer values.
                The first value is stored literally.
                All following values are stored as deltas relative to the previous value.
                """
                SEQ_FIELDS = ["first_value_raw", "num_deltas_raw", "deltas"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['first_value_raw']['start'] = self._io.pos()
                    self.first_value_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.first_value_raw._read()
                    self._debug['first_value_raw']['end'] = self._io.pos()
                    self._debug['num_deltas_raw']['start'] = self._io.pos()
                    self.num_deltas_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.num_deltas_raw._read()
                    self._debug['num_deltas_raw']['end'] = self._io.pos()
                    self._debug['deltas']['start'] = self._io.pos()
                    self.deltas = [None] * (self.num_deltas)
                    for i in range(self.num_deltas):
                        if not 'arr' in self._debug['deltas']:
                            self._debug['deltas']['arr'] = []
                        self._debug['deltas']['arr'].append({'start': self._io.pos()})
                        self.deltas[i] = self._io.read_s1()
                        self._debug['deltas']['arr'][i]['end'] = self._io.pos()

                    self._debug['deltas']['end'] = self._io.pos()

                @property
                def first_value(self):
                    """The first value in the sequence.
                    
                    Although it is stored as a variable-length integer,
                    this value must be in the range `-0x8000 <= x <= 0x7fff`,
                    i. e. a signed 16-bit integer.
                    """
                    if hasattr(self, '_m_first_value'):
                        return self._m_first_value if hasattr(self, '_m_first_value') else None

                    self._m_first_value = self.first_value_raw.value
                    return self._m_first_value if hasattr(self, '_m_first_value') else None

                @property
                def num_deltas(self):
                    """The number of deltas stored in this chunk.
                    
                    This number must not be negative.
                    """
                    if hasattr(self, '_m_num_deltas'):
                        return self._m_num_deltas if hasattr(self, '_m_num_deltas') else None

                    self._m_num_deltas = self.num_deltas_raw.value
                    return self._m_num_deltas if hasattr(self, '_m_num_deltas') else None


            class DeltaEncoding32BitBody(KaitaiStruct):
                """The body of a 32-bit delta encoding chunk.
                
                This chunk expands to a sequence of 32-bit big-endian integer values.
                The first value is stored literally.
                All following values are stored as deltas relative to the previous value.
                """
                SEQ_FIELDS = ["first_value_raw", "num_deltas_raw", "deltas_raw"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['first_value_raw']['start'] = self._io.pos()
                    self.first_value_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.first_value_raw._read()
                    self._debug['first_value_raw']['end'] = self._io.pos()
                    self._debug['num_deltas_raw']['start'] = self._io.pos()
                    self.num_deltas_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                    self.num_deltas_raw._read()
                    self._debug['num_deltas_raw']['end'] = self._io.pos()
                    self._debug['deltas_raw']['start'] = self._io.pos()
                    self.deltas_raw = [None] * (self.num_deltas)
                    for i in range(self.num_deltas):
                        if not 'arr' in self._debug['deltas_raw']:
                            self._debug['deltas_raw']['arr'] = []
                        self._debug['deltas_raw']['arr'].append({'start': self._io.pos()})
                        _t_deltas_raw = dcmp_variable_length_integer.DcmpVariableLengthInteger(self._io)
                        _t_deltas_raw._read()
                        self.deltas_raw[i] = _t_deltas_raw
                        self._debug['deltas_raw']['arr'][i]['end'] = self._io.pos()

                    self._debug['deltas_raw']['end'] = self._io.pos()

                @property
                def first_value(self):
                    """The first value in the sequence.
                    """
                    if hasattr(self, '_m_first_value'):
                        return self._m_first_value if hasattr(self, '_m_first_value') else None

                    self._m_first_value = self.first_value_raw.value
                    return self._m_first_value if hasattr(self, '_m_first_value') else None

                @property
                def num_deltas(self):
                    """The number of deltas stored in this chunk.
                    
                    This number must not be negative.
                    """
                    if hasattr(self, '_m_num_deltas'):
                        return self._m_num_deltas if hasattr(self, '_m_num_deltas') else None

                    self._m_num_deltas = self.num_deltas_raw.value
                    return self._m_num_deltas if hasattr(self, '_m_num_deltas') else None






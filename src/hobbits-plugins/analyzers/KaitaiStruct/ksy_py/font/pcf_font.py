# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import bytes_with_io
class PcfFont(KaitaiStruct):
    """Portable Compiled Format (PCF) font is a bitmap font format
    originating from X11 Window System. It matches BDF format (which is
    text-based) closely, but instead being binary and
    platform-independent (as opposed to previously used SNF binary
    format) due to introduced features to handle different endianness
    and bit order.
    
    The overall composition of the format is straightforward: it's more
    or less classic directory of type-offset-size pointers, pointing to
    what PCF format calls "tables". Each table carries a certain
    piece of information related to the font (metadata properties,
    metrics, bitmaps, mapping of glyphs to characters, etc).
    
    .. seealso::
       Source - https://fontforge.org/docs/techref/pcf-format.html
    """

    class Types(Enum):
        properties = 1
        accelerators = 2
        metrics = 4
        bitmaps = 8
        ink_metrics = 16
        bdf_encodings = 32
        swidths = 64
        glyph_names = 128
        bdf_accelerators = 256
    SEQ_FIELDS = ["magic", "num_tables", "tables"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['magic']['start'] = self._io.pos()
        self.magic = self._io.read_bytes(4)
        self._debug['magic']['end'] = self._io.pos()
        if not self.magic == b"\x01\x66\x63\x70":
            raise kaitaistruct.ValidationNotEqualError(b"\x01\x66\x63\x70", self.magic, self._io, u"/seq/0")
        self._debug['num_tables']['start'] = self._io.pos()
        self.num_tables = self._io.read_u4le()
        self._debug['num_tables']['end'] = self._io.pos()
        self._debug['tables']['start'] = self._io.pos()
        self.tables = [None] * (self.num_tables)
        for i in range(self.num_tables):
            if not 'arr' in self._debug['tables']:
                self._debug['tables']['arr'] = []
            self._debug['tables']['arr'].append({'start': self._io.pos()})
            _t_tables = PcfFont.Table(self._io, self, self._root)
            _t_tables._read()
            self.tables[i] = _t_tables
            self._debug['tables']['arr'][i]['end'] = self._io.pos()

        self._debug['tables']['end'] = self._io.pos()

    class Table(KaitaiStruct):
        """Table offers a offset + length pointer to a particular
        table. "Type" of table references certain enum. Applications can
        ignore enum values which they don't support.
        """
        SEQ_FIELDS = ["type", "format", "len_body", "ofs_body"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(PcfFont.Types, self._io.read_u4le())
            self._debug['type']['end'] = self._io.pos()
            self._debug['format']['start'] = self._io.pos()
            self.format = PcfFont.Format(self._io, self, self._root)
            self.format._read()
            self._debug['format']['end'] = self._io.pos()
            self._debug['len_body']['start'] = self._io.pos()
            self.len_body = self._io.read_u4le()
            self._debug['len_body']['end'] = self._io.pos()
            self._debug['ofs_body']['start'] = self._io.pos()
            self.ofs_body = self._io.read_u4le()
            self._debug['ofs_body']['end'] = self._io.pos()

        class Swidths(KaitaiStruct):
            """Table containing scalable widths of characters.
            
            .. seealso::
               Source - https://fontforge.org/docs/techref/pcf-format.html#the-scalable-widths-table
            """
            SEQ_FIELDS = ["format", "num_glyphs", "swidths"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['format']['start'] = self._io.pos()
                self.format = PcfFont.Format(self._io, self, self._root)
                self.format._read()
                self._debug['format']['end'] = self._io.pos()
                self._debug['num_glyphs']['start'] = self._io.pos()
                self.num_glyphs = self._io.read_u4le()
                self._debug['num_glyphs']['end'] = self._io.pos()
                self._debug['swidths']['start'] = self._io.pos()
                self.swidths = [None] * (self.num_glyphs)
                for i in range(self.num_glyphs):
                    if not 'arr' in self._debug['swidths']:
                        self._debug['swidths']['arr'] = []
                    self._debug['swidths']['arr'].append({'start': self._io.pos()})
                    self.swidths[i] = self._io.read_u4le()
                    self._debug['swidths']['arr'][i]['end'] = self._io.pos()

                self._debug['swidths']['end'] = self._io.pos()


        class Properties(KaitaiStruct):
            """Array of properties (key-value pairs), used to convey different X11
            settings of a font. Key is always an X font atom.
            
            .. seealso::
               Source - https://fontforge.org/docs/techref/pcf-format.html#properties-table
            """
            SEQ_FIELDS = ["format", "num_props", "props", "padding", "len_strings", "strings"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['format']['start'] = self._io.pos()
                self.format = PcfFont.Format(self._io, self, self._root)
                self.format._read()
                self._debug['format']['end'] = self._io.pos()
                self._debug['num_props']['start'] = self._io.pos()
                self.num_props = self._io.read_u4le()
                self._debug['num_props']['end'] = self._io.pos()
                self._debug['props']['start'] = self._io.pos()
                self.props = [None] * (self.num_props)
                for i in range(self.num_props):
                    if not 'arr' in self._debug['props']:
                        self._debug['props']['arr'] = []
                    self._debug['props']['arr'].append({'start': self._io.pos()})
                    _t_props = PcfFont.Table.Properties.Prop(self._io, self, self._root)
                    _t_props._read()
                    self.props[i] = _t_props
                    self._debug['props']['arr'][i]['end'] = self._io.pos()

                self._debug['props']['end'] = self._io.pos()
                self._debug['padding']['start'] = self._io.pos()
                self.padding = self._io.read_bytes((0 if (self.num_props & 3) == 0 else (4 - (self.num_props & 3))))
                self._debug['padding']['end'] = self._io.pos()
                self._debug['len_strings']['start'] = self._io.pos()
                self.len_strings = self._io.read_u4le()
                self._debug['len_strings']['end'] = self._io.pos()
                self._debug['strings']['start'] = self._io.pos()
                self._raw_strings = self._io.read_bytes(self.len_strings)
                _io__raw_strings = KaitaiStream(BytesIO(self._raw_strings))
                self.strings = bytes_with_io.BytesWithIo(_io__raw_strings)
                self.strings._read()
                self._debug['strings']['end'] = self._io.pos()

            class Prop(KaitaiStruct):
                """Property is a key-value pair, "key" being always a
                string and "value" being either a string or a 32-bit
                integer based on an additinal flag (`is_string`).
                
                Simple offset-based mechanism is employed to keep this
                type's sequence fixed-sized and thus have simple access
                to property key/value by index.
                """
                SEQ_FIELDS = ["ofs_name", "is_string", "value_or_ofs_value"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['ofs_name']['start'] = self._io.pos()
                    self.ofs_name = self._io.read_u4le()
                    self._debug['ofs_name']['end'] = self._io.pos()
                    self._debug['is_string']['start'] = self._io.pos()
                    self.is_string = self._io.read_u1()
                    self._debug['is_string']['end'] = self._io.pos()
                    self._debug['value_or_ofs_value']['start'] = self._io.pos()
                    self.value_or_ofs_value = self._io.read_u4le()
                    self._debug['value_or_ofs_value']['end'] = self._io.pos()

                @property
                def name(self):
                    """Name of the property addressed in the strings buffer.
                    """
                    if hasattr(self, '_m_name'):
                        return self._m_name if hasattr(self, '_m_name') else None

                    io = self._parent.strings._io
                    _pos = io.pos()
                    io.seek(self.ofs_name)
                    self._debug['_m_name']['start'] = io.pos()
                    self._m_name = (io.read_bytes_term(0, False, True, True)).decode(u"UTF-8")
                    self._debug['_m_name']['end'] = io.pos()
                    io.seek(_pos)
                    return self._m_name if hasattr(self, '_m_name') else None

                @property
                def str_value(self):
                    """Value of the property addressed in the strings
                    buffer, if this is a string value.
                    """
                    if hasattr(self, '_m_str_value'):
                        return self._m_str_value if hasattr(self, '_m_str_value') else None

                    if self.is_string != 0:
                        io = self._parent.strings._io
                        _pos = io.pos()
                        io.seek(self.value_or_ofs_value)
                        self._debug['_m_str_value']['start'] = io.pos()
                        self._m_str_value = (io.read_bytes_term(0, False, True, True)).decode(u"UTF-8")
                        self._debug['_m_str_value']['end'] = io.pos()
                        io.seek(_pos)

                    return self._m_str_value if hasattr(self, '_m_str_value') else None

                @property
                def int_value(self):
                    """Value of the property, if this is an integer value.
                    """
                    if hasattr(self, '_m_int_value'):
                        return self._m_int_value if hasattr(self, '_m_int_value') else None

                    if self.is_string == 0:
                        self._m_int_value = self.value_or_ofs_value

                    return self._m_int_value if hasattr(self, '_m_int_value') else None



        class BdfEncodings(KaitaiStruct):
            """Table that allows mapping of character codes to glyphs present in the
            font. Supports 1-byte and 2-byte character codes.
            
            Note that this mapping is agnostic to character encoding itself - it
            can represent ASCII, Unicode (ISO/IEC 10646), various single-byte
            national encodings, etc. If application cares about it, normally
            encoding will be specified in `properties` table, in the properties named
            `CHARSET_REGISTRY` / `CHARSET_ENCODING`.
            
            .. seealso::
               Source - https://fontforge.org/docs/techref/pcf-format.html#the-encoding-table
            """
            SEQ_FIELDS = ["format", "min_char_or_byte2", "max_char_or_byte2", "min_byte1", "max_byte1", "default_char", "glyph_indexes"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['format']['start'] = self._io.pos()
                self.format = PcfFont.Format(self._io, self, self._root)
                self.format._read()
                self._debug['format']['end'] = self._io.pos()
                self._debug['min_char_or_byte2']['start'] = self._io.pos()
                self.min_char_or_byte2 = self._io.read_u2le()
                self._debug['min_char_or_byte2']['end'] = self._io.pos()
                self._debug['max_char_or_byte2']['start'] = self._io.pos()
                self.max_char_or_byte2 = self._io.read_u2le()
                self._debug['max_char_or_byte2']['end'] = self._io.pos()
                self._debug['min_byte1']['start'] = self._io.pos()
                self.min_byte1 = self._io.read_u2le()
                self._debug['min_byte1']['end'] = self._io.pos()
                self._debug['max_byte1']['start'] = self._io.pos()
                self.max_byte1 = self._io.read_u2le()
                self._debug['max_byte1']['end'] = self._io.pos()
                self._debug['default_char']['start'] = self._io.pos()
                self.default_char = self._io.read_u2le()
                self._debug['default_char']['end'] = self._io.pos()
                self._debug['glyph_indexes']['start'] = self._io.pos()
                self.glyph_indexes = [None] * ((((self.max_char_or_byte2 - self.min_char_or_byte2) + 1) * ((self.max_byte1 - self.min_byte1) + 1)))
                for i in range((((self.max_char_or_byte2 - self.min_char_or_byte2) + 1) * ((self.max_byte1 - self.min_byte1) + 1))):
                    if not 'arr' in self._debug['glyph_indexes']:
                        self._debug['glyph_indexes']['arr'] = []
                    self._debug['glyph_indexes']['arr'].append({'start': self._io.pos()})
                    self.glyph_indexes[i] = self._io.read_u2le()
                    self._debug['glyph_indexes']['arr'][i]['end'] = self._io.pos()

                self._debug['glyph_indexes']['end'] = self._io.pos()


        class GlyphNames(KaitaiStruct):
            """Table containing character names for every glyph.
            
            .. seealso::
               Source - https://fontforge.org/docs/techref/pcf-format.html#the-glyph-names-table
            """
            SEQ_FIELDS = ["format", "num_glyphs", "names", "len_strings", "strings"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['format']['start'] = self._io.pos()
                self.format = PcfFont.Format(self._io, self, self._root)
                self.format._read()
                self._debug['format']['end'] = self._io.pos()
                self._debug['num_glyphs']['start'] = self._io.pos()
                self.num_glyphs = self._io.read_u4le()
                self._debug['num_glyphs']['end'] = self._io.pos()
                self._debug['names']['start'] = self._io.pos()
                self.names = [None] * (self.num_glyphs)
                for i in range(self.num_glyphs):
                    if not 'arr' in self._debug['names']:
                        self._debug['names']['arr'] = []
                    self._debug['names']['arr'].append({'start': self._io.pos()})
                    _t_names = PcfFont.Table.GlyphNames.StringRef(self._io, self, self._root)
                    _t_names._read()
                    self.names[i] = _t_names
                    self._debug['names']['arr'][i]['end'] = self._io.pos()

                self._debug['names']['end'] = self._io.pos()
                self._debug['len_strings']['start'] = self._io.pos()
                self.len_strings = self._io.read_u4le()
                self._debug['len_strings']['end'] = self._io.pos()
                self._debug['strings']['start'] = self._io.pos()
                self._raw_strings = self._io.read_bytes(self.len_strings)
                _io__raw_strings = KaitaiStream(BytesIO(self._raw_strings))
                self.strings = bytes_with_io.BytesWithIo(_io__raw_strings)
                self.strings._read()
                self._debug['strings']['end'] = self._io.pos()

            class StringRef(KaitaiStruct):
                SEQ_FIELDS = ["ofs_string"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['ofs_string']['start'] = self._io.pos()
                    self.ofs_string = self._io.read_u4le()
                    self._debug['ofs_string']['end'] = self._io.pos()

                @property
                def value(self):
                    if hasattr(self, '_m_value'):
                        return self._m_value if hasattr(self, '_m_value') else None

                    io = self._parent.strings._io
                    _pos = io.pos()
                    io.seek(self.ofs_string)
                    self._debug['_m_value']['start'] = io.pos()
                    self._m_value = (io.read_bytes_term(0, False, True, True)).decode(u"UTF-8")
                    self._debug['_m_value']['end'] = io.pos()
                    io.seek(_pos)
                    return self._m_value if hasattr(self, '_m_value') else None



        class Bitmaps(KaitaiStruct):
            """Table containing uncompressed glyph bitmaps.
            
            .. seealso::
               Source - https://fontforge.org/docs/techref/pcf-format.html#the-bitmap-table
            """
            SEQ_FIELDS = ["format", "num_glyphs", "offsets", "bitmap_sizes"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['format']['start'] = self._io.pos()
                self.format = PcfFont.Format(self._io, self, self._root)
                self.format._read()
                self._debug['format']['end'] = self._io.pos()
                self._debug['num_glyphs']['start'] = self._io.pos()
                self.num_glyphs = self._io.read_u4le()
                self._debug['num_glyphs']['end'] = self._io.pos()
                self._debug['offsets']['start'] = self._io.pos()
                self.offsets = [None] * (self.num_glyphs)
                for i in range(self.num_glyphs):
                    if not 'arr' in self._debug['offsets']:
                        self._debug['offsets']['arr'] = []
                    self._debug['offsets']['arr'].append({'start': self._io.pos()})
                    self.offsets[i] = self._io.read_u4le()
                    self._debug['offsets']['arr'][i]['end'] = self._io.pos()

                self._debug['offsets']['end'] = self._io.pos()
                self._debug['bitmap_sizes']['start'] = self._io.pos()
                self.bitmap_sizes = [None] * (4)
                for i in range(4):
                    if not 'arr' in self._debug['bitmap_sizes']:
                        self._debug['bitmap_sizes']['arr'] = []
                    self._debug['bitmap_sizes']['arr'].append({'start': self._io.pos()})
                    self.bitmap_sizes[i] = self._io.read_u4le()
                    self._debug['bitmap_sizes']['arr'][i]['end'] = self._io.pos()

                self._debug['bitmap_sizes']['end'] = self._io.pos()


        @property
        def body(self):
            if hasattr(self, '_m_body'):
                return self._m_body if hasattr(self, '_m_body') else None

            _pos = self._io.pos()
            self._io.seek(self.ofs_body)
            self._debug['_m_body']['start'] = self._io.pos()
            _on = self.type
            if _on == PcfFont.Types.properties:
                self._raw__m_body = self._io.read_bytes(self.len_body)
                _io__raw__m_body = KaitaiStream(BytesIO(self._raw__m_body))
                self._m_body = PcfFont.Table.Properties(_io__raw__m_body, self, self._root)
                self._m_body._read()
            elif _on == PcfFont.Types.bdf_encodings:
                self._raw__m_body = self._io.read_bytes(self.len_body)
                _io__raw__m_body = KaitaiStream(BytesIO(self._raw__m_body))
                self._m_body = PcfFont.Table.BdfEncodings(_io__raw__m_body, self, self._root)
                self._m_body._read()
            elif _on == PcfFont.Types.swidths:
                self._raw__m_body = self._io.read_bytes(self.len_body)
                _io__raw__m_body = KaitaiStream(BytesIO(self._raw__m_body))
                self._m_body = PcfFont.Table.Swidths(_io__raw__m_body, self, self._root)
                self._m_body._read()
            elif _on == PcfFont.Types.glyph_names:
                self._raw__m_body = self._io.read_bytes(self.len_body)
                _io__raw__m_body = KaitaiStream(BytesIO(self._raw__m_body))
                self._m_body = PcfFont.Table.GlyphNames(_io__raw__m_body, self, self._root)
                self._m_body._read()
            elif _on == PcfFont.Types.bitmaps:
                self._raw__m_body = self._io.read_bytes(self.len_body)
                _io__raw__m_body = KaitaiStream(BytesIO(self._raw__m_body))
                self._m_body = PcfFont.Table.Bitmaps(_io__raw__m_body, self, self._root)
                self._m_body._read()
            else:
                self._m_body = self._io.read_bytes(self.len_body)
            self._debug['_m_body']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_body if hasattr(self, '_m_body') else None


    class Format(KaitaiStruct):
        """Table format specifier, always 4 bytes. Original implementation treats
        it as always little-endian and makes liberal use of bitmasking to parse
        various parts of it.
        
        TODO: this format specification recognizes endianness and bit
        order format bits, but it does not really takes any parsing
        decisions based on them.
        
        .. seealso::
           Source - https://fontforge.org/docs/techref/pcf-format.html#file-header
        """
        SEQ_FIELDS = ["padding1", "scan_unit_mask", "is_most_significant_bit_first", "is_big_endian", "glyph_pad_mask", "format", "padding"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['padding1']['start'] = self._io.pos()
            self.padding1 = self._io.read_bits_int_be(2)
            self._debug['padding1']['end'] = self._io.pos()
            self._debug['scan_unit_mask']['start'] = self._io.pos()
            self.scan_unit_mask = self._io.read_bits_int_be(2)
            self._debug['scan_unit_mask']['end'] = self._io.pos()
            self._debug['is_most_significant_bit_first']['start'] = self._io.pos()
            self.is_most_significant_bit_first = self._io.read_bits_int_be(1) != 0
            self._debug['is_most_significant_bit_first']['end'] = self._io.pos()
            self._debug['is_big_endian']['start'] = self._io.pos()
            self.is_big_endian = self._io.read_bits_int_be(1) != 0
            self._debug['is_big_endian']['end'] = self._io.pos()
            self._debug['glyph_pad_mask']['start'] = self._io.pos()
            self.glyph_pad_mask = self._io.read_bits_int_be(2)
            self._debug['glyph_pad_mask']['end'] = self._io.pos()
            self._io.align_to_byte()
            self._debug['format']['start'] = self._io.pos()
            self.format = self._io.read_u1()
            self._debug['format']['end'] = self._io.pos()
            self._debug['padding']['start'] = self._io.pos()
            self.padding = self._io.read_u2le()
            self._debug['padding']['end'] = self._io.pos()




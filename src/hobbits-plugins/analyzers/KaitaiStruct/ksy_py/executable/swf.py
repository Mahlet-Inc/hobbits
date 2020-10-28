# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections
import zlib


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Swf(KaitaiStruct):
    """SWF files are used by Adobe Flash (AKA Shockwave Flash, Macromedia
    Flash) to encode rich interactive multimedia content and are,
    essentially, a container for special bytecode instructions to play
    back that content. In early 2000s, it was dominant rich multimedia
    web format (.swf files were integrated into web pages and played
    back with a browser plugin), but its usage largely declined in
    2010s, as HTML5 and performant browser-native solutions
    (i.e. JavaScript engines and graphical approaches, such as WebGL)
    emerged.
    
    There are a lot of versions of SWF (~36), format is somewhat
    documented by Adobe.
    
    .. seealso::
       Source - https://www.adobe.com/content/dam/acom/en/devnet/pdf/swf-file-format-spec.pdf
    """

    class Compressions(Enum):
        zlib = 67
        none = 70
        lzma = 90

    class TagType(Enum):
        end_of_file = 0
        place_object = 4
        remove_object = 5
        set_background_color = 9
        define_sound = 14
        place_object2 = 26
        remove_object2 = 28
        frame_label = 43
        export_assets = 56
        script_limits = 65
        file_attributes = 69
        place_object3 = 70
        symbol_class = 76
        metadata = 77
        define_scaling_grid = 78
        do_abc = 82
        define_scene_and_frame_label_data = 86
    SEQ_FIELDS = ["compression", "signature", "version", "len_file", "plain_body", "zlib_body"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['compression']['start'] = self._io.pos()
        self.compression = KaitaiStream.resolve_enum(Swf.Compressions, self._io.read_u1())
        self._debug['compression']['end'] = self._io.pos()
        self._debug['signature']['start'] = self._io.pos()
        self.signature = self._io.read_bytes(2)
        self._debug['signature']['end'] = self._io.pos()
        if not self.signature == b"\x57\x53":
            raise kaitaistruct.ValidationNotEqualError(b"\x57\x53", self.signature, self._io, u"/seq/1")
        self._debug['version']['start'] = self._io.pos()
        self.version = self._io.read_u1()
        self._debug['version']['end'] = self._io.pos()
        self._debug['len_file']['start'] = self._io.pos()
        self.len_file = self._io.read_u4le()
        self._debug['len_file']['end'] = self._io.pos()
        if self.compression == Swf.Compressions.none:
            self._debug['plain_body']['start'] = self._io.pos()
            self._raw_plain_body = self._io.read_bytes_full()
            _io__raw_plain_body = KaitaiStream(BytesIO(self._raw_plain_body))
            self.plain_body = Swf.SwfBody(_io__raw_plain_body, self, self._root)
            self.plain_body._read()
            self._debug['plain_body']['end'] = self._io.pos()

        if self.compression == Swf.Compressions.zlib:
            self._debug['zlib_body']['start'] = self._io.pos()
            self._raw__raw_zlib_body = self._io.read_bytes_full()
            self._raw_zlib_body = zlib.decompress(self._raw__raw_zlib_body)
            _io__raw_zlib_body = KaitaiStream(BytesIO(self._raw_zlib_body))
            self.zlib_body = Swf.SwfBody(_io__raw_zlib_body, self, self._root)
            self.zlib_body._read()
            self._debug['zlib_body']['end'] = self._io.pos()


    class Rgb(KaitaiStruct):
        SEQ_FIELDS = ["r", "g", "b"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['r']['start'] = self._io.pos()
            self.r = self._io.read_u1()
            self._debug['r']['end'] = self._io.pos()
            self._debug['g']['start'] = self._io.pos()
            self.g = self._io.read_u1()
            self._debug['g']['end'] = self._io.pos()
            self._debug['b']['start'] = self._io.pos()
            self.b = self._io.read_u1()
            self._debug['b']['end'] = self._io.pos()


    class DoAbcBody(KaitaiStruct):
        SEQ_FIELDS = ["flags", "name", "abcdata"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['flags']['start'] = self._io.pos()
            self.flags = self._io.read_u4le()
            self._debug['flags']['end'] = self._io.pos()
            self._debug['name']['start'] = self._io.pos()
            self.name = (self._io.read_bytes_term(0, False, True, True)).decode(u"ASCII")
            self._debug['name']['end'] = self._io.pos()
            self._debug['abcdata']['start'] = self._io.pos()
            self.abcdata = self._io.read_bytes_full()
            self._debug['abcdata']['end'] = self._io.pos()


    class SwfBody(KaitaiStruct):
        SEQ_FIELDS = ["rect", "frame_rate", "frame_count", "file_attributes_tag", "tags"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['rect']['start'] = self._io.pos()
            self.rect = Swf.Rect(self._io, self, self._root)
            self.rect._read()
            self._debug['rect']['end'] = self._io.pos()
            self._debug['frame_rate']['start'] = self._io.pos()
            self.frame_rate = self._io.read_u2le()
            self._debug['frame_rate']['end'] = self._io.pos()
            self._debug['frame_count']['start'] = self._io.pos()
            self.frame_count = self._io.read_u2le()
            self._debug['frame_count']['end'] = self._io.pos()
            if self._root.version >= 8:
                self._debug['file_attributes_tag']['start'] = self._io.pos()
                self.file_attributes_tag = Swf.Tag(self._io, self, self._root)
                self.file_attributes_tag._read()
                self._debug['file_attributes_tag']['end'] = self._io.pos()

            self._debug['tags']['start'] = self._io.pos()
            self.tags = []
            i = 0
            while not self._io.is_eof():
                if not 'arr' in self._debug['tags']:
                    self._debug['tags']['arr'] = []
                self._debug['tags']['arr'].append({'start': self._io.pos()})
                _t_tags = Swf.Tag(self._io, self, self._root)
                _t_tags._read()
                self.tags.append(_t_tags)
                self._debug['tags']['arr'][len(self.tags) - 1]['end'] = self._io.pos()
                i += 1

            self._debug['tags']['end'] = self._io.pos()


    class Rect(KaitaiStruct):
        SEQ_FIELDS = ["b1", "skip"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['b1']['start'] = self._io.pos()
            self.b1 = self._io.read_u1()
            self._debug['b1']['end'] = self._io.pos()
            self._debug['skip']['start'] = self._io.pos()
            self.skip = self._io.read_bytes(self.num_bytes)
            self._debug['skip']['end'] = self._io.pos()

        @property
        def num_bits(self):
            if hasattr(self, '_m_num_bits'):
                return self._m_num_bits if hasattr(self, '_m_num_bits') else None

            self._m_num_bits = (self.b1 >> 3)
            return self._m_num_bits if hasattr(self, '_m_num_bits') else None

        @property
        def num_bytes(self):
            if hasattr(self, '_m_num_bytes'):
                return self._m_num_bytes if hasattr(self, '_m_num_bytes') else None

            self._m_num_bytes = (((self.num_bits * 4) - 3) + 7) // 8
            return self._m_num_bytes if hasattr(self, '_m_num_bytes') else None


    class Tag(KaitaiStruct):
        SEQ_FIELDS = ["record_header", "tag_body"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['record_header']['start'] = self._io.pos()
            self.record_header = Swf.RecordHeader(self._io, self, self._root)
            self.record_header._read()
            self._debug['record_header']['end'] = self._io.pos()
            self._debug['tag_body']['start'] = self._io.pos()
            _on = self.record_header.tag_type
            if _on == Swf.TagType.define_sound:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.DefineSoundBody(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            elif _on == Swf.TagType.set_background_color:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.Rgb(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            elif _on == Swf.TagType.script_limits:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.ScriptLimitsBody(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            elif _on == Swf.TagType.do_abc:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.DoAbcBody(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            elif _on == Swf.TagType.export_assets:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.SymbolClassBody(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            elif _on == Swf.TagType.symbol_class:
                self._raw_tag_body = self._io.read_bytes(self.record_header.len)
                _io__raw_tag_body = KaitaiStream(BytesIO(self._raw_tag_body))
                self.tag_body = Swf.SymbolClassBody(_io__raw_tag_body, self, self._root)
                self.tag_body._read()
            else:
                self.tag_body = self._io.read_bytes(self.record_header.len)
            self._debug['tag_body']['end'] = self._io.pos()


    class SymbolClassBody(KaitaiStruct):
        SEQ_FIELDS = ["num_symbols", "symbols"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['num_symbols']['start'] = self._io.pos()
            self.num_symbols = self._io.read_u2le()
            self._debug['num_symbols']['end'] = self._io.pos()
            self._debug['symbols']['start'] = self._io.pos()
            self.symbols = [None] * (self.num_symbols)
            for i in range(self.num_symbols):
                if not 'arr' in self._debug['symbols']:
                    self._debug['symbols']['arr'] = []
                self._debug['symbols']['arr'].append({'start': self._io.pos()})
                _t_symbols = Swf.SymbolClassBody.Symbol(self._io, self, self._root)
                _t_symbols._read()
                self.symbols[i] = _t_symbols
                self._debug['symbols']['arr'][i]['end'] = self._io.pos()

            self._debug['symbols']['end'] = self._io.pos()

        class Symbol(KaitaiStruct):
            SEQ_FIELDS = ["tag", "name"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['tag']['start'] = self._io.pos()
                self.tag = self._io.read_u2le()
                self._debug['tag']['end'] = self._io.pos()
                self._debug['name']['start'] = self._io.pos()
                self.name = (self._io.read_bytes_term(0, False, True, True)).decode(u"ASCII")
                self._debug['name']['end'] = self._io.pos()



    class DefineSoundBody(KaitaiStruct):

        class SamplingRates(Enum):
            rate_5_5_khz = 0
            rate_11_khz = 1
            rate_22_khz = 2
            rate_44_khz = 3

        class Bps(Enum):
            sound_8_bit = 0
            sound_16_bit = 1

        class Channels(Enum):
            mono = 0
            stereo = 1
        SEQ_FIELDS = ["id", "format", "sampling_rate", "bits_per_sample", "num_channels", "num_samples"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['id']['start'] = self._io.pos()
            self.id = self._io.read_u2le()
            self._debug['id']['end'] = self._io.pos()
            self._debug['format']['start'] = self._io.pos()
            self.format = self._io.read_bits_int_be(4)
            self._debug['format']['end'] = self._io.pos()
            self._debug['sampling_rate']['start'] = self._io.pos()
            self.sampling_rate = KaitaiStream.resolve_enum(Swf.DefineSoundBody.SamplingRates, self._io.read_bits_int_be(2))
            self._debug['sampling_rate']['end'] = self._io.pos()
            self._debug['bits_per_sample']['start'] = self._io.pos()
            self.bits_per_sample = KaitaiStream.resolve_enum(Swf.DefineSoundBody.Bps, self._io.read_bits_int_be(1))
            self._debug['bits_per_sample']['end'] = self._io.pos()
            self._debug['num_channels']['start'] = self._io.pos()
            self.num_channels = KaitaiStream.resolve_enum(Swf.DefineSoundBody.Channels, self._io.read_bits_int_be(1))
            self._debug['num_channels']['end'] = self._io.pos()
            self._io.align_to_byte()
            self._debug['num_samples']['start'] = self._io.pos()
            self.num_samples = self._io.read_u4le()
            self._debug['num_samples']['end'] = self._io.pos()


    class RecordHeader(KaitaiStruct):
        SEQ_FIELDS = ["tag_code_and_length", "big_len"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['tag_code_and_length']['start'] = self._io.pos()
            self.tag_code_and_length = self._io.read_u2le()
            self._debug['tag_code_and_length']['end'] = self._io.pos()
            if self.small_len == 63:
                self._debug['big_len']['start'] = self._io.pos()
                self.big_len = self._io.read_s4le()
                self._debug['big_len']['end'] = self._io.pos()


        @property
        def tag_type(self):
            if hasattr(self, '_m_tag_type'):
                return self._m_tag_type if hasattr(self, '_m_tag_type') else None

            self._m_tag_type = KaitaiStream.resolve_enum(Swf.TagType, (self.tag_code_and_length >> 6))
            return self._m_tag_type if hasattr(self, '_m_tag_type') else None

        @property
        def small_len(self):
            if hasattr(self, '_m_small_len'):
                return self._m_small_len if hasattr(self, '_m_small_len') else None

            self._m_small_len = (self.tag_code_and_length & 63)
            return self._m_small_len if hasattr(self, '_m_small_len') else None

        @property
        def len(self):
            if hasattr(self, '_m_len'):
                return self._m_len if hasattr(self, '_m_len') else None

            self._m_len = (self.big_len if self.small_len == 63 else self.small_len)
            return self._m_len if hasattr(self, '_m_len') else None


    class ScriptLimitsBody(KaitaiStruct):
        SEQ_FIELDS = ["max_recursion_depth", "script_timeout_seconds"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['max_recursion_depth']['start'] = self._io.pos()
            self.max_recursion_depth = self._io.read_u2le()
            self._debug['max_recursion_depth']['end'] = self._io.pos()
            self._debug['script_timeout_seconds']['start'] = self._io.pos()
            self.script_timeout_seconds = self._io.read_u2le()
            self._debug['script_timeout_seconds']['end'] = self._io.pos()



